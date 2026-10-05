#!/bin/bash
# Sweep the AR(1) validation across several seeds to check that parameter
# recovery is stable and not an artifact of one particular random draw.

set -e

# Visual Studio (Windows) puts the executable in build/Debug; Linux/CI in build/.
if [ -f "build/Debug/ar1.exe" ]; then
    EXE="build/Debug/ar1.exe"
elif [ -f "build/ar1" ]; then
    EXE="build/ar1"
else
    echo "Could not find the ar1 executable. Build the project first."
    exit 1
fi

SEEDS=(1 2 3 42 100)

for seed in "${SEEDS[@]}"; do
    echo "Running AR(1) validation with seed=$seed"
    "$EXE" "$seed"
    cp ar1_validation.csv "ar1_validation_seed${seed}.csv"
done

echo "Done. Per-seed validation CSVs saved as ar1_validation_seed<seed>.csv"
