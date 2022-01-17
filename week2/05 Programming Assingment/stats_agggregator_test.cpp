#include "stats_aggregator.h"
#include "test_runner.h"

#include <sstream>
using namespace std;

string PrintedValue(const StatsAggregator& aggr) {
  ostringstream output;
  aggr.PrintValue(output);
  return output.str();
}

void TestSumStatsAggregator() {
  SumStatsAggregator aggr;
  ASSERT_EQUAL(PrintedValue(aggr), "Sum is 0");

  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(16);

  ASSERT_EQUAL(PrintedValue(aggr), "Sum is 26");
}

void TestMinStatsAggregator() {
  MinStatsAggregator aggr;
  ASSERT_EQUAL(PrintedValue(aggr), "Min is undefined");

  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(16);

  ASSERT_EQUAL(PrintedValue(aggr), "Min is -1");
}

void TestMaxStatsAggregator() {
  MaxStatsAggregator aggr;
  ASSERT_EQUAL(PrintedValue(aggr), "Max is undefined");

  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(16);

  ASSERT_EQUAL(PrintedValue(aggr), "Max is 16");
}

void TestAverageStatsAggregator() {
  AverageStatsAggregator aggr;
  ASSERT_EQUAL(PrintedValue(aggr), "Average is undefined");

  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(16);

  ASSERT_EQUAL(PrintedValue(aggr), "Average is 6");
}

void TestModeStatsAggregator() {
  ModeStatsAggregator aggr;
  ASSERT_EQUAL(PrintedValue(aggr), "Mode is undefined");

  aggr.Process(3);
  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(8);
  aggr.Process(8);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(-1);
  aggr.Process(-1);
  aggr.Process(16);

  ASSERT_EQUAL(PrintedValue(aggr), "Mode is 8");
}

void TestCompositeStatsAggregator() {
  CompositeStatsAggregator aggr;
  aggr.Add(make_unique<SumStatsAggregator>());
  aggr.Add(make_unique<MinStatsAggregator>());
  aggr.Add(make_unique<MaxStatsAggregator>());
  aggr.Add(make_unique<AverageStatsAggregator>());
  aggr.Add(make_unique<ModeStatsAggregator>());

  aggr.Process(3);
  aggr.Process(8);
  aggr.Process(-1);
  aggr.Process(16);
  aggr.Process(16);

  string expected = "Sum is 42\n";
  expected += "Min is -1\n";
  expected += "Max is 16\n";
  expected += "Average is 8\n";
  expected += "Mode is 16\n";
  ASSERT_EQUAL(PrintedValue(aggr), expected);
}
