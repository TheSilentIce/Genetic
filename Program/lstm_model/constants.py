import torch

__all__ = [
    "feature_cols",
    "SEQUENCE",
    "HIDDEN_DIM",
    "LEARNING_RATE",
    "NUM_EPOCHS",
    "BATCH_SIZE",
    "ALPHA",
    "N_SPLITS",
    "gpu",
]

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
    "CCI",
]

SEQUENCE = 30
HIDDEN_DIM = 256
LEARNING_RATE = 0.001
NUM_EPOCHS = 120
BATCH_SIZE = 64
ALPHA = 0.5
N_SPLITS = 5  # number of walk-forward folds
gpu = torch.device("mps" if torch.backends.mps.is_available() else "cpu")
