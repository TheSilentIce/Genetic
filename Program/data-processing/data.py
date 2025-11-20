import yfinance as yf

tickers = [
    # Tech
    "AAPL",
    "MSFT",
    "GOOGL",
    "GOOG",
    "AMZN",
    "META",
    "NVDA",
    "TSLA",
    "ADBE",
    "INTC",
    "CRM",
    # Finance
    "JPM",
    "BAC",
    "C",
    "WFC",
    "GS",
    "MS",
    "BLK",
    "V",
    "MA",
    "PYPL",
]

data = yf.download(tickers, period="2y")
data.to_csv("data.csv")
