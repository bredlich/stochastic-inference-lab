// Prices a European call option on the FTSE 100 via Monte Carlo
// simulation of GBM price paths, and compares the result against
// the closed-form Black-Scholes price.
// Date: 07/09/2026

#include <iostream>
#include <cstdlib>
#include <random>
#include "monte_carlo.h"

int main(int argc, char* argv[]) {
    // Defaults
    double K_multiplier = 1.0;   // strike as a multiple of S0 (1.0 = at-the-money)
    double r = 0.04;             // risk-free rate assumption (~UK short-term rate)
    double T = 1.0;              // maturity in years
    int numSimulations = 100000;
    unsigned int seed = std::random_device{}();
    bool seedProvided = false;

    if (argc >= 2) K_multiplier = std::atof(argv[1]);
    if (argc >= 3) r = std::atof(argv[2]);
    if (argc >= 4) T = std::atof(argv[3]);
    if (argc >= 5) numSimulations = std::atoi(argv[4]);
    if (argc >= 6) { seed = static_cast<unsigned int>(std::atoi(argv[5])); seedProvided = true; }

    std::vector<double> closes = readClosingPrices("../data/ftse100.csv");
    if (closes.empty()) {
        std::cout << "Failed to read FTSE 100 data. Expected ../data/ftse100.csv" << std::endl;
        return 1;
    }

    double S0, sigma;
    estimateS0AndSigma(closes, S0, sigma);
    double K = S0 * K_multiplier;

    std::mt19937 gen(seed);

    double mcPrice = monteCarloCallPrice(S0, K, r, sigma, T, numSimulations, gen);
    double bsPrice = blackScholesCall(S0, K, r, sigma, T);

    std::cout << "FTSE 100 European Call Option Pricing" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "S0 (spot):        " << S0 << std::endl;
    std::cout << "Sigma (annual):   " << sigma << std::endl;
    std::cout << "K (strike):       " << K << std::endl;
    std::cout << "r (risk-free):    " << r << std::endl;
    std::cout << "T (years):        " << T << std::endl;
    std::cout << "Simulations:      " << numSimulations << std::endl;
    std::cout << std::endl;
    std::cout << "Monte Carlo price:   " << mcPrice << std::endl;
    std::cout << "Black-Scholes price: " << bsPrice << std::endl;
    std::cout << "Absolute difference: " << std::abs(mcPrice - bsPrice) << std::endl;

    return 0;
}
