import time

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import DataSet
import LSTMModel
import constants

start_time = time.time()


def print_stats(stats: dict, indent: int = 2):
    pad = " " * indent
    print(f"{pad}Directional Acc  : {stats['dir_acc']:.1f}%")
    print(f"{pad}Magnitude Acc    : {stats['mag_acc']:.1f}%   (within ±0.5pp)")
    print(f"{pad}MAE              : {stats['mae']:.4f} pp")
    print(f"{pad}MAD (median)     : {stats['mad']:.4f} pp")
    print(f"{pad}RMSE             : {stats['rmse']:.4f} pp")
    print(f"{pad}MAPE             : {stats['mape']:.2f}%")
    print(f"{pad}Std of errors    : {stats['std_err']:.4f} pp")
    print(f"{pad}Max error        : {stats['max_err']:.4f} pp")
    print(f"{pad}Bias             : {stats['bias']:+.4f} pp")


train = pd.read_csv("../data/new_training.csv")
test = pd.read_csv("../data/new_testing.csv")
pd.concat([train, test]).to_csv("../data/full_data.csv", index=False)

full_df = pd.read_csv("../data/full_data.csv")
print(f"Total rows: {len(full_df)}")
print(f"Tickers   : {sorted(full_df['Ticket'].unique())}")

splits = DataSet.make_walk_forward_splits(full_df, constants.N_SPLITS)
fold_results = []

print(f"\nRunning {len(splits)}-fold walk-forward validation")
print("=" * 55)

for fold_idx, (train_df, test_df) in enumerate(splits):
    n_tickers = len(full_df["Ticket"].unique())
    train_rows = len(train_df) // n_tickers
    test_rows = len(test_df) // n_tickers

    print(
        f"\n── Fold {fold_idx+1}/{len(splits)}  "
        f"(train: {train_rows} rows/ticker | test: {test_rows} rows/ticker) ──"
    )

    train_datasets, test_datasets = DataSet.build_datasets(train_df, test_df)
    print(f"  Tickers in fold: {len(train_datasets)}")

    model = LSTMModel.train_model(train_datasets)
    per_ticker, fold_stats = LSTMModel.evaluate_model(model, test_datasets)
    fold_results.append(fold_stats)

    print(f"\n  Fold {fold_idx+1} results:")
    print_stats(fold_stats)

    dir_accs = [s["dir_acc"] for s in per_ticker.values()]
    print(
        f"\n  Per-ticker dir acc — "
        f"best: {max(dir_accs):.1f}%  "
        f"worst: {min(dir_accs):.1f}%  "
        f"std: {np.std(dir_accs):.1f}pp"
    )


# ── Cross-fold summary ────────────────────────────────────────────────────────
print(f"\n{'='*55}")
print("WALK-FORWARD SUMMARY")
print("=" * 55)

for metric in fold_results[0].keys():
    values = [r[metric] for r in fold_results]
    print(
        f"  {metric:<16} "
        f"mean: {np.mean(values):>8.4f}   "
        f"std: {np.std(values):>7.4f}   "
        f"min: {np.min(values):>8.4f}   "
        f"max: {np.max(values):>8.4f}"
    )

dir_accs_per_fold = [r["dir_acc"] for r in fold_results]
print(f"\n  Dir acc per fold : {[f'{x:.1f}%' for x in dir_accs_per_fold]}")

print(f"\n{'='*55}")
print(f"⏱  Total time: {time.time() - start_time:.1f}s")
