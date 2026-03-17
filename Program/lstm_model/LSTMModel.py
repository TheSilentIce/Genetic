import torch
import numpy as np
from torch import nn
import constants
from torch.utils.data import DataLoader, ConcatDataset


class LSTMModel(nn.Module):
    def __init__(
        self, input_dim: int, hidden_dim: int, layer_dim: int, output_dim: int
    ):
        super().__init__()
        self.lstm = nn.LSTM(input_dim, hidden_dim, layer_dim, batch_first=True)
        self.fc = nn.Linear(hidden_dim, output_dim)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out, _ = self.lstm(x)
        return self.fc(out[:, -1, :])


def combined_loss(
    pred: torch.Tensor, actual: torch.Tensor, alpha: float = constants.ALPHA
) -> torch.Tensor:
    mse = nn.MSELoss()(pred, actual)
    sign_penalty = torch.mean(torch.relu(-pred * actual))
    return (1 - alpha) * mse + alpha * sign_penalty


def train_model(train_datasets: dict) -> LSTMModel:
    model = LSTMModel(
        len(constants.feature_cols), constants.HIDDEN_DIM, layer_dim=2, output_dim=1
    ).to(constants.gpu)
    optimizer = torch.optim.Adam(model.parameters(), lr=constants.LEARNING_RATE)
    loader = DataLoader(
        ConcatDataset(list(train_datasets.values())),
        batch_size=constants.BATCH_SIZE,
        shuffle=True,
    )

    model.train()
    for epoch in range(constants.NUM_EPOCHS):
        epoch_loss = 0.0
        for X_batch, y_batch in loader:
            optimizer.zero_grad()
            loss = combined_loss(
                model(X_batch.to(constants.gpu)), y_batch.to(constants.gpu)
            )
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item()
        if (epoch + 1) % 10 == 0:
            print(
                f"    Epoch [{epoch+1}/{constants.NUM_EPOCHS}]  Loss: {epoch_loss/len(loader):.4f}"
            )

    return model


def compute_stats(pred: np.ndarray, actual: np.ndarray) -> dict:
    errors = pred - actual
    abs_errors = np.abs(errors)
    pct_errors = np.where(actual != 0, abs_errors / np.abs(actual) * 100, np.nan)

    return {
        "dir_acc": (np.sign(pred) == np.sign(actual)).mean() * 100,
        "mag_acc": (abs_errors <= 0.005).mean() * 100,
        "mae": abs_errors.mean() * 100,  # mean absolute error (pp)
        "mad": np.median(abs_errors) * 100,  # median absolute deviation (pp)
        "rmse": np.sqrt((errors**2).mean()) * 100,  # root mean squared error (pp)
        "mape": np.nanmean(pct_errors),  # mean absolute % error
        "std_err": abs_errors.std() * 100,  # std of absolute errors (pp)
        "max_err": abs_errors.max() * 100,  # worst single prediction (pp)
        "bias": errors.mean() * 100,  # +ve = over-predicts, -ve = under
    }


def evaluate_model(model: LSTMModel, test_datasets: dict) -> tuple[dict, dict]:
    model.eval()
    per_ticker = {}
    all_pred, all_actual = [], []

    with torch.no_grad():
        for ticker, dataset in test_datasets.items():
            pred_scaled = model(dataset.X)
            pred_pct = dataset.scaler_y.inverse_transform(
                pred_scaled.cpu().numpy()
            ).flatten()
            actual_pct = dataset.scaler_y.inverse_transform(
                dataset.y.cpu().numpy()
            ).flatten()
            per_ticker[ticker] = compute_stats(pred_pct, actual_pct)
            all_pred.append(pred_pct)
            all_actual.append(actual_pct)

    fold_stats = compute_stats(np.concatenate(all_pred), np.concatenate(all_actual))
    return per_ticker, fold_stats
