# Demonstrates Monte Carlo convergence toward the Black-Scholes price
# as the number of simulations increases. Runs monte_carlo.exe at a
# range of simulation counts and plots the resulting price against
# the true closed-form value.
# Date: 07/09/2026

import subprocess
import matplotlib.pyplot as plt

simulation_counts = [100, 1000, 5000, 10000, 50000, 100000, 500000]
mc_prices = []
bs_price = None

for n in simulation_counts:
    # args: K_multiplier r T numSimulations seed
    result = subprocess.run(
        ["build/Debug/monte_carlo.exe", "1.0", "0.04", "1.0", str(n), "42"],
        capture_output=True, text=True
    )
    output = result.stdout

    for line in output.splitlines():
        if "Monte Carlo price:" in line:
            mc_prices.append(float(line.split(":")[1].strip()))
        if "Black-Scholes price:" in line:
            bs_price = float(line.split(":")[1].strip())

plt.figure(figsize=(9, 5))
plt.plot(simulation_counts, mc_prices, "o-", label="Monte Carlo estimate")
plt.axhline(y=bs_price, color="red", linestyle="--", label=f"Black-Scholes price ({bs_price:.4f})")
plt.xscale("log")
plt.xlabel("Number of simulations (log scale)")
plt.ylabel("Option price")
plt.title("Monte Carlo Convergence to Black-Scholes Price")
plt.legend()
plt.savefig("mc_convergence.png", dpi=300, bbox_inches="tight")
plt.show()
