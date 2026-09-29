// Hurst exponent estimation via Rescaled Range (R/S) analysis.
// Given a time series of increments, estimates the Hurst exponent H by
// computing the rescaled range statistic across multiple window sizes
// and fitting a line to log(n) vs log(R/S), whose slope is the estimate.
// Date: 29/09/2026

#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <utility>

// Computes the rescaled range (R/S) statistic for a single window of increments
inline double computeRS(const std::vector<double>& increments) {
    int n = static_cast<int>(increments.size());
    double mean = std::accumulate(increments.begin(), increments.end(), 0.0) / n;

    std::vector<double> Z(n);
    double cumSum = 0.0;
    for (int i = 0; i < n; i++) {
        cumSum += (increments[i] - mean);
        Z[i] = cumSum;
    }

    double R = *std::max_element(Z.begin(), Z.end()) - *std::min_element(Z.begin(), Z.end());

    double variance = 0.0;
    for (double x : increments) variance += (x - mean) * (x - mean);
    variance /= (n - 1);
    double S = std::sqrt(variance);

    if (S == 0.0) return 0.0;
    return R / S;
}

// Averages R/S across all non-overlapping windows of size n within the full series
inline double averageRSForWindowSize(const std::vector<double>& increments, int n) {
    int numWindows = static_cast<int>(increments.size()) / n;
    if (numWindows == 0) return 0.0;

    double sum = 0.0;
    int count = 0;
    for (int w = 0; w < numWindows; w++) {
        std::vector<double> window(increments.begin() + w * n, increments.begin() + (w + 1) * n);
        double rs = computeRS(window);
        if (rs > 0.0) {
            sum += rs;
            count++;
        }
    }
    return count > 0 ? sum / count : 0.0;
}

// Least-squares linear regression y = a + b*x, returns {a, b}
inline std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    int n = static_cast<int>(x.size());
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0, sumX2 = 0.0;
    for (int i = 0; i < n; i++) {
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    double b = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double a = (sumY - b * sumX) / n;
    return { a, b };
}

// Runs full R/S analysis across a range of window sizes; the slope of
// log(R/S) vs log(n) is the estimated Hurst exponent
inline double estimateHurstExponent(const std::vector<double>& increments,
    const std::vector<int>& windowSizes) {
    std::vector<double> logN, logRS;
    for (int n : windowSizes) {
        double rs = averageRSForWindowSize(increments, n);
        if (rs > 0.0) {
            logN.push_back(std::log(static_cast<double>(n)));
            logRS.push_back(std::log(rs));
        }
    }
    auto [intercept, slope] = linearRegression(logN, logRS);
    return slope;
}

// First differences of a path (e.g. fBm level series -> fractional Gaussian noise)
inline std::vector<double> computeIncrements(const std::vector<double>& path) {
    std::vector<double> increments;
    for (size_t i = 1; i < path.size(); i++) {
        increments.push_back(path[i] - path[i - 1]);
    }
    return increments;
}

// Log returns of a price series
inline std::vector<double> computeLogReturns(const std::vector<double>& prices) {
    std::vector<double> returns;
    for (size_t i = 1; i < prices.size(); i++) {
        returns.push_back(std::log(prices[i] / prices[i - 1]));
    }
    return returns;
}

// Reads the Close column from a FTSE-100-style CSV
inline std::vector<double> readClosingPrices(const std::string& filepath) {
    std::vector<double> closes;
    std::ifstream file(filepath);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;
        int col = 0;
        while (std::getline(ss, field, ',')) {
            if (col == 4) closes.push_back(std::stod(field));
            col++;
        }
    }
    return closes;
}
