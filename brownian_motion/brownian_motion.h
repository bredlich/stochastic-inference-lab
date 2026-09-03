// Header for the Brownian motion path simulation function.
// Shared between brownian_motion.cpp (the program) and
// test_brownian_motion.cpp (the unit tests), so both use the
// exact same implementation.
// Date: 03/09/2026

#pragma once

#include <vector>
#include <random>
#include <cmath>

// Simulates one Brownian motion path over [0, T] using n steps
inline std::vector<double> simulateBrownianPath(double T, int n, std::mt19937& gen) {
    double dt = T / n;
    double stdDev = std::sqrt(dt);

    std::normal_distribution<double> gaussian(0.0, stdDev);

    std::vector<double> path;
    path.push_back(0.0);

    double current = 0.0;
    for (int i = 0; i < n; i++) {
        current += gaussian(gen);
        path.push_back(current);
    }

    return path;
}
