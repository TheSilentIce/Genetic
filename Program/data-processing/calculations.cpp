#include "calculations.h"
#include "file_util.h"
#include <algorithm>
#include <string>

// constexpr i16 NUM_TICKETS = 500;
constexpr i16 INITIAL_DAYS = 14;
constexpr i16 MOVING_AVERAGE_PERIOD = 10;
constexpr float SMOOTHING_FACTOR = 2.0;
constexpr float SMOOTHING_OVER_PERIOD =
    SMOOTHING_FACTOR / (MOVING_AVERAGE_PERIOD + 1);

/**
 * This method creates a vector of Relative Strength Indices(RSI) for each day
 * We initialize the RSI as RSI is standardized as being a 2 week period
 *
 *
 */
std::vector<float> initialize_RSI(const std::vector<std::string> &stock_data) {
  std::vector<float> rsi_vector{};

  float avg_gain = 0;
  float avg_loss = 0;

  float before = split_line(stock_data.at(2)).at(CLOSE);
  float curr{};

  // initialize rsi
  for (i16 i = 0; i < INITIAL_DAYS; ++i) {
    std::vector<double> a = split_line(stock_data.at(i));
    curr = a.at(CLOSE);
    if (curr - before >= 0) {
      avg_gain += curr - before;
    } else {
      before += before - curr;
    }

    before = curr;
  }

  avg_gain /= 14;
  avg_loss /= 14;

  rsi_vector.push_back(100 - (100 / (1 + avg_gain / avg_loss)));

  for (i16 i = INITIAL_DAYS; i < stock_data.size(); ++i) {
    curr = split_line(stock_data.at(i)).at(CLOSE);
    float rsi = calculate_RSI(avg_gain, avg_loss, curr, before);

    rsi_vector.push_back(rsi);
    before = curr;
  }
  return rsi_vector;
}

// avg_gain = (prev_gain * 13 + curr_gain) / 14;
// avg_loss = (prev_loss * 13 + curr_loss) / 14;
//  RSI = 100 - 100 / (1 + RS)
//  RS = Average Up / Average Down
float calculate_RSI(float &avg_gain, float &avg_loss, float curr,
                    float before) {
  float gain = std::max(0.0f, curr - before);
  float loss = std::max(0.0f, before - curr);

  avg_gain = (avg_gain * 13 + gain) / 14;
  avg_loss = (avg_loss * 13 + loss) / 14;

  float RS = avg_gain / avg_loss;
  return 100 - (100 / (1 + RS));
}

/**
 * this method creates a vector for simple moving average
 */
std::vector<float> init_SMA(const std::vector<std::string> &stock_data) {
  std::vector<float> sma_vector{};
  for (i16 i = 9; i < stock_data.size(); ++i) {
    float sma = calculate_SMA(i - 9, i, stock_data);
    sma_vector.push_back(sma);
  }
  return sma_vector;
}

/**
 *  simple moving average is generally defined as the average price
 *  over a 10 day period. However, this can be changed as you want.
 */
float calculate_SMA(i16 beg, i16 end,
                    const std::vector<std::string> &stock_data) {
  float sum = 0;
  for (i16 i = beg; i <= end; ++i) {
    auto line = split_line(stock_data.at(i));
    sum += line.at(CLOSE);
  }
  return sum / MOVING_AVERAGE_PERIOD;
}

/*
 * This method generates a vector of exponential moving average(EMA)
 * The difference between EMA and SMA is that EMA is that EMA gives a higher
 * weightage to more recent prices.
 * The idea is that stock prices are more influenced by their most recent
 * prices, so we should give more importance to said recent prices
 */
std::vector<float> init_EMA(const std::vector<std::string> &stock_data) {
  float ema = calculate_SMA(0, 9, stock_data);
  std::vector<float> ema_vector{};
  ema_vector.push_back(ema);

  for (i16 i = 10; i < stock_data.size(); ++i) {
    float price = split_line(stock_data.at(i)).at(CLOSE);
    ema = calculate_EMA(ema, price);
    ema_vector.push_back(ema);
  }
  return ema_vector;
}

/**
 * Information taken from Investpedia's article on EMAs
 * The initial EMA is defined as just the simple moving average of first 10 days
 * The formula is as follows:
 * EMA_today = (Value_today * (Smoothing factor / (1 + DAYS))) +
 * EMA_yesterday * (1 - (Smoothing factor / (1 + DAYS)))
 *
 * Smoothing factor: This is the weighting factor if you will
 * Set to 2 per Investpedia's article, however can be changed as you will
 * Higher Smoothing factor means more weightage towards recent prices
 *
 * As for the period:
 * 50-200 days are preferred by long-term investors
 * 8-20 days are preferred by short-term investors
 * For now, we set this to mirror SMA, which for now is 10
 * Can be adjusted
 */
float calculate_EMA(float ema, float price) {
  float a = price * SMOOTHING_OVER_PERIOD;
  float b = ema * (1 - SMOOTHING_OVER_PERIOD);
  return a + b;
}

/*
 * This method creates a vector of stochastic oscillations
 * Stochastic oscillation is a momentum indicator, like RSI
 * The difference between the two is that stochastic oscillation is based on the
 * idea that the closing prices move in the same direction as the current trend
 * RSI measures velocity of price movements , stochastic oscillation does that
 * within a range and with the assumption above.
 */
std::vector<float> init_SO(const std::vector<std::string> &stock_data) {
  std::vector<float> so_vector{};

  for (i16 i = 13; i < stock_data.size(); ++i) {
    float so = calculate_SO(stock_data, i - 13);
    so_vector.push_back(so);
  }
  return so_vector;
}

/**
 *Stochastic oscillation actually has two "lines":
 *%K, which is what this method implements, also known as fast stochastic
 * indicator %D is the slow stochastic indicator The difference is that fast is
 * more sensitive
 * It's good to have both
 *
 * For the range, it's typically 2 weeks, which is what we hard-coded here
 * Can of course be changed
 *
 * HP - highest price over range
 * LP - lowest price over range
 * C - Closing Price
 * %K - Fast indicator
 *
 * %K = 100 * (C - LP) / (HP - LP)
 */
float calculate_SO(const std::vector<std::string> &stock_data, i16 beginning) {
  double lowest{0};
  double highest = std::numeric_limits<double>::lowest();

  for (i16 i = beginning; i < beginning + 14; ++i) {
    auto line = split_line(stock_data.at(i));
    lowest = std::min(lowest, line.at(LOW));
    highest = std::max(highest, line.at(HIGH));
  }

  double curr_close = split_line(stock_data.at(beginning + 13)).at(CLOSE);

  if (highest == lowest)
    return 0.0f;

  return static_cast<float>((curr_close - lowest) / (highest - lowest) * 100.0);
}
