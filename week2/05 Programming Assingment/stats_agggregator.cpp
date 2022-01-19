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

void StatsAggregators::Composite::Process(int value) {
  for (auto& aggr : aggregators) {
    aggr->Process(value);
  }
}

void StatsAggregators::Composite::PrintValue(std::ostream& output) const {
  for (const auto& aggr : aggregators) {
    aggr->PrintValue(output);
    output << '\n';
  }
}

void StatsAggregators::Composite::Add(std::unique_ptr<StatsAggregator> aggr) {
  aggregators.push_back(std::move(aggr));
}

void StatsAggregators::Sum::Process(int value) {
  sum += value;
}

void StatsAggregators::Sum::PrintValue(std::ostream& out) const {
  out << "Sum is " << sum;
}

void StatsAggregators::Min::Process(int value) {
  if (!current_min || value < *current_min) {
    current_min = value;
  }
}

void StatsAggregators::Min::PrintValue(std::ostream& out) const {
  out << "Min is " << current_min;
}

void StatsAggregators::Max::Process(int value) {
  if (!current_max || value > *current_max) {
    current_max = value;
  }
}

void StatsAggregators::Max::PrintValue(std::ostream& out) const {
  out << "Max is " << current_max;
}

void StatsAggregators::Average::Process(int value) {
  sum += value;
  ++total;
}

void StatsAggregators::Average::PrintValue(std::ostream& out) const {
  out << "Average is ";
  if (total == 0) {
    out << "undefined";
  } else {
    out << sum / total;
  }
}

void StatsAggregators::Mode::Process(int value) {
  int current_count = ++count[value];
  if (!mode || current_count > count[*mode]) {
    mode = value;
  }
}

void StatsAggregators::Mode::PrintValue(std::ostream& out) const {
  out << "Mode is " << mode;
}
