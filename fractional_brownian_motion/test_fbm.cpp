// Unit tests for the fractional Brownian motion generator.
// Date: 15/09/2026

#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "fbm.h"
#include <cmath>

TEST_CASE("Path starts at zero", "[fbm]") {
    std::mt19937 gen(42);
    auto path = simulateFBmPath(1.0, 100, 0.7, gen);
    REQUIRE(path[0] == 0.0);
}

TEST_CASE("Path has correct length", "[fbm]") {
    std::mt19937 gen(42);
    auto path = simulateFBmPath(1.0, 100, 0.7, gen);
    REQUIRE(path.size() == 101);
}

TEST_CASE("Same seed produces identical paths", "[fbm]") {
    std::mt19937 gen1(42);
    std::mt19937 gen2(42);
    auto path1 = simulateFBmPath(1.0, 50, 0.7, gen1);
    auto path2 = simulateFBmPath(1.0, 50, 0.7, gen2);
    REQUIRE(path1 == path2);
}

TEST_CASE("Covariance matrix is symmetric", "[fbm]") {
    Eigen::MatrixXd Sigma = buildFBmCovariance(20, 0.05, 0.7);
    for (int i = 0; i < Sigma.rows(); i++) {
        for (int j = 0; j < Sigma.cols(); j++) {
            REQUIRE(std::abs(Sigma(i, j) - Sigma(j, i)) < 1e-10);
        }
    }
}

TEST_CASE("H=0.5 recovers standard Brownian motion variance", "[fbm]") {
    // At H=0.5, Var(B_t^H) should equal t exactly (Remark 1).
    // Check this directly from the covariance matrix diagonal, Sigma_ii = t_i.
    int n = 10;
    double dt = 0.1;
    Eigen::MatrixXd Sigma = buildFBmCovariance(n, dt, 0.5);

    for (int i = 0; i < n; i++) {
        double t_i = (i + 1) * dt;
        REQUIRE(std::abs(Sigma(i, i) - t_i) < 1e-10);
    }
}
