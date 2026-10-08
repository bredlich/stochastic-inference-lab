// GARCH(1,1) volatility model: validation against synthetic data with known
// parameters, then application to real FTSE 100 log returns.
// Date: 08/10/2026

#include "garch.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <cmath>

int main(int argc, char* argv[]) {
    int seed = (argc > 1) ? std::stoi(argv[1]) : 42;
    std::mt19937 gen(seed);

    // ---- Part A: Validation against synthetic GARCH data ----
    struct Params { double alpha; double beta; };
    std::vector<Params> trueParams = {
        {0.05, 0.90}, {0.10, 0.85}, {0.15, 0.80}, {0.05, 0.93}
    };
    double longRunVariance = 1e-4; // a daily volatility of 1 percent
    int n = 5000;

    std::ofstream validationFile("garch_validation.csv");
    validationFile << "true_alpha,estimated_alpha,true_beta,estimated_beta,"
        "true_persistence,estimated_persistence,absolute_error_persistence\n";

    std::cout << "Part A: Validation against synthetic GARCH data\n";
    std::cout << "------------------------------------------------\n";

    for (const Params& p : trueParams) {
        double omega = longRunVariance * (1.0 - p.alpha - p.beta);
        GarchPath path = simulateGARCH(omega, p.alpha, p.beta, n, gen);
        GarchFit fit = fitGARCH(path.returns);

        double truePersistence = p.alpha + p.beta;
        double estimatedPersistence = fit.alpha + fit.beta;
        double persistenceError = std::abs(truePersistence - estimatedPersistence);

        std::cout << "True (alpha, beta) = (" << p.alpha << ", " << p.beta << ")"
            << ", estimated = (" << fit.alpha << ", " << fit.beta << ")"
            << ", persistence error = " << persistenceError << "\n";

        validationFile << p.alpha << "," << fit.alpha << ","
            << p.beta << "," << fit.beta << ","
            << truePersistence << "," << estimatedPersistence << ","
            << persistenceError << "\n";
    }
    validationFile.close();

    // ---- Part B: Application to real FTSE 100 log returns ----
    std::cout << "\nPart B: Fitting GARCH(1,1) to real FTSE 100 log returns\n";
    std::cout << "---------------------------------------------------------\n";

    std::vector<double> prices = readClosingPrices("../data/ftse100.csv");
    std::vector<double> returns = computeLogReturns(prices);

    // Volatility clustering shows up in the squared returns, not the returns.
    std::vector<double> squaredReturns = squared(returns);
    std::ofstream acfFile("garch_acf_ftse.csv");
    acfFile << "lag,acf_returns,acf_squared_returns\n";
    for (int lag = 1; lag <= 20; ++lag) {
        acfFile << lag << "," << autocorrelation(returns, lag) << ","
            << autocorrelation(squaredReturns, lag) << "\n";
    }
    acfFile.close();
    std::cout << "Lag 1 autocorrelation of returns: " << autocorrelation(returns, 1) << "\n";
    std::cout << "Lag 1 autocorrelation of squared returns: "
        << autocorrelation(squaredReturns, 1) << "\n";

    GarchFit fit = fitGARCH(returns);
    double persistence = fit.alpha + fit.beta;
    double halfLife = std::log(0.5) / std::log(persistence);
    double annualise = std::sqrt(252.0);

    std::vector<double> eps(returns.size());
    for (size_t t = 0; t < returns.size(); ++t) eps[t] = returns[t] - fit.mean;
    double v = sampleVariance(eps);
    std::vector<double> h = conditionalVariances(eps, fit.omega, fit.alpha, fit.beta, v);
    double hNext = nextVariance(eps, h, fit.omega, fit.alpha, fit.beta);

    double longRunVol = std::sqrt(unconditionalVariance(fit.omega, fit.alpha, fit.beta)) * annualise;
    double currentVol = std::sqrt(hNext) * annualise;
    double oneYearVol = std::sqrt(averageForecastVariance(fit.omega, fit.alpha, fit.beta,
        hNext, 252)) * annualise;

    std::cout << "Estimated alpha = " << fit.alpha << "\n";
    std::cout << "Estimated beta = " << fit.beta << "\n";
    std::cout << "Persistence (alpha + beta) = " << persistence << "\n";
    std::cout << "Half life of a volatility shock (days) = " << halfLife << "\n";
    std::cout << "Long run annualised volatility = " << longRunVol << "\n";
    std::cout << "Current annualised volatility = " << currentVol << "\n";
    std::cout << "Average forecast volatility over the next year = " << oneYearVol << "\n";

    std::ofstream ftseFile("garch_ftse_result.csv");
    ftseFile << "estimated_omega,estimated_alpha,estimated_beta,persistence,half_life_days,"
        "long_run_vol_annual,current_vol_annual,one_year_forecast_vol_annual,"
        "log_likelihood\n";
    ftseFile << fit.omega << "," << fit.alpha << "," << fit.beta << "," << persistence << ","
        << halfLife << "," << longRunVol << "," << currentVol << "," << oneYearVol << ","
        << fit.logLikelihood << "\n";
    ftseFile.close();

    std::ofstream sigmaFile("garch_ftse_sigma.csv");
    sigmaFile << "t,return,conditional_vol_annual\n";
    for (size_t t = 0; t < returns.size(); ++t) {
        sigmaFile << t << "," << returns[t] << "," << std::sqrt(h[t]) * annualise << "\n";
    }
    sigmaFile.close();

    // ---- Out of sample check: fit on the first 70 percent, score on the rest ----
    size_t trainN = returns.size() * 7 / 10;
    std::vector<double> train(returns.begin(), returns.begin() + trainN);
    GarchFit trainFit = fitGARCH(train);

    std::vector<double> epsAll(returns.size());
    for (size_t t = 0; t < returns.size(); ++t) epsAll[t] = returns[t] - trainFit.mean;
    std::vector<double> epsTrain(epsAll.begin(), epsAll.begin() + trainN);
    double trainVariance = sampleVariance(epsTrain);

    std::vector<double> hAll = conditionalVariances(epsAll, trainFit.omega, trainFit.alpha,
        trainFit.beta, trainVariance);
    std::vector<double> epsTest(epsAll.begin() + trainN, epsAll.end());
    std::vector<double> hGarch(hAll.begin() + trainN, hAll.end());
    std::vector<double> hConstant(epsTest.size(), trainVariance);

    double qlikeGarch = qlikeLoss(epsTest, hGarch);
    double qlikeConstant = qlikeLoss(epsTest, hConstant);
    double mseGarch = squaredReturnMSE(epsTest, hGarch);
    double mseConstant = squaredReturnMSE(epsTest, hConstant);

    std::cout << "\nOut of sample check (fit on " << trainN << " days, test on "
        << epsTest.size() << " days)\n";
    std::cout << "QLIKE, GARCH: " << qlikeGarch << ", constant variance: " << qlikeConstant << "\n";
    std::cout << "MSE of squared returns, GARCH: " << mseGarch
        << ", constant variance: " << mseConstant << "\n";

    std::ofstream oosFile("garch_oos_result.csv");
    oosFile << "train_n,test_n,qlike_garch,qlike_constant,mse_garch,mse_constant\n";
    oosFile << trainN << "," << epsTest.size() << "," << qlikeGarch << "," << qlikeConstant
        << "," << mseGarch << "," << mseConstant << "\n";
    oosFile.close();

    return 0;
}
