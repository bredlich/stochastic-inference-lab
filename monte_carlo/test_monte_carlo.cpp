// Unit tests for the Monte Carlo option pricing and Black-Scholes implementation.
// Date: 08/09/2026

#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "monte_carlo.h"
#include <cmath>

TEST_CASE("Volatility estimation on a known simple series", "[monte_carlo]") {
    // A simple, deliberately constructed price series
    std::vector<double> prices = { 100.0, 101.0, 100.0, 101.0, 100.0, 101.0 };
    double S0, sigma;
    estimateS0AndSigma(prices, S0, sigma);

    REQUIRE(S0 == 101.0); // last price in the series
    REQUIRE(sigma > 0.0); // genuine volatility, not zero or negative
}

TEST_CASE("Monte Carlo converges to Black-Scholes within tolerance", "[monte_carlo]") {
    std::mt19937 gen(42);
    double S0 = 100.0, K = 100.0, r = 0.04, sigma = 0.2, T = 1.0;

    double mcPrice = monteCarloCallPrice(S0, K, r, sigma, T, 500000, gen);
    double bsPrice = blackScholesCall(S0, K, r, sigma, T);

    REQUIRE(std::abs(mcPrice - bsPrice) < 0.5); // within 50 cents on a ~10-unit price
}

TEST_CASE("Same seed produces identical Monte Carlo price", "[monte_carlo]") {
    std::mt19937 gen1(42);
    std::mt19937 gen2(42);
    double S0 = 100.0, K = 100.0, r = 0.04, sigma = 0.2, T = 1.0;

    double price1 = monteCarloCallPrice(S0, K, r, sigma, T, 10000, gen1);
    double price2 = monteCarloCallPrice(S0, K, r, sigma, T, 10000, gen2);

    REQUIRE(price1 == price2);
}

TEST_CASE("Black-Scholes deep in-the-money approaches intrinsic value", "[monte_carlo]") {
    // Very high spot relative to strike: option should be worth close to S0 - K*exp(-rT)
    double S0 = 1000.0, K = 100.0, r = 0.04, sigma = 0.2, T = 1.0;
    double price = blackScholesCall(S0, K, r, sigma, T);
    double intrinsic = S0 - K * std::exp(-r * T);

    REQUIRE(std::abs(price - intrinsic) < 1.0);
}

TEST_CASE("Black-Scholes deep out-of-the-money approaches zero", "[monte_carlo]") {
    double S0 = 10.0, K = 1000.0, r = 0.04, sigma = 0.2, T = 1.0;
    double price = blackScholesCall(S0, K, r, sigma, T);

    REQUIRE(price < 0.01);
}
