#pragma once

#include <ostream>
#include <limits>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>

struct StatsAggregator {
  virtual ~StatsAggregator() {
  }

  virtual void Process(int value) = 0;
  virtual void PrintValue(std::ostream& out) const = 0;
};

class SumStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& out) const override;

private:
  int sum = 0;
};

class MinStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& out) const override;

private:
  // Ð Ð°Ð½ÐµÐµ Ð¼Ñ‹ Ð½Ðµ Ñ€Ð°ÑÑÐ¼Ð°Ñ‚Ñ€Ð¸Ð²Ð°Ð»Ð¸ ÑˆÐ°Ð±Ð»Ð¾Ð½ std::optional. Ðž Ð½Ñ‘Ð¼ Ð¼Ð¾Ð¶Ð½Ð¾ Ð¿Ð¾Ñ‡Ð¸Ñ‚Ð°Ñ‚ÑŒ Ð² Ð´Ð¾ÐºÑƒÐ¼ÐµÐ½Ñ‚Ð°Ñ†Ð¸Ð¸
  // https://en.cppreference.com/w/cpp/utility/optional. ÐšÑ€Ð¾Ð¼Ðµ Ñ‚Ð¾Ð³Ð¾, ÐµÐ¼Ñƒ Ð±ÑƒÐ´ÐµÑ‚ ÑƒÐ´ÐµÐ»ÐµÐ½Ð¾ Ð²Ð½Ð¸Ð¼Ð°Ð½Ð¸Ðµ
  // Ð² Ñ€Ð°Ð·Ð´ÐµÐ»Ðµ Ð¿Ñ€Ð¾ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸
  std::optional<int> current_min;
};

class MaxStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& out) const override;

private:
  std::optional<int> current_max;
};

class AverageStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& out) const override;

private:
  int sum = 0;
  int total = 0;
};

class ModeStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& out) const override;

private:
  std::unordered_map<int, int> count;
  std::optional<int> mode;
};

class CompositeStatsAggregator : public StatsAggregator {
public:
  void Process(int value) override;
  void PrintValue(std::ostream& output) const override;

  void Add(std::unique_ptr<StatsAggregator> aggr);

private:
  std::vector<std::unique_ptr<StatsAggregator>> aggregators;
};

void TestSumStatsAggregator();
void TestMinStatsAggregator();
void TestMaxStatsAggregator();
void TestAverageStatsAggregator();
void TestModeStatsAggregator();
void TestCompositeStatsAggregator();
