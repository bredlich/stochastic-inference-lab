# Plots the log(n) vs log(R/S) diagnostic underlying the Hurst estimate,
# for both the synthetic fBm validation case and the real FTSE 100 data.
# Date: 29/09/2026

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

synthetic = pd.read_csv("hurst_loglog_synthetic.csv")
ftse = pd.read_csv("hurst_loglog_ftse.csv")

fig, axes = plt.subplots(1, 2, figsize=(13, 5))

def plot_panel(ax, data, title):
    x = data["log_n"]
    y = data["log_RS"]
    slope, intercept = np.polyfit(x, y, 1)
    ax.scatter(x, y, color="tab:blue", zorder=3)
    ax.plot(x, slope * x + intercept, "r--", label=f"Fitted slope (H) = {slope:.3f}")
    ax.set_xlabel("log(n)")
    ax.set_ylabel("log(R/S)")
    ax.set_title(title)
    ax.legend()
    ax.grid(alpha=0.3)

plot_panel(axes[0], synthetic, "Synthetic fBm (true H = 0.7)")
plot_panel(axes[1], ftse, "FTSE 100 Log Returns")

fig.suptitle("Rescaled Range (R/S) Analysis: log-log Diagnostic")
plt.tight_layout()
plt.savefig("hurst_loglog_diagnostic.png", dpi=300, bbox_inches="tight")
plt.show()
