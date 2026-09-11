# Compares fractional Brownian motion paths across three Hurst values,
# demonstrating anti-persistent (H<0.5), standard (H=0.5), and
# persistent (H>0.5) behaviour side by side.
# Date: 11/09/2026

import subprocess
import pandas as pd
import matplotlib.pyplot as plt

hurst_values = [0.3, 0.5, 0.7]
labels = ["H = 0.3 (anti-persistent)", "H = 0.5 (standard BM)", "H = 0.7 (persistent)"]

fig, axes = plt.subplots(1, 3, figsize=(15, 4), sharey=True)

for ax, H, label in zip(axes, hurst_values, labels):
    subprocess.run(["build/Debug/fbm.exe", str(H), "300", "1", "42"])
    data = pd.read_csv("fbm_paths.csv")
    ax.plot(data["t"], data["path_0"])
    ax.set_title(label)
    ax.set_xlabel("t")

axes[0].set_ylabel("Value")
fig.suptitle("Effect of the Hurst Parameter on Path Behaviour")
plt.tight_layout()
plt.savefig("hurst_comparison.png", dpi=300, bbox_inches="tight")
plt.show()
