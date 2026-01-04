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
 * Format:
 * Date, Open, High, Low, Close, Volume
 * Line:
 2025-11-28,277.260009765625,279.0,275.989990234375,278.8500061035156,20135600.0

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

std::vector<float> init_SMA(const std::vector<std::string> &stock_data) {
  std::vector<float> sma_vector{};
  for (i16 i = 9; i < stock_data.size(); ++i) {
    float sma = calculate_SMA(i - 9, i, stock_data);
    sma_vector.push_back(sma);
  }
  return sma_vector;
}

float calculate_SMA(i16 beg, i16 end,
                    const std::vector<std::string> &stock_data) {
  float sum = 0;
  for (i16 i = beg; i <= end; ++i) {
    auto line = split_line(stock_data.at(i));
    sum += line.at(CLOSE);
  }
  return sum / MOVING_AVERAGE_PERIOD;
}

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

float calculate_EMA(float ema, float price) {
  float a = price * SMOOTHING_OVER_PERIOD;
  float b = ema * (1 - SMOOTHING_OVER_PERIOD);
  return a + b;
}
