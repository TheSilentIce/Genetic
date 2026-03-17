from torch.utils.data import Dataset
import pandas as pd
import torch

from sklearn.preprocessing import MinMaxScaler
from constants import SEQUENCE, feature_cols, gpu


class StockDataSet(Dataset):
    def __init__(
        self,
        ticker_data: pd.DataFrame,
        seq_length: int = SEQUENCE,
        scaler_X=None,
        scaler_y=None,
        fit_scalers: bool = True,
    ):
        self.ticker = ticker_data["Ticket"].iloc[0]
        self.seq_length = seq_length

        X_data = ticker_data[feature_cols].values
        y_data = ticker_data["Close"].values.reshape(-1, 1)

        if fit_scalers:
            self.scaler_X = MinMaxScaler()
            self.scaler_y = MinMaxScaler()
            X_scaled = self.scaler_X.fit_transform(X_data)
            y_scaled = self.scaler_y.fit_transform(y_data)
        else:
            if scaler_X is None or scaler_y is None:
                raise ValueError("Must provide fitted scalers for test set.")
            self.scaler_X = scaler_X
            self.scaler_y = scaler_y
            X_scaled = self.scaler_X.transform(X_data)
            y_scaled = self.scaler_y.transform(y_data)

        X_seq, y_seq = [], []
        for i in range(len(X_scaled) - seq_length):
            X_seq.append(X_scaled[i : i + seq_length])
            y_seq.append(y_scaled[i + seq_length])

        self.X = torch.tensor(X_seq, dtype=torch.float32, device=gpu)
        self.y = torch.tensor(y_seq, dtype=torch.float32, device=gpu)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.y[idx]


def build_datasets(train_df: pd.DataFrame, test_df: pd.DataFrame):
    train_datasets, test_datasets = {}, {}
    common = set(train_df["Ticket"].unique()) & set(test_df["Ticket"].unique())

    for ticker in common:
        tr = train_df[train_df["Ticket"] == ticker].reset_index(drop=True)
        te = test_df[test_df["Ticket"] == ticker].reset_index(drop=True)

        if len(tr) > SEQUENCE and len(te) > SEQUENCE:
            train_ds = StockDataSet(tr, fit_scalers=True)
            test_ds = StockDataSet(
                te,
                scaler_X=train_ds.scaler_X,
                scaler_y=train_ds.scaler_y,
                fit_scalers=False,
            )
            train_datasets[ticker] = train_ds
            test_datasets[ticker] = test_ds

    return train_datasets, test_datasets


def make_walk_forward_splits(df: pd.DataFrame, n_splits: int) -> list[tuple]:
    tickers = df["Ticket"].unique()
    n_rows = len(df[df["Ticket"] == tickers[0]])
    test_size = n_rows // (n_splits + 1)
    min_train = test_size
    splits = []

    for fold in range(n_splits):
        train_end = min_train + fold * test_size
        test_end = train_end + test_size
        if test_end > n_rows:
            break

        train_parts, test_parts = [], []
        for ticker in tickers:
            group = df[df["Ticket"] == ticker].reset_index(drop=True)
            train_parts.append(group.iloc[:train_end])
            test_parts.append(group.iloc[train_end:test_end])

        splits.append(
            (
                pd.concat(train_parts).reset_index(drop=True),
                pd.concat(test_parts).reset_index(drop=True),
            )
        )

    return splits
