"""Visualise AR(1) sample paths at different phi values to show the
qualitative effect of the autoregressive parameter."""

import numpy as np
import matplotlib.pyplot as plt

# This plot re-simulates paths in Python (NumPy) purely for illustration.
# The estimation and validation results come from the C++ implementation
# (ar1_validation.csv).


def simulate_ar1(phi, c, sigma, n, seed):
    rng = np.random.default_rng(seed)
    path = np.zeros(n + 1)
    path[0] = c / (1 - phi)
    for t in range(1, n + 1):
        path[t] = c + phi * path[t - 1] + rng.normal(0, sigma)
    return path


phis = [-0.7, 0.0, 0.5, 0.95]
n = 300
seed = 42

fig, axes = plt.subplots(2, 2, figsize=(10, 7), sharex=True)

for ax, phi in zip(axes.flat, phis):
    path = simulate_ar1(phi, 0.0, 1.0, n, seed)
    ax.plot(path, color="#1f77b4", linewidth=1)
    ax.axhline(0, color="gray", linewidth=0.5, linestyle="--")
    ax.set_title(f"phi = {phi}")
    ax.grid(alpha=0.3)

fig.suptitle("AR(1) sample paths across phi values")
fig.supxlabel("t")
fig.supylabel("X_t")
plt.tight_layout()
plt.savefig("ar1_paths.png", dpi=150)
print("Saved ar1_paths.png")
