// Hurst exponent estimation: validates the R/S estimator on synthetic fBm
// with known H, then applies it to real FTSE 100 log returns.
// Date: 29/09/2026

#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include "hurst.h"
#include "../fractional_brownian_motion/fbm.h"

std::vector<int> generateWindowSizes(int minN, int maxN, int count) {
    std::vector<int> sizes;
    double logMin = std::log(static_cast<double>(minN));
    double logMax = std::log(static_cast<double>(maxN));
    for (int i = 0; i < count; i++) {
        double logN = logMin + (logMax - logMin) * i / (count - 1);
        int n = static_cast<int>(std::round(std::exp(logN)));
        if (sizes.empty() || sizes.back() != n) sizes.push_back(n);
    }
    return sizes;
}

int main(int argc, char* argv[]) {
    unsigned int seed = 42;
    if (argc >= 2) seed = static_cast<unsigned int>(std::atoi(argv[1]));

    std::cout << "Part A: Validation on synthetic fBm" << std::endl;
    std::cout << "-------------------------------------" << std::endl;

    std::vector<double> trueHValues = { 0.3, 0.5, 0.7, 0.9 };
    int pathLength = 1000; // kept modest given fBm generator's O(n^3) Cholesky cost
    std::vector<int> windowSizes = generateWindowSizes(10, pathLength / 4, 15);

    std::ofstream validationFile("hurst_validation.csv");
    validationFile << "true_H,estimated_H,absolute_error\n";

    for (double trueH : trueHValues) {
        std::mt19937 gen(seed);
        auto path = simulateFBmPath(1.0, pathLength, trueH, gen);
        auto increments = computeIncrements(path);
        double estimatedH = estimateHurstExponent(increments, windowSizes);
        double error = std::abs(estimatedH - trueH);

        std::cout << "True H = " << trueH << ",  Estimated H = " << estimatedH
            << ",  Error = " << error << std::endl;

        validationFile << trueH << "," << estimatedH << "," << error << "\n";
    }
    validationFile.close();

    {
        std::mt19937 gen(seed);
        auto path = simulateFBmPath(1.0, pathLength, 0.7, gen);
        auto increments = computeIncrements(path);

        std::ofstream loglogFile("hurst_loglog_synthetic.csv");
        loglogFile << "log_n,log_RS\n";
        for (int n : windowSizes) {
            double rs = averageRSForWindowSize(increments, n);
            if (rs > 0.0) loglogFile << std::log(static_cast<double>(n)) << "," << std::log(rs) << "\n";
        }
        loglogFile.close();
    }

    std::cout << "\nPart B: Application to real FTSE 100 data" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    std::vector<double> closes = readClosingPrices("../data/ftse100.csv");
    if (closes.empty()) {
        std::cout << "Failed to read FTSE 100 data. Expected ../data/ftse100.csv" << std::endl;
        return 1;
    }

    std::vector<double> logReturns = computeLogReturns(closes);
    std::vector<int> ftseWindowSizes = generateWindowSizes(10, static_cast<int>(logReturns.size()) / 4, 15);
    double ftseH = estimateHurstExponent(logReturns, ftseWindowSizes);

    std::cout << "FTSE 100 estimated Hurst exponent (log returns): " << ftseH << std::endl;

    if (ftseH > 0.55) {
        std::cout << "Interpretation: H > 0.5 suggests persistent, long-memory behaviour." << std::endl;
    }
    else if (ftseH < 0.45) {
        std::cout << "Interpretation: H < 0.5 suggests anti-persistent, mean-reverting behaviour." << std::endl;
    }
    else {
        std::cout << "Interpretation: H is close to 0.5, consistent with a random walk (weak-form efficiency)." << std::endl;
    }

    std::ofstream loglogFtse("hurst_loglog_ftse.csv");
    loglogFtse << "log_n,log_RS\n";
    for (int n : ftseWindowSizes) {
        double rs = averageRSForWindowSize(logReturns, n);
        if (rs > 0.0) loglogFtse << std::log(static_cast<double>(n)) << "," << std::log(rs) << "\n";
    }
    loglogFtse.close();

    std::ofstream resultFile("hurst_ftse_result.csv");
    resultFile << "estimated_H\n" << ftseH << "\n";
    resultFile.close();

    return 0;
}
