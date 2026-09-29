# Plots true vs estimated Hurst exponent from validation runs on synthetic fBm.
# Date: 29/09/2026

import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("hurst_validation.csv")

plt.figure(figsize=(7, 7))
plt.scatter(data["true_H"], data["estimated_H"], s=80, color="tab:blue", zorder=3, label="Estimated")
plt.plot([0, 1], [0, 1], "r--", label="Perfect recovery (y = x)")

for _, row in data.iterrows():
    plt.annotate(f"H={row['true_H']}", (row["true_H"], row["estimated_H"]),
                 textcoords="offset points", xytext=(8, -4), fontsize=9)

plt.xlabel("True Hurst exponent")
plt.ylabel("Estimated Hurst exponent (R/S analysis)")
plt.title("Hurst Exponent Estimator: Validation Against Known Ground Truth")
plt.xlim(0, 1)
plt.ylim(0, 1)
plt.legend()
plt.grid(alpha=0.3)
plt.savefig("hurst_validation.png", dpi=300, bbox_inches="tight")
plt.show()
