# Loads Hurst estimation results into PostgreSQL.
#
# Replaces store_hurst_results.cpp for CI/Linux purposes: Ubuntu's
# libpqxx-dev package has an ABI mismatch that prevents linking on CI,
# so this Python script (using the already-proven psycopg2 pattern from
# query_results.py) is the cross-platform path for loading results.
# store_hurst_results.cpp remains as a Windows-local demonstration of
# the C++/libpqxx integration but is no longer built on Linux CI.
#
# Reads the connection string from the PG_CONN_STRING environment
# variable. No credentials are ever hardcoded in this file.

import csv
import os
import sys

import psycopg2


def get_connection_string():
    conn_string = os.environ.get("PG_CONN_STRING")
    if not conn_string:
        raise RuntimeError("PG_CONN_STRING environment variable not set")
    return conn_string


def load_validation_results(cur, path):
    # hurst_validation.csv has columns: true_H, estimated_H, absolute_error
    # (no seed column, so seed is left NULL in the table).
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    for row in rows:
        true_h = float(row["true_H"])
        estimated_h = float(row["estimated_H"])
        absolute_error = float(row["absolute_error"])
        cur.execute(
            """
            INSERT INTO hurst_validation_results (true_h, estimated_h, absolute_error)
            VALUES (%s, %s, %s)
            """,
            (true_h, estimated_h, absolute_error),
        )

    print(f"Loaded {len(rows)} validation rows into Postgres.")


def interpret(estimated_h):
    if estimated_h > 0.55:
        return "persistent, long-range dependent"
    elif estimated_h < 0.45:
        return "anti-persistent"
    else:
        return "close to a random walk"


def load_ftse_result(cur, path):
    # hurst_ftse_result.csv has a single column: estimated_H.
    # Interpretation is derived here using the same thresholds as the
    # C++ estimator (0.55 / 0.45), since the CSV does not carry the text.
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    for row in rows:
        estimated_h = float(row["estimated_H"])
        interpretation = interpret(estimated_h)
        cur.execute(
            """
            INSERT INTO hurst_ftse_results (estimated_h, interpretation)
            VALUES (%s, %s)
            """,
            (estimated_h, interpretation),
        )

    print(f"Loaded {len(rows)} FTSE result row(s) into Postgres.")


def main():
    validation_csv = "hurst_validation.csv"
    ftse_csv = "hurst_ftse_result.csv"

    if not os.path.exists(validation_csv):
        print(f"Error: {validation_csv} not found. Run the hurst executable first.")
        sys.exit(1)
    if not os.path.exists(ftse_csv):
        print(f"Error: {ftse_csv} not found. Run the hurst executable first.")
        sys.exit(1)

    conn = psycopg2.connect(get_connection_string())
    try:
        with conn:
            with conn.cursor() as cur:
                load_validation_results(cur, validation_csv)
                load_ftse_result(cur, ftse_csv)
    finally:
        conn.close()


if __name__ == "__main__":
    main()
