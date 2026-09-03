#!/bin/bash
# Runs the Brownian motion generator multiple times with different path counts,
# saving each result to its own CSV for comparison.
# Date: 03/09/2026

echo "Running Brownian motion experiments..."

for num_paths in 5 20 100; do
    echo "Simulating with $num_paths paths..."
    ./build/Debug/brownian_motion.exe "$num_paths" 500
    mv brownian_paths.csv "brownian_paths_${num_paths}.csv"
done

echo "Done. Results saved as brownian_paths_5.csv, brownian_paths_20.csv, brownian_paths_100.csv"
