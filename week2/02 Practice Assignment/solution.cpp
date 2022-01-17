#include "json.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Spending {
  string category;
  int amount;
};

vector<Spending> LoadFromJson(istream& input) {
  Document doc = Load(input);
  vector<Spending> result;
  for (const Node& node : doc.GetRoot().AsArray()) {
    result.push_back({
      node.AsMap().at("category").AsString(),
      node.AsMap().at("amount").AsInt()
    });
  }
  return result;
}
