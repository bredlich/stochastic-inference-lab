# stochastic-inference-lab
C++ simulations of stochastic processes. Extending my background as a statistical programmer into a first-principles understanding of the underlying simulation methods. Includes Python plotting scripts and shell-scripted experiment pipelines.

## Projects

| Project | Description |
|---|---|
| [Standard Brownian motion generator](brownian_motion/) | Simulates a Wiener process via cumulative Gaussian increments, with CLI-configurable parameters, a fixed-seed reproducibility option, Python plotting, and a shell script for parameter sweeps |
| [Monte Carlo option pricing](monte_carlo/) | Prices a European call option on the FTSE 100 via Monte Carlo simulation of GBM paths, validated against the closed-form Black-Scholes price, with volatility estimated directly from real historical data |
| [Fractional Brownian motion generator](fractional_brownian_motion/) | Simulates fBm via Cholesky decomposition of the covariance matrix, parameterised by the Hurst exponent, with verification that H=0.5 exactly recovers standard Brownian motion |
| [Hurst exponent estimation](hurst_estimation/) | Estimates the Hurst exponent via Rescaled Range analysis, validated against synthetic fBm with known ground truth, then applied to real FTSE 100 data to test for long-range dependence |
| [AR(1) process simulator](ar1/) | Simulates and validates a first order autoregressive process, then fits it to real FTSE 100 log returns to test for short range linear dependence and one step ahead forecastability |
| [GARCH-lite](Garch/) | Simulates a GARCH(1,1) process, estimates it by maximum likelihood and forecasts volatility, validated on synthetic data and then applied to FTSE 100 returns to test for volatility clustering |

## Running with Docker

The image builds every project and runs its tests, its main program and its plotting scripts. It needs only Docker.

```
docker run --rm -v "$(pwd)/results:/results" ghcr.io/bredlich/stochastic-inference-lab:latest
```

On Windows PowerShell use `${PWD}` in place of `$(pwd)`. The CSV and PNG outputs for each project are saved in `results/<project>/`.

The same seed gives the same numbers within one toolchain. Results on the real FTSE 100 data match the project READMEs to the printed digits. Results on synthetic data can differ in the last digits, because the C++ standard leaves the algorithm behind `std::normal_distribution` to each compiler's library. The READMEs report numbers from Windows with MSVC and the image uses Linux with GCC. For example the Hurst estimate for a true H of 0.3 is 0.391 in the README and 0.405 in the image.

The image does not include PostgreSQL, so the loaders are not run in it. They run in the GitHub Actions workflow against a PostgreSQL service container. GitHub Actions builds the image on every push and publishes it to the GitHub Container Registry from the main branch.
