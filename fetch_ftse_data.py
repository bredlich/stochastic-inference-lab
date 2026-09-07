# Fetches FTSE 100 daily historical price data and saves it as the
# shared dataset used across the applied projects in this repository.
# Date: 07/09/2026

import yfinance as yf

ftse = yf.Ticker("^FTSE")
data = ftse.history(period="3y", interval="1d")

data.to_csv("data/ftse100.csv")
print(f"Saved {len(data)} rows to data/ftse100.csv")
