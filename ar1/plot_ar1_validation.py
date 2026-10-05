"""Scatter plot of true vs estimated phi for AR(1) validation."""

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("ar1_validation.csv")

fig, ax = plt.subplots(figsize=(6, 6))
ax.scatter(df["true_phi"], df["estimated_phi"], color="#1f77b4", s=70, zorder=3)

lims = [-1, 1]
ax.plot(lims, lims, linestyle="--", color="gray", label="y = x (perfect recovery)")

for _, row in df.iterrows():
    ax.annotate(
        f"{row['true_phi']:.2f}",
        (row["true_phi"], row["estimated_phi"]),
        textcoords="offset points",
        xytext=(8, -4),
    )

ax.set_xlabel("True phi")
ax.set_ylabel("Estimated phi")
ax.set_title("AR(1) parameter recovery: true vs estimated phi")
ax.set_xlim(-1, 1)
ax.set_ylim(-1, 1)
ax.legend()
ax.grid(alpha=0.3)

plt.tight_layout()
plt.savefig("ar1_validation.png", dpi=150)
print("Saved ar1_validation.png")
