import matplotlib.pyplot as plt
import pandas as pd
import torch
from sklearn.preprocessing import MinMaxScaler
from torch import nn
from torch.utils.data import Dataset

df = pd.read_csv("./data/new_data.csv")


class StockDataSet(Dataset):
    def __init__(
        self,
        ticker_data,
        seq_length=30,
        train=True,
        split_ratio=0.8,
        scaler_X=None,
        scaler_y=None,
    ):
        self.ticker = ticker_data["Ticket"].iloc[0]
        self.seq_length = seq_length

        # Split data by time (chronological split)
        split_idx = int(len(ticker_data) * split_ratio)

        if train:
            data = ticker_data.iloc[:split_idx]  # First 80% for training
        else:
            data = ticker_data.iloc[split_idx:]  # Last 20% for testing

        # Normalize data
        feature_cols = ["Open", "High", "Low", "RSI", "SMA", "EMA"]

        X_data = data[feature_cols].values
        y_data = data["Close"].values.reshape(-1, 1)

        if train:
            # FIT scaler on training data only
            self.scaler_X = MinMaxScaler()
            self.scaler_y = MinMaxScaler()
            X_scaled = self.scaler_X.fit_transform(X_data)
            y_scaled = self.scaler_y.fit_transform(y_data)
        else:
            # USE the training scaler (don't fit on test data)
            if scaler_X is None or scaler_y is None:
                raise ValueError("Must provide scalers for test set")
            self.scaler_X = scaler_X
            self.scaler_y = scaler_y
            X_scaled = self.scaler_X.transform(X_data)  # transform only
            y_scaled = self.scaler_y.transform(y_data)  # transform only

        # Create sequences
        self.X = []
        self.y = []
        for i in range(len(X_scaled) - seq_length):
            self.X.append(X_scaled[i : i + seq_length])
            self.y.append(y_scaled[i + seq_length])

        self.X = torch.FloatTensor(self.X)
        self.y = torch.FloatTensor(self.y)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.y[idx]


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


# Create train and test datasets with proper scaler sharing
train_datasets = {}
test_datasets = {}

for ticker, group in df.groupby("Ticket"):
    if len(group) > 30:  # Need enough data for sequences
        # Create training dataset (fits scaler)
        train_ds = StockDataSet(group, seq_length=30, train=True, split_ratio=0.8)

        # Create test dataset (uses training scaler)
        test_ds = StockDataSet(
            group,
            seq_length=30,
            train=False,
            split_ratio=0.8,
            scaler_X=train_ds.scaler_X,
            scaler_y=train_ds.scaler_y,
        )

        train_datasets[ticker] = train_ds
        test_datasets[ticker] = test_ds

print(f"Training on {len(train_datasets)} stocks")

# Model setup
input_dim = 6
model = LSTMModel(input_dim=input_dim, hidden_dim=50, layer_dim=2, output_dim=1)
criterion = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

num_epochs = 100

# Training (ONLY on training data)
print("\n=== Training ===")
for epoch in range(num_epochs):
    model.train()
    epoch_loss = 0.0

    for ticker, dataset in train_datasets.items():
        trainX = dataset.X
        trainY = dataset.y

        optimizer.zero_grad()
        outputs = model(trainX)
        loss = criterion(outputs, trainY)
        loss.backward()
        optimizer.step()

        epoch_loss += loss.item()

    if (epoch + 1) % 10 == 0:
        avg_loss = epoch_loss / len(train_datasets)
        print(f"Epoch [{epoch+1}/{num_epochs}], Avg Loss: {avg_loss:.4f}")

# Evaluation on TEST data (data model has NEVER seen)
print("\n=== Testing on Unseen Data ===")
model.eval()
with torch.no_grad():
    num_stocks = len(test_datasets)
    fig, axes = plt.subplots(num_stocks, 1, figsize=(12, 5 * num_stocks))

    if num_stocks == 1:
        axes = [axes]

    overall_errors = []

    for idx, (ticker, dataset) in enumerate(test_datasets.items()):
        predictions = model(dataset.X)

        # Inverse transform to get actual prices
        pred_prices = dataset.scaler_y.inverse_transform(predictions.numpy())
        actual_prices = dataset.scaler_y.inverse_transform(dataset.y.numpy())

        print(f"\n{ticker}:")
        print(f"  Test set size: {len(predictions)} predictions")
        for i in range(min(10, len(predictions))):
            pred_val = pred_prices[i][0]
            actual_val = actual_prices[i][0]
            error = abs(pred_val - actual_val)
            error_pct = (error / actual_val) * 100
            print(
                f"  [{i}] Pred: ${pred_val:.2f}, Actual: ${actual_val:.2f}, Error: ${error:.2f} ({error_pct:.2f}%)"
            )

        # Calculate average error for this stock
        all_errors = abs(pred_prices - actual_prices)
        avg_error = all_errors.mean()
        avg_error_pct = (all_errors / actual_prices * 100).mean()
        print(f"  Average Error: ${avg_error:.2f} ({avg_error_pct:.2f}%)")

        overall_errors.append(avg_error_pct)

        # Plot
        ax = axes[idx]
        ax.plot(actual_prices, label="Actual", linewidth=2)
        ax.plot(pred_prices, label="Predicted", linewidth=2, alpha=0.7)
        ax.set_title(f"{ticker} - Test Error: {avg_error_pct:.2f}%")
        ax.set_xlabel("Time Step")
        ax.set_ylabel("Close Price ($)")
        ax.legend()
        ax.grid(True, alpha=0.3)

    # Overall statistics
    print(f"\n{'='*50}")
    print(f"OVERALL TEST PERFORMANCE")
    print(
        f"Average Error Across All Stocks: {sum(overall_errors)/len(overall_errors):.2f}%"
    )
    print(f"Best Stock: {min(overall_errors):.2f}%")
    print(f"Worst Stock: {max(overall_errors):.2f}%")
    print(
        f"Number of test predictions per stock: ~{len(test_datasets[list(test_datasets.keys())[0]].X)}"
    )
    print(f"{'='*50}")

    plt.tight_layout()
    plt.savefig("test_predictions.png", dpi=300, bbox_inches="tight")
    print("\n✅ Plot saved as 'test_predictions.png'")
    plt.show()
