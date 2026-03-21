import time

import numpy as np
import pandas as pd
import torch
import DataSet
import LSTMModel
import constants

start_time = time.time()


def compute_baseline(test_datasets: dict) -> dict:
    results = {}

    for ticker, dataset in test_datasets.items():
        actual = dataset.scaler_y.inverse_transform(dataset.y.cpu().numpy()).flatten()

        n = len(actual)
        actual_sign = np.sign(actual)

        always_up = (actual_sign == 1).mean() * 100
        always_down = (actual_sign == -1).mean() * 100
        random_acc = ((np.random.choice([-1, 1], size=n) == actual_sign)).mean() * 100
        persistence = (actual_sign[1:] == actual_sign[:-1]).mean() * 100

        results[ticker] = {
            "always_up_acc": round(always_up, 2),
            "always_down_acc": round(always_down, 2),
            "random_acc": round(random_acc, 2),
            "persistence_acc": round(persistence, 2),
            "actual_pct_up": round(always_up, 2),
        }

    return results


def print_baseline_vs_model(baseline: dict, per_ticker: dict):
    print(
        f"\n{'Ticker':<10} {'Always Up':>10} {'Persist':>10} {'LSTM':>10}  {'Edge':>10}"
    )
    print("-" * 55)
    for ticker in baseline:
        always_up = baseline[ticker]["always_up_acc"]
        persist = baseline[ticker]["persistence_acc"]
        lstm_acc = per_ticker[ticker]["dir_acc"]
        edge = lstm_acc - always_up
        print(
            f"{ticker:<10} {always_up:>9.1f}% {persist:>9.1f}% {lstm_acc:>9.1f}%  {edge:>+9.1f}%"
        )


def print_stats(stats: dict, indent: int = 2):
    pad = " " * indent
    print(f"{pad}Directional Acc  : {stats['dir_acc']:.1f}%")
    print(f"{pad}Magnitude Acc    : {stats['mag_acc']:.1f}%   (within ±0.5pp)")
    print(f"{pad}MAD (median)     : {stats['mad']:.4f} pp")
    print(f"{pad}RMSE             : {stats['rmse']:.4f} pp")
    print(f"{pad}MAE              : {stats['mae']:.2f}%")
    print(f"{pad}Std of errors    : {stats['std_err']:.4f} pp")
    print(f"{pad}Max error        : {stats['max_err']:.4f} pp")
    print(f"{pad}Bias             : {stats['bias']:+.4f} pp")


train = pd.read_csv("../data/new_training.csv")
test = pd.read_csv("../data/new_testing.csv")
full_df = pd.concat([train, test]).reset_index(drop=True)

splits = DataSet.make_walk_forward_splits(full_df, constants.N_SPLITS)
fold_results = []

best_dir_acc = -1.0
best_model_state = None
best_fold_idx = -1

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

    baseline = compute_baseline(test_datasets)
    print_baseline_vs_model(baseline, per_ticker)

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

    # ── Save best fold model ──────────────────────────────────────────────────
    if fold_stats["dir_acc"] > best_dir_acc:
        best_dir_acc = fold_stats["dir_acc"]
        best_model_state = {k: v.cpu().clone() for k, v in model.state_dict().items()}
        best_fold_idx = fold_idx + 1


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

# ── Save best model ───────────────────────────────────────────────────────────
save_path = "lstm_best.pth"
torch.save(
    {
        "model_state_dict": best_model_state,
        "fold": int(best_fold_idx),
        "dir_acc": float(best_dir_acc),
        "input_dim": int(len(constants.feature_cols)),
        "hidden_dim": int(constants.HIDDEN_DIM),
        "layer_dim": 2,
        "output_dim": 1,
    },
    save_path,
)
print(
    f"\n  ✓ Best model (fold {best_fold_idx}, dir_acc={best_dir_acc:.1f}%) saved → {save_path}"
)

print(f"\n{'='*55}")
print(f"⏱  Total time: {time.time() - start_time:.1f}s")
