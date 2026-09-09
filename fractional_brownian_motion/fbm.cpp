// Simulates fractional Brownian motion paths for a given Hurst
// parameter, and exports them to CSV for plotting.
// Date: 09/09/2026

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <random>
#include "fbm.h"

int main(int argc, char* argv[]) {
    double T = 1.0;
    int n = 300; // kept modest given O(n^3) cost of Cholesky
    double H = 0.7;
    int numPaths = 3;
    unsigned int seed = std::random_device{}();

    if (argc >= 2) H = std::atof(argv[1]);
    if (argc >= 3) n = std::atoi(argv[2]);
    if (argc >= 4) numPaths = std::atoi(argv[3]);
    if (argc >= 5) seed = static_cast<unsigned int>(std::atoi(argv[4]));

    std::mt19937 gen(seed);

    std::ofstream outFile("fbm_paths.csv");
    if (!outFile.is_open()) {
        std::cout << "Failed to open output file." << std::endl;
        return 1;
    }

    outFile << "t";
    for (int p = 0; p < numPaths; p++) outFile << ",path_" << p;
    outFile << "\n";

    std::vector<std::vector<double>> allPaths;
    for (int p = 0; p < numPaths; p++) {
        allPaths.push_back(simulateFBmPath(T, n, H, gen));
    }

    double dt = T / n;
    for (int i = 0; i <= n; i++) {
        outFile << (i * dt);
        for (int p = 0; p < numPaths; p++) outFile << "," << allPaths[p][i];
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Wrote fbm_paths.csv with " << numPaths << " paths, H=" << H
        << ", " << n << " steps each." << std::endl;

    return 0;
}
