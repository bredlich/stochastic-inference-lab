# Loads AR(1) results into PostgreSQL.
#
# Reads the connection string from the PG_CONN_STRING environment
# variable. 
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
    # ar1_validation.csv columns: true_phi, estimated_phi, absolute_error
    with open(path, newline="") as f:
        rows = list(csv.DictReader(f))

    for row in rows:
        cur.execute(
            """
            INSERT INTO ar1_validation_results (true_phi, estimated_phi, absolute_error)
            VALUES (%s, %s, %s)
            """,
            (
                float(row["true_phi"]),
                float(row["estimated_phi"]),
                float(row["absolute_error"]),
            ),
        )

    print(f"Loaded {len(rows)} validation rows into Postgres.")


def load_ftse_result(cur, path):
    # ar1_ftse_result.csv columns: estimated_c, estimated_phi, rmse_ar1, rmse_naive
    with open(path, newline="") as f:
        rows = list(csv.DictReader(f))

    for row in rows:
        cur.execute(
            """
            INSERT INTO ar1_ftse_results (estimated_c, estimated_phi, rmse_ar1, rmse_naive)
            VALUES (%s, %s, %s, %s)
            """,
            (
                float(row["estimated_c"]),
                float(row["estimated_phi"]),
                float(row["rmse_ar1"]),
                float(row["rmse_naive"]),
            ),
        )

    print(f"Loaded {len(rows)} FTSE result row(s) into Postgres.")


def main():
    validation_csv = "ar1_validation.csv"
    ftse_csv = "ar1_ftse_result.csv"

    for path in (validation_csv, ftse_csv):
        if not os.path.exists(path):
            print(f"Error: {path} not found. Run the ar1 executable first.")
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
