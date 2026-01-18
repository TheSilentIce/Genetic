import os.path

import pandas as pd
import yfinance as yf

tickers = [
    "AAPL",
    "MSFT",
    "AMZN",
    "NVDA",
    "GOOG",
    "META",
    "TSLA",
    "AMD",
    "GS",
    "JPM",
    "BK",
]


# convert into format:
# Name
# Open,High,Close,...
# Data,Data,Data,...
# Intentional space

df = yf.download(tickers, period="5y", group_by="ticker")
rows = []

for ticker in tickers:
    if isinstance(df.columns, pd.MultiIndex):
        sub = df[ticker]
    else:
        sub = df.copy()

    rows.append([ticker])

    rows.append(["Date", "days"] + list(sub.columns))
    start_date = sub.index[0]
    for date, r in sub.iterrows():
        days_elapsed = (date - start_date).days
        rows.append([date.strftime("%Y-%m-%d"), days_elapsed] + r.values.tolist())

    rows.append([])

out = pd.DataFrame(rows)
out.to_csv("../data/data.csv", index=False, header=False)
