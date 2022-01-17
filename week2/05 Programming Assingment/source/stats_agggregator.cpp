#include "stats_aggregator.h"

using namespace std;

template <typename T>
ostream& operator << (ostream& os, const optional<T>& v) {
  if (v) {
    os << *v;
  } else {
    os << "undefined";
  }
  return os;
}

void CompositeStatsAggregator::Process(int value) {
  for (auto& aggr : aggregators) {
    aggr->Process(value);
  }
}

void CompositeStatsAggregator::PrintValue(std::ostream& output) const {
  for (const auto& aggr : aggregators) {
    aggr->PrintValue(output);
    output << '\n';
  }
}

void CompositeStatsAggregator::Add(std::unique_ptr<StatsAggregator> aggr) {
  aggregators.push_back(std::move(aggr));
}

void SumStatsAggregator::Process(int value) {
  sum += value;
}

void SumStatsAggregator::PrintValue(std::ostream& out) const {
  out << "Sum is " << sum;
}

void MinStatsAggregator::Process(int value) {
  if (!current_min || value < *current_min) {
    current_min = value;
  }
}

void MinStatsAggregator::PrintValue(std::ostream& out) const {
  out << "Min is " << current_min;
}

void MaxStatsAggregator::Process(int value) {
  if (!current_max || value > *current_max) {
    current_max = value;
  }
}

void MaxStatsAggregator::PrintValue(std::ostream& out) const {
  out << "Max is " << current_max;
}

void AverageStatsAggregator::Process(int value) {
  sum += value;
  ++total;
}

void AverageStatsAggregator::PrintValue(std::ostream& out) const {
  out << "Average is ";
  if (total == 0) {
    out << "undefined";
  } else {
    out << sum / total;
  }
}

void ModeStatsAggregator::Process(int value) {
  int current_count = ++count[value];
  if (!mode || current_count > count[*mode]) {
    mode = value;
  }
}

void ModeStatsAggregator::PrintValue(std::ostream& out) const {
  out << "Mode is " << mode;
}
