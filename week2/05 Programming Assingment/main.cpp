#include "test_runner.h"
#include "stats_aggregator.h"

#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>
using namespace std;

void TestAll();

unique_ptr<StatsAggregator> ReadAggregators(istream& input) {
  const unordered_map<string, std::function<unique_ptr<StatsAggregator>()>> known_builders = {
    {"sum", [] { return make_unique<StatsAggregators::Sum>(); }},
    {"min", [] { return make_unique<StatsAggregators::Min>(); }},
    {"max", [] { return make_unique<StatsAggregators::Max>(); }},
    {"avg", [] { return make_unique<StatsAggregators::Average>(); }},
    {"mode", [] { return make_unique<StatsAggregators::Mode>(); }}
  };

  auto result = make_unique<StatsAggregators::Composite>();

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
  RUN_TEST(tr, StatsAggregators::TestSum);
  RUN_TEST(tr, StatsAggregators::TestMin);
  RUN_TEST(tr, StatsAggregators::TestMax);
  RUN_TEST(tr, StatsAggregators::TestAverage);
  RUN_TEST(tr, StatsAggregators::TestMode);
  RUN_TEST(tr, StatsAggregators::TestComposite);
}
