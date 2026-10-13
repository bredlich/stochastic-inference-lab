#!/bin/bash
# Run the tests and main program of every project.

set -e

RESULTS="${RESULTS_DIR:-/results}"
export MPLBACKEND=Agg

for dir in */; do
    name="${dir%/}"
    [ -f "${dir}CMakeLists.txt" ] || continue
    [ -d "${dir}build" ] || continue

    echo "=== ${name} ==="
    (
        cd "$dir"

        for exe in build/*_tests; do
            [ -f "$exe" ] || continue
            echo "Testing ${exe}"
            "$exe"
        done

        for exe in build/*; do
            [ -f "$exe" ] && [ -x "$exe" ] || continue
            case "$exe" in *_tests) continue ;; esac
            echo "Running ${exe}"
            "$exe"
        done

        for script in plot_*.py; do
            [ -f "$script" ] || continue
            echo "Plotting with ${script}"
            python3 "$script"
        done

        mkdir -p "${RESULTS}/${name}"
        find . -maxdepth 1 \( -name '*.csv' -o -name '*.png' \) -exec cp {} "${RESULTS}/${name}/" \;
    )
done

echo "Done. Results are in ${RESULTS}."
