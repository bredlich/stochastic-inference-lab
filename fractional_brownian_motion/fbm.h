// Fractional Brownian motion generator via Cholesky decomposition.
// Constructs the fBm covariance matrix for a given Hurst parameter H,
// decomposes it, and applies it to independent Gaussian draws to
// produce a correctly correlated sample path.
// Date: 09/09/2026

#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <Eigen/Dense>

// Builds the n x n fBm covariance matrix over discretised time points
// t_i = i * dt, for i = 1, ..., n, using the covariance formula:
// Sigma_ij = 0.5 * (|t_i|^2H + |t_j|^2H - |t_i - t_j|^2H)
inline Eigen::MatrixXd buildFBmCovariance(int n, double dt, double H) {
    Eigen::MatrixXd Sigma(n, n);
    for (int i = 0; i < n; i++) {
        double t_i = (i + 1) * dt;
        for (int j = 0; j < n; j++) {
            double t_j = (j + 1) * dt;
            Sigma(i, j) = 0.5 * (std::pow(t_i, 2 * H) + std::pow(t_j, 2 * H)
                - std::pow(std::abs(t_i - t_j), 2 * H));
        }
    }
    return Sigma;
}

// Simulates one fBm sample path of n steps over [0, T] with Hurst parameter H,
// via Cholesky decomposition of the covariance matrix.
inline std::vector<double> simulateFBmPath(double T, int n, double H, std::mt19937& gen) {
    double dt = T / n;

    Eigen::MatrixXd Sigma = buildFBmCovariance(n, dt, H);

    // Cholesky decomposition: Sigma = L * L^T
    Eigen::LLT<Eigen::MatrixXd> chol(Sigma);
    Eigen::MatrixXd L = chol.matrixL();

    // n independent standard normal draws
    std::normal_distribution<double> gaussian(0.0, 1.0);
    Eigen::VectorXd Z(n);
    for (int i = 0; i < n; i++) {
        Z(i) = gaussian(gen);
    }

    // Apply the correlation structure: B^H = L * Z
    Eigen::VectorXd B = L * Z;

    // Prepend the starting point B_0^H = 0
    std::vector<double> path;
    path.push_back(0.0);
    for (int i = 0; i < n; i++) {
        path.push_back(B(i));
    }

    return path;
}
