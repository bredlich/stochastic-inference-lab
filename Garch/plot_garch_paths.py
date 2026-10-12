"Compare a constant volatility return series with a GARCH(1,1) series tha has the same long run variance, to show volatility clustering."

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# These paths are simulated in Python for illustration only. The estimation
# and validation results come from C++ .

n = 1000
alpha, beta = 0.10, 0.85
long_run_variance = 1e-4
omega = long_run_variance * (1 - alpha - beta)
rng = np.random.default_rng(42)

constant = np.sqrt(long_run_variance) * rng.standard_normal(n)

garch = np.zeros(n)
h = long_run_variance
for t in range(n):
    garch[t] = np.sqrt(h) * rng.standard_normal()
    h = omega + alpha * garch[t] ** 2 + beta * h

fig, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True, sharey=True)
series = [(constant, "Constant volatility", "#1f77b4"),
          (garch, "GARCH(1,1), alpha = 0.10, beta = 0.85", "#d62728")]

for ax, (x, title, colour) in zip(axes, series):
    rolling = pd.Series(x).rolling(20).std()
    ax.plot(x, color=colour, linewidth=0.7, alpha=0.7)
    ax.plot(rolling, color="black", linewidth=1.4, label="20 day rolling volatility")
    ax.plot(-rolling, color="black", linewidth=1.4)
    ax.axhline(0, color="gray", linewidth=0.5, linestyle="--")
    ax.set_title(title)
    ax.set_ylabel("Return")
    ax.grid(alpha=0.3)
axes[0].legend(loc="upper right")
axes[1].set_xlabel("t")

fig.suptitle("Same long run variance, different behaviour")
plt.tight_layout()
plt.savefig("garch_paths.png", dpi=150)
print("Saved garch_paths.png")
