# Project Genetic
Portfolio Optimization using Genetic Algorithm

## Dependencies

- C++17 with GCC
- CMake
- Python
- uv

## How to build 
In root directory, run:

```bash
cmake -B build
cmake --build build
```

## How to run
First, you must get initial data 
```
cd Program/lstm_model/
uv run data.py
```

Then, run the runner file in the build directory(for Windows, its an exe)

To run LSTM:
```
cd Program/lstm_model/
uv run main.py
```

## How it works
A Long Short-Term Memory model is trained on past historical data.
Example inputs are opening price and Stochastic Oscillation

Technical indicators are computed day-by-day and written to a CSV, with all raw data being normalized. The model predicts percentage returns rather than raw prices.
A PyTorch LSTM is trained on the normalized indicator data using a custom combined loss (MSE + directional penalty) to optimize both magnitude accuracy and directional accuracy.
The Genetic Algorithm takes the LSTM's return predictions and optimizes a portfolio allocation across tickers, factoring in risk/reward tradeoffs.

## TODO
update cmake to standardize where executables are
implement concurrency for pre-processing
create CLI program
implement magnitude accuracy indicators
