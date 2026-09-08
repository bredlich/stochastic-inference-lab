# stochastic-inference-lab
C++ simulations of stochastic processes. Extending my background as a statistical programmer into a first-principles understanding of the underlying simulation methods. Includes Python bindings and shell-scripted experiment pipelines.

## Projects

| Project | Description |
|---|---|
| [Standard Brownian motion generator](brownian_motion/) | Simulates a Wiener process via cumulative Gaussian increments, with CLI-configurable parameters, a fixed-seed reproducibility option, Python plotting, and a shell script for parameter sweeps |
| [Monte Carlo option pricing](monte_carlo/) | Prices a European call option on the FTSE 100 via Monte Carlo simulation of GBM paths, validated against the closed-form Black-Scholes price, with volatility estimated directly from real historical data |
