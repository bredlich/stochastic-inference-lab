# stochastic-inference-lab
C++ simulations of stochastic processes. Extending my background as a statistical programmer into a first-principles understanding of the underlying simulation methods. Includes Python bindings and shell-scripted experiment pipelines.

## Projects

| Project | Description |
|---|---|
| [Standard Brownian motion generator](brownian_motion/) | Simulates a Wiener process via cumulative Gaussian increments, with CLI-configurable parameters, a fixed-seed reproducibility option, Python plotting, and a shell script for parameter sweeps |
| [Monte Carlo option pricing](monte_carlo/) | Prices a European call option on the FTSE 100 via Monte Carlo simulation of GBM paths, validated against the closed-form Black-Scholes price, with volatility estimated directly from real historical data |
| [Fractional Brownian motion generator](fractional_brownian_motion/) | Simulates fBm via Cholesky decomposition of the covariance matrix, parameterised by the Hurst exponent, with verification that H=0.5 exactly recovers standard Brownian motion |
| [Hurst exponent estimation](hurst_estimation/) | Estimates the Hurst exponent via Rescaled Range analysis, validated against synthetic fBm with known ground truth, then applied to real FTSE 100 data to test for long-range dependence |
| [AR(1) process simulator](ar1/) | Simulates and validates a first order autoregressive process, then fits it to real FTSE 100 log returns to test for short range linear dependence and one step ahead forecastability |
| [GARCH-lite](Garch/) | GARCH(1,1) simulator, maximum likelihood estimator and variance forecast, validated on synthetic data and applied to FTSE 100 returns | Persistence recovered to within 0.035 across 20 synthetic fits. FTSE 100 persistence is 0.646 with a half life of 1.6 days, driven by one week in April 2025. Lower QLIKE than a constant variance forecast out of sample. |
