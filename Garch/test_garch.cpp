#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "garch.h"
#include <random>
#include <cmath>

TEST_CASE("Simulated path has correct length and positive variances", "[garch]") {
    std::mt19937 gen(42);
    auto path = simulateGARCH(1e-6, 0.10, 0.85, 500, gen);

    REQUIRE(path.returns.size() == 500);
    REQUIRE(path.variances.size() == 500);
    for (double h : path.variances) {
        REQUIRE(h > 0.0);
    }
}

TEST_CASE("Simulation is reproducible under a fixed seed", "[garch]") {
    std::mt19937 gen1(7);
    std::mt19937 gen2(7);
    auto path1 = simulateGARCH(1e-6, 0.10, 0.85, 300, gen1);
    auto path2 = simulateGARCH(1e-6, 0.10, 0.85, 300, gen2);

    for (size_t i = 0; i < path1.returns.size(); ++i) {
        REQUIRE(path1.returns[i] == Catch::Approx(path2.returns[i]));
    }
}

TEST_CASE("Sample variance approaches the unconditional variance", "[garch]") {
    std::mt19937 gen(99);
    double omega = 1e-6;
    double alpha = 0.05;
    double beta = 0.90;
    auto path = simulateGARCH(omega, alpha, beta, 200000, gen);

    double expected = unconditionalVariance(omega, alpha, beta);
    REQUIRE(sampleVariance(path.returns) == Catch::Approx(expected).epsilon(0.1));
}

TEST_CASE("Squared returns are autocorrelated but returns are not", "[garch]") {
    // This is volatility clustering: the direction of a return is unpredictable,
    // but the size of one return carries information about the next.
    std::mt19937 gen(5);
    auto path = simulateGARCH(1e-6, 0.10, 0.85, 100000, gen);

    double acfReturns = autocorrelation(path.returns, 1);
    double acfSquared = autocorrelation(squared(path.returns), 1);

    REQUIRE(std::abs(acfReturns) < 0.02);
    REQUIRE(acfSquared > 0.10);
}

TEST_CASE("Fit recovers persistence and beats a constant variance model", "[garch]") {
    std::mt19937 gen(123);
    double omega = 1e-6;
    double alpha = 0.10;
    double beta = 0.85;
    auto path = simulateGARCH(omega, alpha, beta, 10000, gen);

    GarchFit fit = fitGARCH(path.returns);
    REQUIRE(std::abs((fit.alpha + fit.beta) - (alpha + beta)) < 0.05);

    // Compare against a model with constant variance on the same data.
    double v = sampleVariance(path.returns);
    std::vector<double> constantH(path.returns.size(), v);
    double llConstant = gaussianLogLikelihood(path.returns, constantH);
    REQUIRE(fit.logLikelihood > llConstant);
}
