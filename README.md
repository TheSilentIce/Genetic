# Project Genetic

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


