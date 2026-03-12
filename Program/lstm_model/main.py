import time

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import torch
from sklearn.preprocessing import MinMaxScaler
from torch import nn
from torch.utils.data import Dataset

start_time = time.time()

# ── Load CSVs (flat format with Ticket column) ────────────────────────────────
train_df = pd.read_csv("../data/new_training.csv")
test_df = pd.read_csv("../data/new_testing.csv")

print(f"Train rows: {len(train_df)} | Test rows: {len(test_df)}")
print(f"Tickers: {sorted(train_df['Ticket'].unique())}")

gpu = torch.device("cpu")
if torch.backends.mps.is_available():
    gpu = torch.device("mps")


# ── Dataset ───────────────────────────────────────────────────────────────────
class StockDataSet(Dataset):
    def __init__(
        self, ticker_data, seq_length=30, scaler_X=None, scaler_y=None, fit_scalers=True
    ):
        self.ticker = ticker_data["Ticket"].iloc[0]
        self.seq_length = seq_length

        feature_cols = [
            "Open",
            "High",
            "Low",
            "Volume",
            "OBV",
            "RSI",
            "SMA",
            "EMA",
            "%K",
            "%D",
        ]

        X_data = ticker_data[feature_cols].values
        y_data = ticker_data["Close"].values.reshape(-1, 1)

        if fit_scalers:
            self.scaler_X = MinMaxScaler()
            self.scaler_y = MinMaxScaler()
            X_scaled = self.scaler_X.fit_transform(X_data)
            y_scaled = self.scaler_y.fit_transform(y_data)
        else:
            if scaler_X is None or scaler_y is None:
                raise ValueError("Must provide scalers for test set")
            self.scaler_X = scaler_X
            self.scaler_y = scaler_y
            X_scaled = self.scaler_X.transform(X_data)
            y_scaled = self.scaler_y.transform(y_data)

        self.X = []
        self.y = []
        for i in range(len(X_scaled) - seq_length):
            self.X.append(X_scaled[i : i + seq_length])
            self.y.append(y_scaled[i + seq_length])

        self.X = torch.tensor(self.X, dtype=torch.float32, device=gpu)
        self.y = torch.tensor(self.y, dtype=torch.float32, device=gpu)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.y[idx]


# ── Build datasets ────────────────────────────────────────────────────────────
train_datasets = {}
test_datasets = {}

common_tickers = set(train_df["Ticket"].unique()) & set(test_df["Ticket"].unique())

for ticker in common_tickers:
    train_group = train_df[train_df["Ticket"] == ticker].reset_index(drop=True)
    test_group = test_df[test_df["Ticket"] == ticker].reset_index(drop=True)

    if len(train_group) > 30 and len(test_group) > 30:
        train_ds = StockDataSet(train_group, seq_length=30, fit_scalers=True)
        test_ds = StockDataSet(
            test_group,
            seq_length=30,
            scaler_X=train_ds.scaler_X,
            scaler_y=train_ds.scaler_y,
            fit_scalers=False,
        )
        train_datasets[ticker] = train_ds
        test_datasets[ticker] = test_ds

print(f"Training on {len(train_datasets)} stocks")


# ── Model ─────────────────────────────────────────────────────────────────────
class LSTMModel(nn.Module):
    def __init__(self, input_dim, hidden_dim, layer_dim, output_dim):
        super(LSTMModel, self).__init__()
        self.hidden_dim = hidden_dim
        self.layer_dim = layer_dim
        self.lstm = nn.LSTM(input_dim, hidden_dim, layer_dim, batch_first=True)
        self.fc = nn.Linear(hidden_dim, output_dim)

    def forward(self, x):
        out, _ = self.lstm(x)
        out = self.fc(out[:, -1, :])
        return out


model = LSTMModel(input_dim=10, hidden_dim=256, layer_dim=2, output_dim=1).to(gpu)
criterion = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
num_epochs = 120

# ── Training ──────────────────────────────────────────────────────────────────
print("\n=== Training ===")
for epoch in range(num_epochs):
    model.train()
    epoch_loss = 0.0

    for ticker, dataset in train_datasets.items():
        optimizer.zero_grad()
        outputs = model(dataset.X)
        loss = criterion(outputs, dataset.y)
        loss.backward()
        optimizer.step()
        epoch_loss += loss.item()

    if (epoch + 1) % 10 == 0:
        avg_loss = epoch_loss / len(train_datasets)
        print(f"Epoch [{epoch+1}/{num_epochs}], Avg Loss: {avg_loss:.4f}")


# ── Helpers ───────────────────────────────────────────────────────────────────
def directional_accuracy(pred: np.ndarray, actual: np.ndarray) -> float:
    """% of predictions where up/down direction matches actual."""
    pred_dir = np.sign(pred)
    actual_dir = np.sign(actual)
    return (pred_dir == actual_dir).mean() * 100


def magnitude_accuracy(
    pred: np.ndarray, actual: np.ndarray, tolerance: float = 0.005
) -> float:
    """% of predictions within ±tolerance of actual (default 0.5 pp)."""
    return (np.abs(pred - actual) <= tolerance).mean() * 100


# ── Testing ───────────────────────────────────────────────────────────────────
print("\n=== Testing on Unseen Data (% change predictions) ===")
model.eval()

with torch.no_grad():
    num_stocks = len(test_datasets)
    fig, axes = plt.subplots(num_stocks, 1, figsize=(12, 5 * num_stocks))
    if num_stocks == 1:
        axes = [axes]

    overall_mae = []
    overall_dir_acc = []
    overall_mag_acc = []

    for idx, (ticker, dataset) in enumerate(test_datasets.items()):
        pred_scaled = model(dataset.X)
        # Inverse-transform back to raw % change values
        pred_pct = dataset.scaler_y.inverse_transform(
            pred_scaled.cpu().numpy()
        ).flatten()
        actual_pct = dataset.scaler_y.inverse_transform(
            dataset.y.cpu().numpy()
        ).flatten()

        # ── Per-prediction stats ──────────────────────────────────────────────
        abs_errors = np.abs(pred_pct - actual_pct)
        mae = abs_errors.mean()  # mean abs error in pct-change units
        mae_pp = mae * 100  # convert to percentage points

        dir_acc = directional_accuracy(pred_pct, actual_pct)  # up/down accuracy %
        mag_acc = magnitude_accuracy(
            pred_pct, actual_pct, tolerance=0.005
        )  # within 0.5pp

        overall_mae.append(mae_pp)
        overall_dir_acc.append(dir_acc)
        overall_mag_acc.append(mag_acc)

        print(f"\n{ticker}:")
        print(f"  Test set size: {len(pred_pct)} predictions")
        print(f"  {'Sample':>6}  {'Pred':>9}  {'Actual':>9}  {'Error':>9}  {'Dir':>5}")
        print(f"  {'-'*46}")
        for i in range(min(10, len(pred_pct))):
            p, a = pred_pct[i], actual_pct[i]
            err = abs(p - a)
            match = "✓" if np.sign(p) == np.sign(a) else "✗"
            print(
                f"  [{i:>4}]  {p*100:>+8.3f}%  {a*100:>+8.3f}%  {err*100:>8.3f}pp  {match:>5}"
            )

        print(f"\n  Mean Abs Error    : {mae_pp:.4f} percentage points")
        print(f"  Directional Acc   : {dir_acc:.1f}%  (up/down correct)")
        print(f"  Magnitude Acc     : {mag_acc:.1f}%  (within ±0.5pp of actual)")

        ax = axes[idx]
        ax.plot(actual_pct * 100, label="Actual % Change", linewidth=2)
        ax.plot(pred_pct * 100, label="Predicted % Change", linewidth=2, alpha=0.7)
        ax.axhline(0, color="black", linewidth=0.8, linestyle="--")
        ax.set_title(
            f"{ticker} — Dir Acc: {dir_acc:.1f}%  |  MAE: {mae_pp:.4f}pp  |  Mag Acc: {mag_acc:.1f}%"
        )
        ax.set_xlabel("Time Step")
        ax.set_ylabel("% Change")
        ax.legend()
        ax.grid(True, alpha=0.3)

    print(f"\n{'='*55}")
    print("OVERALL TEST PERFORMANCE")
    print(f"  Mean Abs Error    : {np.mean(overall_mae):.4f} percentage points")
    print(f"  Directional Acc   : {np.mean(overall_dir_acc):.1f}%")
    print(f"  Magnitude Acc     : {np.mean(overall_mag_acc):.1f}%  (within ±0.5pp)")
    print(f"  Best Dir Acc      : {max(overall_dir_acc):.1f}%")
    print(f"  Worst Dir Acc     : {min(overall_dir_acc):.1f}%")
    print(f"{'='*55}")

    plt.tight_layout()
    plt.savefig("test_predictions_pct.png", dpi=300, bbox_inches="tight")
    print("\n✅ Plot saved as 'test_predictions_pct.png'")
    plt.show()


# ── Save predictions to txt ───────────────────────────────────────────────────
has_date_col = "Date" in test_df.columns

with open("../data/predictions.txt", "w") as f:
    for ticker in sorted(test_datasets.keys()):
        dataset = test_datasets[ticker]

        with torch.no_grad():
            pred_scaled = model(dataset.X)

        pred_pct = dataset.scaler_y.inverse_transform(pred_scaled.cpu().numpy())

        group = test_df[test_df["Ticket"] == ticker]
        group = (
            group.sort_values("Date").reset_index(drop=True)
            if has_date_col
            else group.reset_index(drop=True)
        )

        f.write(f"{ticker}\n")
        for i in range(len(pred_pct)):
            date_val = (
                str(group["Date"].iloc[i + dataset.seq_length])[:10]
                if has_date_col and (i + dataset.seq_length) < len(group)
                else f"step_{i:04d}"
            )
            f.write(f"{date_val}, {pred_pct[i][0]:+.6f}\n")
        f.write("\n")

print(f"\n✅ Predictions saved to '../data/predictions.txt'")
print(f"⏱  Total time: {time.time() - start_time:.1f}s")
