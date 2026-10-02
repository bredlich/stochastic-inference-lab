#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "ar1.h"
#include <random>
#include <cmath>

TEST_CASE("OLS recovers exact phi for a noiseless deterministic series", "[ar1]") {
    // sigma = 0, so the path is purely deterministic: X_t = c + phi*X_{t-1}.
    // Starting at the stationary mean (the default) would make every
    // value identical, since the process is already at its fixed point,
    // giving a constant series with zero variance that OLS cannot fit.
    // Starting away from equilibrium (x0 = 5.0) gives the deterministic
    // path real variation to regress on, so OLS should recover phi exactly.
    std::mt19937 gen(1);
    double truePhi = 0.6;
    double trueC = 0.5;
    auto path = simulateAR1Path(truePhi, trueC, 0.0, 100, gen, 5.0);
    auto [cHat, phiHat] = estimateAR1(path);

    REQUIRE(phiHat == Catch::Approx(truePhi).margin(1e-8));
    REQUIRE(cHat == Catch::Approx(trueC).margin(1e-8));
}

TEST_CASE("Simulated path has correct length", "[ar1]") {
    std::mt19937 gen(42);
    auto path = simulateAR1Path(0.5, 0.0, 1.0, 300, gen);
    REQUIRE(path.size() == 301);
}

TEST_CASE("Simulation is reproducible under a fixed seed", "[ar1]") {
    std::mt19937 gen1(7);
    std::mt19937 gen2(7);
    auto path1 = simulateAR1Path(0.4, 0.1, 1.0, 200, gen1);
    auto path2 = simulateAR1Path(0.4, 0.1, 1.0, 200, gen2);

    REQUIRE(path1.size() == path2.size());
    for (size_t i = 0; i < path1.size(); ++i) {
        REQUIRE(path1[i] == Catch::Approx(path2[i]));
    }
}

TEST_CASE("Estimator recovers phi within tolerance on noisy data", "[ar1]") {
    std::mt19937 gen(123);
    double truePhi = 0.7;
    auto path = simulateAR1Path(truePhi, 0.0, 1.0, 5000, gen);
    auto [cHat, phiHat] = estimateAR1(path);

    REQUIRE(phiHat == Catch::Approx(truePhi).margin(0.05));
}

TEST_CASE("Sample mean approaches theoretical mean for a long stationary path", "[ar1]") {
    std::mt19937 gen(99);
    double phi = 0.5;
    double c = 1.0;
    auto path = simulateAR1Path(phi, c, 1.0, 20000, gen);

    double sampleMean = 0.0;
    for (double x : path) sampleMean += x;
    sampleMean /= path.size();

    double expectedMean = theoreticalMean(phi, c);
    REQUIRE(sampleMean == Catch::Approx(expectedMean).margin(0.1));
}
