"Two panels for the FTSE 100 fit: autocorrelation of returns against squared returns and the fitted conditional volatility over time."

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

acf = pd.read_csv("garch_acf_ftse.csv")
sigma = pd.read_csv("garch_ftse_sigma.csv")
result = pd.read_csv("garch_ftse_result.csv")

n = len(sigma)
band = 1.96 / np.sqrt(n)

fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))

width = 0.4
axes[0].bar(acf["lag"] - width / 2, acf["acf_returns"], width, color="#1f77b4", label="Returns")
axes[0].bar(acf["lag"] + width / 2, acf["acf_squared_returns"], width, color="#d62728",
            label="Squared returns")
axes[0].axhline(band, color="gray", linestyle="--", linewidth=0.8)
axes[0].axhline(-band, color="gray", linestyle="--", linewidth=0.8, label="95% band under no autocorrelation")
axes[0].set_xticks([1, 5, 10, 15, 20])
axes[0].set_xlabel("Lag (days)")
axes[0].set_ylabel("Autocorrelation")
axes[0].set_title("FTSE 100 autocorrelation")
axes[0].legend(loc="upper center", bbox_to_anchor=(0.5, -0.15), ncol=3, fontsize=8)
axes[0].grid(alpha=0.3)

axes[1].plot(sigma["t"], sigma["conditional_vol_annual"], color="#d62728", linewidth=1)
axes[1].axhline(result["long_run_vol_annual"].iloc[0], color="gray", linestyle="--",
                label="Long run volatility")
axes[1].set_xlabel("Trading day")
axes[1].set_ylabel("Annualised volatility")
axes[1].set_title("Fitted conditional volatility")
axes[1].legend()
axes[1].grid(alpha=0.3)

plt.tight_layout()
plt.savefig("garch_ftse.png", dpi=150)
print("Saved garch_ftse.png")
