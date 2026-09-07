// Header for GBM simulation, Monte Carlo option pricing, and the
// Black-Scholes closed form, plus utilities for estimating volatility
// from real historical price data.
// Date: 07/09/2026

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

// Reads the Close column from a FTSE-100 CSV
// (Date,Open,High,Low,Close,Volume,Dividends,Stock Splits)
inline std::vector<double> readClosingPrices(const std::string& filepath) {
    std::vector<double> closes;
    std::ifstream file(filepath);
    std::string line;

    std::getline(file, line); // skip header row

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;
        int col = 0;
        while (std::getline(ss, field, ',')) {
            if (col == 4) { // Close column
                closes.push_back(std::stod(field));
            }
            col++;
        }
    }
    return closes;
}

// Computes S0 (most recent close) and annualised volatility from log returns
inline void estimateS0AndSigma(const std::vector<double>& closes, double& S0, double& sigma) {
    std::vector<double> logReturns;
    for (size_t i = 1; i < closes.size(); i++) {
        logReturns.push_back(std::log(closes[i] / closes[i - 1]));
    }

    double mean = 0.0;
    for (double r : logReturns) mean += r;
    mean /= logReturns.size();

    double variance = 0.0;
    for (double r : logReturns) variance += (r - mean) * (r - mean);
    variance /= (logReturns.size() - 1); // sample variance

    double dailyStdDev = std::sqrt(variance);

    S0 = closes.back();
    sigma = dailyStdDev * std::sqrt(252.0); // annualised
}

// Simulates one terminal GBM price under the risk-neutral measure
inline double simulateGBMTerminal(double S0, double r, double sigma, double T, std::mt19937& gen) {
    std::normal_distribution<double> gaussian(0.0, std::sqrt(T));
    double W_T = gaussian(gen);
    return S0 * std::exp((r - 0.5 * sigma * sigma) * T + sigma * W_T);
}

// Monte Carlo price of a European call option
inline double monteCarloCallPrice(double S0, double K, double r, double sigma, double T,
    int numSimulations, std::mt19937& gen) {
    double payoffSum = 0.0;
    for (int i = 0; i < numSimulations; i++) {
        double S_T = simulateGBMTerminal(S0, r, sigma, T, gen);
        payoffSum += std::max(S_T - K, 0.0);
    }
    double meanPayoff = payoffSum / numSimulations;
    return std::exp(-r * T) * meanPayoff;
}

// Standard normal CDF, via the error function
inline double normalCDF(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

// Closed-form Black-Scholes price for a European call
inline double blackScholesCall(double S0, double K, double r, double sigma, double T) {
    double d1 = (std::log(S0 / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    return S0 * normalCDF(d1) - K * std::exp(-r * T) * normalCDF(d2);
}
