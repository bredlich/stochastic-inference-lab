#!/bin/bash
# Runs the option pricer across several strike multipliers to compare
# Monte Carlo vs Black-Scholes pricing at different moneyness levels.
# Date: 07/09/2026

echo "Running option pricing experiments across strikes..."

for k_mult in 0.9 0.95 1.0 1.05 1.1; do
    echo "Strike multiplier: $k_mult"
    ./build/Debug/monte_carlo.exe "$k_mult" 0.04 1.0 100000 42
    echo "---"
done
