#include "calculations.h"
#include "file_util.h"
#include <algorithm>
#include <iostream>
#include <string>

// constexpr short NUM_TICKETS = 500;
constexpr short INITIAL_DAYS = 14;

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

  float before = split_line(stock_data.at(2)).at(3);
  float curr{};

  // initialize rsi
  for (short i = 3; i < INITIAL_DAYS + 3; ++i) {
    std::vector<double> a = split_line(stock_data.at(i));
    curr = a.at(3);
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

  for (short i = INITIAL_DAYS + 3; i < stock_data.size(); ++i) {
    curr = split_line(stock_data.at(i)).at(3);
    float rsi = calculate_RSI(avg_gain, avg_loss, curr, before);
    // printf("Day: %d --- RSI: %f\n", i, rsi);

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
