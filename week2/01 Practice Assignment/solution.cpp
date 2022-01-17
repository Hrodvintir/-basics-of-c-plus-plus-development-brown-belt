#include "xml.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Spending {
  string category;
  int amount;
};

vector<Spending> LoadFromXml(istream& input) {
  Document doc = Load(input);
  vector<Spending> result;
  for (const Node& node : doc.GetRoot().Children()) {
    result.push_back({
      node.AttributeValue<string>("category"),
      node.AttributeValue<int>("amount")
    });
  }
  return result;
}
