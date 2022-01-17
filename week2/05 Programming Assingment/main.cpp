#include "test_runner.h"
#include "stats_aggregator.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>
using namespace std;

void TestAll();

unique_ptr<StatsAggregator> ReadAggregators(istream& input) {
  const unordered_map<string, std::function<unique_ptr<StatsAggregator>()>> known_builders = {
    {"sum", [] { return make_unique<SumStatsAggregator>(); }},
    {"min", [] { return make_unique<MinStatsAggregator>(); }},
    {"max", [] { return make_unique<MaxStatsAggregator>(); }},
    {"avg", [] { return make_unique<AverageStatsAggregator>(); }},
    {"mode", [] { return make_unique<ModeStatsAggregator>(); }}
  };

  auto result = make_unique<CompositeStatsAggregator>();

  int aggr_count;
  input >> aggr_count;

  string line;
  for (int i = 0; i < aggr_count; ++i) {
    input >> line;
    result->Add(known_builders.at(line)());
  }

  return result;
}

int main() {
  TestAll();

  auto stats_aggregator = ReadAggregators(cin);

  for (int value; cin >> value; ) {
    stats_aggregator->Process(value);
  }
  stats_aggregator->PrintValue(cout);

  return 0;
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestSumStatsAggregator);
  RUN_TEST(tr, TestMinStatsAggregator);
  RUN_TEST(tr, TestMaxStatsAggregator);
  RUN_TEST(tr, TestAverageStatsAggregator);
  RUN_TEST(tr, TestModeStatsAggregator);
  RUN_TEST(tr, TestCompositeStatsAggregator);
}
