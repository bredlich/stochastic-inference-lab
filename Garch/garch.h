// GARCH(1,1) volatility model: simulation, estimation and forecasting.
// Date: 08/10/2026

#pragma once

#include "../ar1/ar1.h" // readClosingPrices, computeLogReturns
#include <vector>
#include <random>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdexcept>

struct GarchPath {
    std::vector<double> returns;
    std::vector<double> variances; // variances[t] is the variance of returns[t]
};

struct GarchFit {
    double mean;
    double omega;
    double alpha;
    double beta;
    double logLikelihood;
};

inline double sampleMean(const std::vector<double>& x) {
    double sum = 0.0;
    for (double v : x) sum += v;
    return sum / x.size();
}

inline double sampleVariance(const std::vector<double>& x) {
    double m = sampleMean(x);
    double sum = 0.0;
    for (double v : x) sum += (v - m) * (v - m);
    return sum / x.size();
}

inline double unconditionalVariance(double omega, double alpha, double beta) {
    return omega / (1.0 - alpha - beta);
}

// Simulates eps_t = sqrt(h_t) * z_t with h_t = omega + alpha*eps_{t-1}^2 + beta*h_{t-1}.
// The path starts at the unconditional variance, so it begins at its long run level.
inline GarchPath simulateGARCH(double omega, double alpha, double beta,
    int n, std::mt19937& gen) {
    std::normal_distribution<double> normal(0.0, 1.0);
    GarchPath path;
    path.returns.resize(n);
    path.variances.resize(n);

    double h = unconditionalVariance(omega, alpha, beta);
    for (int t = 0; t < n; ++t) {
        path.variances[t] = h;
        double eps = std::sqrt(h) * normal(gen);
        path.returns[t] = eps;
        h = omega + alpha * eps * eps + beta * h;
    }
    return path;
}

// Runs the variance recursion over a series of shocks. h[t] is the variance
// of eps[t] given everything up to t-1, and h0 is the starting variance.
inline std::vector<double> conditionalVariances(const std::vector<double>& eps,
    double omega, double alpha,
    double beta, double h0) {
    std::vector<double> h(eps.size());
    double current = h0;
    for (size_t t = 0; t < eps.size(); ++t) {
        h[t] = current;
        current = omega + alpha * eps[t] * eps[t] + beta * h[t];
    }
    return h;
}

// One step ahead variance after the last observation.
inline double nextVariance(const std::vector<double>& eps, const std::vector<double>& h,
    double omega, double alpha, double beta) {
    return omega + alpha * eps.back() * eps.back() + beta * h.back();
}

inline double gaussianLogLikelihood(const std::vector<double>& eps,
    const std::vector<double>& h) {
    const double pi = 3.14159265358979323846;
    double ll = 0.0;
    for (size_t t = 0; t < eps.size(); ++t) {
        ll += -0.5 * (std::log(2.0 * pi) + std::log(h[t]) + eps[t] * eps[t] / h[t]);
    }
    return ll;
}

// Fits GARCH(1,1) by maximising the Gaussian likelihood over (alpha, beta).
// Variance targeting sets omega = v * (1 - alpha - beta), where v is the sample
// variance, which leaves two parameters. A coarse grid search finds the right
// region, then a fine grid refines it. It is slower than a gradient method, but
// it needs no starting guess and no optimiser.
inline GarchFit fitGARCH(const std::vector<double>& returns) {
    double mu = sampleMean(returns);
    std::vector<double> eps(returns.size());
    for (size_t t = 0; t < returns.size(); ++t) eps[t] = returns[t] - mu;
    double v = sampleVariance(eps);

    GarchFit best{ mu, 0.0, 0.0, 0.0, -std::numeric_limits<double>::infinity() };

    auto evaluate = [&](double a, double b) {
        if (a <= 0.0 || b < 0.0 || a + b >= 0.999) return; // keep the model stationary
        double omega = v * (1.0 - a - b);
        std::vector<double> h = conditionalVariances(eps, omega, a, b, v);
        double ll = gaussianLogLikelihood(eps, h);
        if (ll > best.logLikelihood) best = { mu, omega, a, b, ll };
        };

    for (int i = 1; i <= 30; ++i) {
        for (int j = 0; j <= 99; ++j) {
            evaluate(0.01 * i, 0.01 * j);
        }
    }

    double a0 = best.alpha;
    double b0 = best.beta;
    for (int i = -15; i <= 15; ++i) {
        for (int j = -15; j <= 15; ++j) {
            evaluate(a0 + 0.001 * i, b0 + 0.001 * j);
        }
    }
    return best;
}

inline double autocorrelation(const std::vector<double>& x, int lag) {
    double m = sampleMean(x);
    double num = 0.0;
    double den = 0.0;
    for (size_t t = 0; t < x.size(); ++t) den += (x[t] - m) * (x[t] - m);
    for (size_t t = lag; t < x.size(); ++t) num += (x[t] - m) * (x[t - lag] - m);
    return num / den;
}

inline std::vector<double> squared(const std::vector<double>& x) {
    std::vector<double> out(x.size());
    for (size_t t = 0; t < x.size(); ++t) out[t] = x[t] * x[t];
    return out;
}

// Average forecast variance over the next `horizon` days. The forecast for
// k days ahead is V + (alpha+beta)^(k-1) * (h_next - V), which decays from the
// current variance back to the long run variance V.
inline double averageForecastVariance(double omega, double alpha, double beta,
    double hNext, int horizon) {
    double persistence = alpha + beta;
    double V = unconditionalVariance(omega, alpha, beta);
    double sum = 0.0;
    for (int k = 1; k <= horizon; ++k) {
        sum += V + std::pow(persistence, k - 1) * (hNext - V);
    }
    return sum / horizon;
}

// Forecast loss functions, lower is better. QLIKE is the standard loss for
// variance forecasts, since it is less distorted by a few extreme days than
// the mean squared error of squared returns.
inline double qlikeLoss(const std::vector<double>& eps, const std::vector<double>& h) {
    double sum = 0.0;
    for (size_t t = 0; t < eps.size(); ++t) {
        sum += std::log(h[t]) + eps[t] * eps[t] / h[t];
    }
    return sum / eps.size();
}

inline double squaredReturnMSE(const std::vector<double>& eps, const std::vector<double>& h) {
    double sum = 0.0;
    for (size_t t = 0; t < eps.size(); ++t) {
        double diff = eps[t] * eps[t] - h[t];
        sum += diff * diff;
    }
    return sum / eps.size();
}
