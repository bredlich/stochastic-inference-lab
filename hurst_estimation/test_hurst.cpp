// Unit tests for the Hurst exponent estimator.
// Date: 29/09/2026

#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "hurst.h"
#include "../fractional_brownian_motion/fbm.h"
#include <random>

TEST_CASE("Linear regression recovers exact known slope and intercept", "[hurst]") {
    std::vector<double> x = { 1, 2, 3, 4, 5 };
    std::vector<double> y = { 5, 8, 11, 14, 17 }; // y = 2 + 3x exactly
    auto [a, b] = linearRegression(x, y);
    REQUIRE(std::abs(a - 2.0) < 1e-9);
    REQUIRE(std::abs(b - 3.0) < 1e-9);
}

TEST_CASE("Increments and log returns have correct length", "[hurst]") {
    std::vector<double> series = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    REQUIRE(computeIncrements(series).size() == 4);
    REQUIRE(computeLogReturns(series).size() == 4);
}

TEST_CASE("R/S is zero for a constant series", "[hurst]") {
    std::vector<double> constant(50, 5.0);
    REQUIRE(computeRS(constant) == 0.0);
}

TEST_CASE("Hurst estimator recovers H close to 0.5 for independent Gaussian noise", "[hurst]") {
    std::mt19937 gen(42);
    std::normal_distribution<double> gaussian(0.0, 1.0);
    std::vector<double> noise;
    for (int i = 0; i < 2000; i++) noise.push_back(gaussian(gen));

    std::vector<int> windowSizes = { 10, 20, 40, 80, 160, 320 };
    double estimatedH = estimateHurstExponent(noise, windowSizes);

    REQUIRE(std::abs(estimatedH - 0.5) < 0.15);
}

TEST_CASE("Hurst estimator detects persistence in synthetic fBm increments", "[hurst]") {
    std::mt19937 gen(42);
    auto path = simulateFBmPath(1.0, 2000, 0.7, gen);
    auto increments = computeIncrements(path);

    std::vector<int> windowSizes = { 10, 20, 40, 80, 160, 320 };
    double estimatedH = estimateHurstExponent(increments, windowSizes);

    REQUIRE(estimatedH > 0.5);
    REQUIRE(std::abs(estimatedH - 0.7) < 0.2); // R/S is a genuinely noisy estimator
}
