#!/bin/bash
# Runs the fBm generator across a range of Hurst values, saving each
# result to its own CSV for comparison.
# Date: 08/09/2026

echo "Running fBm experiments across Hurst values..."

for H in 0.1 0.3 0.5 0.7 0.9; do
    echo "Hurst parameter: $H"
    ./build/Debug/fbm.exe "$H" 300 3 42
    mv fbm_paths.csv "fbm_paths_H${H}.csv"
done

echo "Done."
