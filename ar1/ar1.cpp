// AR(1) process simulator: validation against synthetic data with known
// ground truth, then application to real FTSE 100 log returns.
// Date: 02/10/2026

#include "ar1.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    int seed = (argc > 1) ? std::stoi(argv[1]) : 42;
    std::mt19937 gen(seed);

    // ---- Part A: Validation against synthetic AR(1) data ----
    std::vector<double> trueParams = { -0.5, 0.0, 0.3, 0.6, 0.9 };
    double c = 0.0;
    double sigma = 1.0;
    int n = 2000;

    std::ofstream validationFile("ar1_validation.csv");
    validationFile << "true_phi,estimated_phi,absolute_error\n";

    std::cout << "Part A: Validation against synthetic AR(1) data\n";
    std::cout << "------------------------------------------------\n";

    for (double truePhi : trueParams) {
        std::vector<double> path = simulateAR1Path(truePhi, c, sigma, n, gen);
        auto [cHat, phiHat] = estimateAR1(path);
        double error = std::abs(truePhi - phiHat);

        std::cout << "True phi = " << truePhi
            << ", Estimated phi = " << phiHat
            << ", Absolute error = " << error << "\n";

        validationFile << truePhi << "," << phiHat << "," << error << "\n";
    }
    validationFile.close();

    // ---- Part B: Application to real FTSE 100 log returns ----
    std::cout << "\nPart B: Fitting AR(1) to real FTSE 100 log returns\n";
    std::cout << "----------------------------------------------------\n";

    std::vector<double> prices = readClosingPrices("../data/ftse100.csv");
    std::vector<double> returns = computeLogReturns(prices);

    auto [cHat, phiHat] = estimateAR1(returns);

    std::cout << "Estimated c = " << cHat << "\n";
    std::cout << "Estimated phi = " << phiHat << "\n";

    // One-step-ahead forecast evaluation: for each t, forecast return_{t+1}
    // from return_t using the fitted AR(1), and compare against both the
    // actual return and a naive "no-change" (zero return) baseline.
    std::vector<double> actualNextStep;
    std::vector<double> ar1Forecast;
    std::vector<double> naiveForecast;

    for (size_t t = 0; t + 1 < returns.size(); ++t) {
        actualNextStep.push_back(returns[t + 1]);
        ar1Forecast.push_back(forecastNextStep(cHat, phiHat, returns[t]));
        naiveForecast.push_back(0.0);
    }

    double rmseAR1 = computeRMSE(actualNextStep, ar1Forecast);
    double rmseNaive = computeRMSE(actualNextStep, naiveForecast);

    std::cout << "AR(1) one-step-ahead forecast RMSE: " << rmseAR1 << "\n";
    std::cout << "Naive (zero-return) forecast RMSE: " << rmseNaive << "\n";

    std::string interpretation;
    if (std::abs(phiHat) < 0.05) {
        interpretation = "negligible autocorrelation, close to white noise";
    }
    else if (phiHat > 0) {
        interpretation = "weak positive autocorrelation";
    }
    else {
        interpretation = "weak negative autocorrelation";
    }
    std::cout << "Interpretation: " << interpretation << "\n";

    std::ofstream ftseFile("ar1_ftse_result.csv");
    ftseFile << "estimated_c,estimated_phi,rmse_ar1,rmse_naive\n";
    ftseFile << cHat << "," << phiHat << "," << rmseAR1 << "," << rmseNaive << "\n";
    ftseFile.close();

    return 0;
}
