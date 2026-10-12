# Reads GARCH results back from PostgreSQL.

import os

import pandas as pd
import psycopg2

conn_string = os.environ.get("PG_CONN_STRING")
if not conn_string:
    raise RuntimeError("PG_CONN_STRING environment variable not set")

queries = {
    "GARCH validation results": "SELECT true_alpha, estimated_alpha, true_beta, estimated_beta, "
                                "true_persistence, estimated_persistence, absolute_error_persistence "
                                "FROM garch_validation_results ORDER BY id",
    "GARCH FTSE result": "SELECT estimated_alpha, estimated_beta, persistence, half_life_days, "
                         "long_run_vol_annual, current_vol_annual, one_year_forecast_vol_annual "
                         "FROM garch_ftse_results ORDER BY id",
    "GARCH out of sample result": "SELECT train_n, test_n, qlike_garch, qlike_constant, "
                                  "mse_garch, mse_constant FROM garch_oos_results ORDER BY id",
}

conn = psycopg2.connect(conn_string)
try:
    for title, query in queries.items():
        print(title + ":")
        print(pd.read_sql(query, conn).to_string(index=False))
        print()
finally:
    conn.close()
