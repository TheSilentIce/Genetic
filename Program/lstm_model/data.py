import os.path

import pandas as pd
import yfinance as yf

tickers = [
    # ── Tech ──────────────────────────────────────────────────────────────────
    "AAPL",
    "MSFT",
    "GOOG",
    "GOOGL",
    "META",
    "AMZN",
    "TSLA",
    "NVDA",
    "AMD",
    "INTC",
    "QCOM",
    "TXN",
    "AVGO",
    "MU",
    "AMAT",
    "LRCX",
    "KLAC",
    "SNPS",
    "CDNS",
    "ADBE",
    "CRM",
    "NOW",
    "SNOW",
    "PLTR",
    "UBER",
    "SHOP",
    "PYPL",
    "COIN",
    "NET",
    "DDOG",
    "ZS",
    "CRWD",
    "PANW",
    "FTNT",
    "OKTA",
    "MDB",
    "TEAM",
    "ZM",
    "DOCU",
    "TWLO",
    "U",
    "RBLX",
    # ── AI ────────────────────────────────────────────────────────────────────
    "SOUN",
    "BBAI",
    "UPST",
    "AI",
    "PATH",
    "GTLB",
    "ASAN",
    "S",
    "VRNS",
    "DT",
    # ── Finance / Banking ─────────────────────────────────────────────────────
    "JPM",
    "BAC",
    "WFC",
    "GS",
    "MS",
    "C",
    "BK",
    "AXP",
    "BLK",
    "SCHW",
    "USB",
    "PNC",
    "TFC",
    "COF",
    "ALLY",
    "SOFI",
    "LC",
    "HOOD",
    "ICE",
    "CME",
    "SPGI",
    "MCO",
    "V",
    "MA",
    # ── Defense ───────────────────────────────────────────────────────────────
    "LMT",
    "RTX",
    "NOC",
    "GD",
    "BA",
    "HII",
    "LHX",
    "TDG",
    "KTOS",
    "LDOS",
    "CACI",
    "SAIC",
    "DRS",
    "BWXT",
]

crypto = [
    "BTC-USD",
    "ETH-USD",
    "BNB-USD",
    "XRP-USD",
    "SOL-USD",
    "TRX-USD",
    "DOGE-USD",
    "ADA-USD",
    "DOT-USD",
    "BCH-USD",
    "LINK-USD",
    "XLM-USD",
    "AVAX-USD",
    "LTC-USD",
]

TRAIN_RATIO = 0.8
# ───────────────────────────────────────────────────────────────────────────

df = yf.download(tickers, period="5y", group_by="ticker")
raw = yf.download(tickers, period="2mo", group_by="ticker")


def build_rows(tickers, df, date_slice):
    rows = []
    for ticker in tickers:
        if isinstance(df.columns, pd.MultiIndex):
            sub = df[ticker].loc[date_slice]
        else:
            sub = df.copy().loc[date_slice]

        rows.append([ticker])
        rows.append(["Date"] + list(sub.columns))
        for date, r in sub.iterrows():
            rows.append([date.strftime("%Y-%m-%d")] + r.values.tolist())
        rows.append([])
    return rows


# Determine the split index on the sorted unique dates
all_dates = df.index.sort_values()
split_idx = int(len(all_dates) * TRAIN_RATIO)
train_dates = all_dates[:split_idx]
test_dates = all_dates[split_idx:]

train_rows = build_rows(tickers, df, train_dates)
test_rows = build_rows(tickers, df, test_dates)

all_dates = raw.index.sort_values()
raw_rows = build_rows(tickers, raw, all_dates)
pd.DataFrame(raw_rows).to_csv("../data/om_nom.csv", index=False, header=False)

os.makedirs("../data", exist_ok=True)

pd.DataFrame(train_rows).to_csv("../data/training.csv", index=False, header=False)
pd.DataFrame(test_rows).to_csv("../data/testing.csv", index=False, header=False)

print(f"Total dates : {len(all_dates)}")
print(f"Train dates : {len(train_dates)}  → ../data/training.csv")
print(f"Test  dates : {len(test_dates)}   → ../data/testing.csv")
print(f"Split ratio : {TRAIN_RATIO:.0%} train / {1 - TRAIN_RATIO:.0%} test")
