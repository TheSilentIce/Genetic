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
From root directory:
```
cd Program/lstm_model/
uv run data.py
```

Then, run the runner file in the build directory(For windows, its an exe)

Afterwards:
From root directory
```
cd Program/lstm_model/
uv run main.py
```

## How it works
A Long Short-Term Memory model is trained on past historical data.
Example inputs are opening price and Stochastic Oscillation

It generates a prediction of prices for a set time interval.

The Genetic Algorithm then generates a portfolio based on those prices as well as other factors(risk/reward/etc..)

## Future
Implementing Metal and CUDA for GPU acceleration

