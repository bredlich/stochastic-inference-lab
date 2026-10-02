// AR(1) process: simulation, OLS estimation, and theoretical moments.
// Date: 02/10/2026

#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <limits>

// Simulates a single AR(1) path: X_t = c + phi * X_{t-1} + eps_t,
// eps_t ~ N(0, sigma^2). Starts at X_0 = c / (1 - phi) (the theoretical
// mean), so the path starts at its stationary level rather than at 0.
inline std::vector<double> simulateAR1Path(double phi, double c, double sigma,
    int n, std::mt19937& gen,
    double x0 = std::numeric_limits<double>::quiet_NaN()) {
    // MSVC's debug STL asserts sigma > 0 for normal_distribution, so a
    // sigma of exactly 0 (used to test deterministic, noiseless recovery)
    // is handled separately rather than passed into the distribution.
    bool noiseless = (sigma == 0.0);
    std::normal_distribution<double> noise(0.0, noiseless ? 1.0 : sigma);

    std::vector<double> path(n + 1);
    // By default the path starts at the stationary mean. Note that with
    // zero noise this makes every subsequent value identical (the process
    // is already at its fixed point), giving a constant series with zero
    // variance; OLS then divides by zero and returns NaN. x0 lets a caller
    // (e.g. a test of noiseless recovery) start away from equilibrium so
    // the deterministic path still has variation to regress on.
    path[0] = std::isnan(x0) ? (c / (1.0 - phi)) : x0;
    for (int t = 1; t <= n; ++t) {
        double eps = noiseless ? 0.0 : noise(gen);
        path[t] = c + phi * path[t - 1] + eps;
    }
    return path;
}

// Ordinary least squares estimate of (c, phi) by regressing X_t on X_{t-1}.
// Returns {c_hat, phi_hat}.
inline std::pair<double, double> estimateAR1(const std::vector<double>& series) {
    int n = static_cast<int>(series.size()) - 1;
    if (n < 2) {
        throw std::invalid_argument("Series too short to estimate AR(1) parameters");
    }

    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    for (int t = 1; t <= n; ++t) {
        double x = series[t - 1]; // lagged value
        double y = series[t];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }

    double meanX = sumX / n;
    double meanY = sumY / n;
    double phiHat = (sumXY - n * meanX * meanY) / (sumXX - n * meanX * meanX);
    double cHat = meanY - phiHat * meanX;

    return { cHat, phiHat };
}

inline double theoreticalMean(double phi, double c) {
    return c / (1.0 - phi);
}

inline double theoreticalVariance(double phi, double sigma) {
    return (sigma * sigma) / (1.0 - phi * phi);
}

// Reads a single column of closing prices from a CSV with a header row
// and a "Close" column (matches data/ftse100.csv used elsewhere in this repo).
inline std::vector<double> readClosingPrices(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<double> prices;
    std::string line;
    std::getline(file, line); // header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> cells;
        while (std::getline(ss, cell, ',')) {
            cells.push_back(cell);
        }
        if (cells.size() >= 2 && !cells[1].empty()) {
            try {
                prices.push_back(std::stod(cells[1]));
            }
            catch (...) {
                // skip malformed rows
            }
        }
    }
    return prices;
}

inline std::vector<double> computeLogReturns(const std::vector<double>& prices) {
    std::vector<double> returns;
    returns.reserve(prices.size() - 1);
    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back(std::log(prices[i] / prices[i - 1]));
    }
    return returns;
}

// One-step-ahead forecast: X_hat_{t+1} = c_hat + phi_hat * X_t
inline double forecastNextStep(double cHat, double phiHat, double currentValue) {
    return cHat + phiHat * currentValue;
}

// Root-mean-squared error between two equal-length series.
inline double computeRMSE(const std::vector<double>& actual,
    const std::vector<double>& predicted) {
    if (actual.size() != predicted.size() || actual.empty()) {
        throw std::invalid_argument("Series must be non-empty and of equal length");
    }
    double sumSq = 0.0;
    for (size_t i = 0; i < actual.size(); ++i) {
        double diff = actual[i] - predicted[i];
        sumSq += diff * diff;
    }
    return std::sqrt(sumSq / actual.size());
}
