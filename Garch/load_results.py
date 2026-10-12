# Loads GARCH results into PostgreSQL.
# Reads the connection string from the PG_CONN_STRING environment variable.

import csv
import os
import sys

import psycopg2


def get_connection_string():
    conn_string = os.environ.get("PG_CONN_STRING")
    if not conn_string:
        raise RuntimeError("PG_CONN_STRING environment variable not set")
    return conn_string


def read_rows(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def load_validation_results(cur, path):
    rows = read_rows(path)
    for row in rows:
        cur.execute(
            """
            INSERT INTO garch_validation_results
                (true_alpha, estimated_alpha, true_beta, estimated_beta,
                 true_persistence, estimated_persistence, absolute_error_persistence)
            VALUES (%s, %s, %s, %s, %s, %s, %s)
            """,
            (
                float(row["true_alpha"]),
                float(row["estimated_alpha"]),
                float(row["true_beta"]),
                float(row["estimated_beta"]),
                float(row["true_persistence"]),
                float(row["estimated_persistence"]),
                float(row["absolute_error_persistence"]),
            ),
        )
    print(f"Loaded {len(rows)} validation rows into Postgres.")


def load_ftse_result(cur, path):
    rows = read_rows(path)
    for row in rows:
        cur.execute(
            """
            INSERT INTO garch_ftse_results
                (estimated_omega, estimated_alpha, estimated_beta, persistence,
                 half_life_days, long_run_vol_annual, current_vol_annual,
                 one_year_forecast_vol_annual, log_likelihood)
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
            """,
            (
                float(row["estimated_omega"]),
                float(row["estimated_alpha"]),
                float(row["estimated_beta"]),
                float(row["persistence"]),
                float(row["half_life_days"]),
                float(row["long_run_vol_annual"]),
                float(row["current_vol_annual"]),
                float(row["one_year_forecast_vol_annual"]),
                float(row["log_likelihood"]),
            ),
        )
    print(f"Loaded {len(rows)} FTSE result row(s) into Postgres.")


def load_oos_result(cur, path):
    rows = read_rows(path)
    for row in rows:
        cur.execute(
            """
            INSERT INTO garch_oos_results
                (train_n, test_n, qlike_garch, qlike_constant, mse_garch, mse_constant)
            VALUES (%s, %s, %s, %s, %s, %s)
            """,
            (
                int(row["train_n"]),
                int(row["test_n"]),
                float(row["qlike_garch"]),
                float(row["qlike_constant"]),
                float(row["mse_garch"]),
                float(row["mse_constant"]),
            ),
        )
    print(f"Loaded {len(rows)} out of sample row(s) into Postgres.")


def main():
    validation_csv = "garch_validation.csv"
    ftse_csv = "garch_ftse_result.csv"
    oos_csv = "garch_oos_result.csv"

    for path in (validation_csv, ftse_csv, oos_csv):
        if not os.path.exists(path):
            print(f"Error: {path} not found. Run the garch executable first.")
            sys.exit(1)

    conn = psycopg2.connect(get_connection_string())
    try:
        with conn:
            with conn.cursor() as cur:
                load_validation_results(cur, validation_csv)
                load_ftse_result(cur, ftse_csv)
                load_oos_result(cur, oos_csv)
    finally:
        conn.close()


if __name__ == "__main__":
    main()
