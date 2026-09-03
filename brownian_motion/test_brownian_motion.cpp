// Unit tests for the Brownian motion generator.
// Tests the simulateBrownianPath function via brownian_motion.h,
// Date: 03/09/2026

#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/catch_amalgamated.hpp"
#include "brownian_motion.h"

TEST_CASE("Path starts at zero", "[brownian_motion]") {
    std::mt19937 gen(42);
    auto path = simulateBrownianPath(1.0, 100, gen);
    REQUIRE(path[0] == 0.0);
}

TEST_CASE("Path has correct length", "[brownian_motion]") {
    std::mt19937 gen(42);
    auto path = simulateBrownianPath(1.0, 100, gen);
    REQUIRE(path.size() == 101);
}

TEST_CASE("Same seed produces identical paths", "[brownian_motion]") {
    std::mt19937 gen1(42);
    std::mt19937 gen2(42);
    auto path1 = simulateBrownianPath(1.0, 50, gen1);
    auto path2 = simulateBrownianPath(1.0, 50, gen2);
    REQUIRE(path1 == path2);
}
