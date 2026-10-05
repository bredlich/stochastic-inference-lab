# Reads AR(1) results back from PostgreSQL.
# Uses the PG_CONN_STRING environment variable.

import os

import pandas as pd
import psycopg2

conn_string = os.environ.get("PG_CONN_STRING")
if not conn_string:
    raise RuntimeError("PG_CONN_STRING environment variable not set")

conn = psycopg2.connect(conn_string)
try:
    validation = pd.read_sql(
        "SELECT true_phi, estimated_phi, absolute_error "
        "FROM ar1_validation_results ORDER BY id",
        conn,
    )
    ftse = pd.read_sql(
        "SELECT estimated_c, estimated_phi, rmse_ar1, rmse_naive "
        "FROM ar1_ftse_results ORDER BY id",
        conn,
    )
finally:
    conn.close()

print("AR(1) validation results:")
print(validation.to_string(index=False))
print("\nAR(1) FTSE result:")
print(ftse.to_string(index=False))
