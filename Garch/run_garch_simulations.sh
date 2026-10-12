#!/bin/bash
# Sweep the GARCH validation across several seeds to check that parameter
# recovery is stable and not an artifact of one particular random draw.

set -e

# Visual Studio puts the executable in build/Debug; Linux/CI in build/.
if [ -f "build/Debug/garch.exe" ]; then
    EXE="build/Debug/garch.exe"
elif [ -f "build/garch" ]; then
    EXE="build/garch"
else
    echo "Could not find the garch executable. Build the project first."
    exit 1
fi

SEEDS=(1 2 3 42 100)

for seed in "${SEEDS[@]}"; do
    echo "Running GARCH validation with seed=$seed"
    "$EXE" "$seed"
    cp garch_validation.csv "garch_validation_seed${seed}.csv"
done

echo "Done. Per-seed validation CSVs saved as garch_validation_seed<seed>.csv"
