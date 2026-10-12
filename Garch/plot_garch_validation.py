"""Parameter recovery for the GARCH(1,1) estimator on synthetic data.

Left: true against estimated values for the default seed.
Right: signed error (estimated minus true) for every seed in the sweep, which
shows what the left panel hides at this scale."""

import glob

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("garch_validation.csv")

fig, axes = plt.subplots(1, 2, figsize=(12, 5.5))

ax = axes[0]
ax.scatter(df["true_alpha"], df["estimated_alpha"], s=70, color="#1f77b4", label="alpha", zorder=3)
ax.scatter(df["true_beta"], df["estimated_beta"], s=70, color="#2ca02c", label="beta", zorder=3)
ax.scatter(df["true_persistence"], df["estimated_persistence"], s=70, marker="s",
           color="#d62728", label="alpha + beta", zorder=3)
ax.plot([0, 1], [0, 1], linestyle="--", color="gray", label="y = x (perfect recovery)")
ax.set_xlim(-0.02, 1.02)
ax.set_ylim(-0.02, 1.02)
ax.set_xlabel("True value")
ax.set_ylabel("Estimated value")
ax.set_title("Default seed")
ax.legend(loc="upper left")
ax.grid(alpha=0.3)

# Signed errors from every seed in the sweep (falls back to the default run).
files = sorted(glob.glob("garch_validation_seed*.csv")) or ["garch_validation.csv"]
sweep = pd.concat([pd.read_csv(f) for f in files], ignore_index=True)
errors = {
    "alpha": sweep["estimated_alpha"] - sweep["true_alpha"],
    "beta": sweep["estimated_beta"] - sweep["true_beta"],
    "alpha + beta": sweep["estimated_persistence"] - sweep["true_persistence"],
}
colours = ["#1f77b4", "#2ca02c", "#d62728"]
rng = np.random.default_rng(0)

ax = axes[1]
for i, (name, err) in enumerate(errors.items()):
    jitter = rng.uniform(-0.12, 0.12, len(err))
    ax.scatter(i + jitter, err, s=35, color=colours[i], alpha=0.8, zorder=3)
    ax.hlines(err.mean(), i - 0.3, i + 0.3, color="black", linewidth=2, zorder=4)
ax.axhline(0, color="gray", linestyle="--", linewidth=0.8)
ax.set_xticks(range(len(errors)))
ax.set_xticklabels(errors.keys())
ax.set_ylabel("Estimated minus true")
ax.set_title(f"Error across {len(files)} seeds ({len(sweep)} fits), black bar = mean")
ax.grid(alpha=0.3)

fig.suptitle("GARCH(1,1) parameter recovery")
plt.tight_layout()
plt.savefig("garch_validation.png", dpi=150)
print("Saved garch_validation.png")
