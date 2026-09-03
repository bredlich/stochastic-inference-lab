// Standard brownian motion path generator
// Simulates one or more Wiener process paths using Gaussian increments
// Date: 02/09/2026

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

// Simulates one Brownian motion path over [0, T] with n steps
std::vector<double> simulateBrownianPath(double T, int n, std::mt19937& gen) {
    double dt = T / n; // size of each time step
    double stdDev = std::sqrt(dt); // increment std dev scales with sqrt(dt)

    std::normal_distribution<double> gaussian(0.0, stdDev);

    std::vector<double> path;
    path.push_back(0.0); // starting point at 0

    double current = 0.0;
    for (int i = 0; i < n; i++) {
        current += gaussian(gen); // add a random increment
        path.push_back(current);
    }

    return path;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    double T = 1.0;      // simulate over the interval [0, 1]
    int n = 500;          // number of time steps
    int numPaths = 5;     // simulate several paths to see the variability

    std::ofstream outFile("brownian_paths.csv");
    if (!outFile.is_open()) {
        std::cout << "Failed to open output file." << std::endl;
        return 1;
    }

    // Header: t, path_0, path_1, ..., path_4
    outFile << "t";
    for (int p = 0; p < numPaths; p++) {
        outFile << ",path_" << p;
    }
    outFile << "\n";

    std::vector<std::vector<double>> allPaths;
    for (int p = 0; p < numPaths; p++) {
        allPaths.push_back(simulateBrownianPath(T, n, gen));
    }

    double dt = T / n;
    for (int i = 0; i <= n; i++) {
        outFile << (i * dt);
        for (int p = 0; p < numPaths; p++) {
            outFile << "," << allPaths[p][i];
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Wrote brownian_paths.csv with " << numPaths << " paths, " << n << " steps each." << std::endl;

    return 0;
}
