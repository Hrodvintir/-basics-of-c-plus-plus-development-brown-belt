#include "animals.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using Zoo = std::vector<std::unique_ptr<Animal>>;

Zoo CreateZoo(std::istream& in) {
  using namespace std;
  Zoo zoo;
  string word;
  while (in >> word) {
    if (word == "Tiger") {
      zoo.push_back(make_unique<Tiger>());
    } else if (word == "Wolf") {
      zoo.push_back(make_unique<Wolf>());
    } else if (word == "Fox") {
      zoo.push_back(make_unique<Fox>());
    } else {
      throw runtime_error("Unknown animal!");
    }
  }
  return zoo;
}

void Process(const Zoo& zoo, std::ostream& out) {
  for (const auto& animal : zoo) {
    out << animal->Voice() << "\n";
  }
}
