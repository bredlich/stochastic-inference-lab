#!/bin/bash
# Runs Hurst exponent validation across several random seeds, to check
# the estimator's stability rather than relying on a single run.
# Date: 29/09/2026

echo "Running Hurst estimator validation across multiple seeds..."

for seed in 1 2 3 42 100; do
    echo "Seed: $seed"
    ./build/Debug/hurst.exe "$seed"
    mv hurst_validation.csv "hurst_validation_seed${seed}.csv"
done

echo "Done."
