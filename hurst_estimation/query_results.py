# Reads back the Hurst results stored in Postgres, confirming the C++
# load succeeded, and prints a summary. Connection string is read from
# the PG_CONN_STRING environment variable; no credentials in this file.
# Date: 29/09/2026

import os
import psycopg2
import pandas as pd

conn_string = os.environ.get("PG_CONN_STRING")
if conn_string is None:
    raise RuntimeError("PG_CONN_STRING environment variable not set")

conn = psycopg2.connect(conn_string)

validation = pd.read_sql("SELECT * FROM hurst_validation_results ORDER BY id", conn)
ftse = pd.read_sql("SELECT * FROM hurst_ftse_results ORDER BY id", conn)

print("Validation results stored in Postgres:")
print(validation)
print("\nFTSE result stored in Postgres:")
print(ftse)

conn.close()
