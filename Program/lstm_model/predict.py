import argparse
import numpy as np
import pandas as pd
import torch
from sklearn.preprocessing import MinMaxScaler

import LSTMModel
import constants

# ── CLI args ──────────────────────────────────────────────────────────────────
parser = argparse.ArgumentParser()
parser.add_argument("--csv", default="../data/new_omnom.csv")
parser.add_argument("--model", default="lstm_best.pth")
parser.add_argument("--days", type=int, default=30)
parser.add_argument("--ticker", default=None)
args = parser.parse_args()

checkpoint = torch.load(args.model, map_location=constants.gpu, weights_only=False)

model = LSTMModel.LSTMModel(
    input_dim=checkpoint["input_dim"],
    hidden_dim=checkpoint["hidden_dim"],
    layer_dim=checkpoint["layer_dim"],
    output_dim=checkpoint["output_dim"],
).to(constants.gpu)

model.load_state_dict(checkpoint["model_state_dict"])
model.eval()

print(
    f"Loaded model from fold {checkpoint['fold']}  "
    f"(dir_acc={checkpoint['dir_acc']:.1f}%)"
)

df = pd.read_csv(args.csv)
tickers = df["Ticket"].unique().tolist()
if args.ticker:
    tickers = [t for t in tickers if t == args.ticker]
    if not tickers:
        raise ValueError(f"Ticker '{args.ticker}' not found in {args.csv}")

SEQ = constants.SEQUENCE

all_forecasts = {}

for ticker in tickers:
    ticker_df = df[df["Ticket"] == ticker].reset_index(drop=True)

    if len(ticker_df) < SEQ:
        print(f"  {ticker}: not enough rows ({len(ticker_df)} < {SEQ}), skipping.")
        continue

    scaler_X = MinMaxScaler()
    scaler_y = MinMaxScaler()

    X_scaled = scaler_X.fit_transform(ticker_df[constants.feature_cols].values)
    scaler_y.fit_transform(ticker_df["Close"].values.reshape(-1, 1))

    window = X_scaled[-SEQ:].copy()

    pct_changes = []

    with torch.no_grad():
        for _ in range(args.days):
            x_tensor = torch.tensor(
                window[np.newaxis, :, :],
                dtype=torch.float32,
                device=constants.gpu,
            )
            pred_scaled = model(x_tensor).cpu().numpy()
            pred_pct = scaler_y.inverse_transform(pred_scaled).flatten()[0]
            pct_changes.append(pred_pct)

            window = np.roll(window, -1, axis=0)

    forecast_df = pd.DataFrame(
        {
            "day": range(1, args.days + 1),
            "pct_change": np.round(pct_changes, 6),
        }
    )

    all_forecasts[ticker] = forecast_df

    print(f"\n{'─'*40}")
    print(f"  {ticker}")
    print(forecast_df.to_string(index=False))

if all_forecasts:
    combined = pd.concat(
        [d.assign(ticker=t) for t, d in all_forecasts.items()],
        ignore_index=True,
    )[["ticker", "day", "pct_change"]]
    combined.to_csv("../data/forecasts.csv", index=False)
