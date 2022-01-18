#include "animals.h"
#include "test_runner.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

using Zoo = vector<Animal>;

// Ð­Ñ‚Ð° Ñ„ÑƒÐ½ÐºÑ†Ð¸Ñ Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÑ‚ Ð½Ð° Ð²Ñ…Ð¾Ð´ Ð¿Ð¾Ñ‚Ð¾Ðº Ð²Ð²Ð¾Ð´Ð° Ð¸ Ñ‡Ð¸Ñ‚Ð°ÐµÑ‚ Ð¸Ð· Ð½ÐµÐ³Ð¾ Ð¾Ð¿Ð¸ÑÐ°Ð½Ð¸Ðµ Ð·Ð²ÐµÑ€ÐµÐ¹.
// Ð•ÑÐ»Ð¸ Ð¾Ñ‡ÐµÑ€ÐµÐ´Ð½Ð¾Ðµ ÑÐ»Ð¾Ð²Ð¾ ÑÑ‚Ð¾Ð³Ð¾ Ñ‚ÐµÐºÑÑ‚Ð° - Tiger, Wolf Ð¸Ð»Ð¸ Fox, Ñ„ÑƒÐ½ÐºÑ†Ð¸Ñ Ð´Ð¾Ð»Ð¶Ð½Ð° Ð¿Ð¾Ð¼ÐµÑÑ‚Ð¸Ñ‚ÑŒ ÑÐ¾Ð¾Ñ‚Ð²ÐµÑ‚ÑÑ‚Ð²ÑƒÑŽÑ‰ÐµÐ³Ð¾ Ð·Ð²ÐµÑ€Ñ Ð² Ð·Ð¾Ð¾Ð¿Ð°Ñ€Ðº.
// Ð’ Ð¿Ñ€Ð¾Ñ‚Ð¸Ð²Ð½Ð¾Ð¼ ÑÐ»ÑƒÑ‡Ð°Ðµ Ð¾Ð½Ð° Ð´Ð¾Ð»Ð¶Ð½Ð° Ð¿Ñ€ÐµÐºÑ€Ð°Ñ‚Ð¸Ñ‚ÑŒ Ñ‡Ñ‚ÐµÐ½Ð¸Ðµ Ð¸ ÑÐ³ÐµÐ½ÐµÑ€Ð¸Ñ€Ð¾Ð²Ð°Ñ‚ÑŒ Ð¸ÑÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ðµ runtime_error.
Zoo CreateZoo(istream& in) {
  Zoo zoo;
  string word;
  while (in >> word) {
    if (word == "Tiger") {
      Tiger t;
      zoo.push_back(t);
    } else if (word == "Wolf") {
      Wolf w;
      zoo.push_back(w);
    } else if (word == "Fox") {
      Fox f;
      zoo.push_back(f);
    } else {
      throw runtime_error("Unknown animal!");
    }
  }
  return zoo;
}

// Ð­Ñ‚Ð° Ñ„ÑƒÐ½ÐºÑ†Ð¸Ñ Ð´Ð¾Ð»Ð¶Ð½Ð° Ð¿ÐµÑ€ÐµÐ±Ñ€Ð°Ñ‚ÑŒ Ð²ÑÐµÑ… Ð·Ð²ÐµÑ€ÐµÐ¹ Ð² Ð·Ð¾Ð¾Ð¿Ð°Ñ€ÐºÐµ Ð² Ð¿Ð¾Ñ€ÑÐ´ÐºÐµ Ð¸Ñ… ÑÐ¾Ð·Ð´Ð°Ð½Ð¸Ñ
// Ð¸ Ð·Ð°Ð¿Ð¸ÑÐ°Ñ‚ÑŒ Ð² Ð²Ñ‹Ñ…Ð¾Ð´Ð½Ð¾Ð¹ Ð¿Ð¾Ñ‚Ð¾Ðº Ð´Ð»Ñ ÐºÐ°Ð¶Ð´Ð¾Ð³Ð¾ Ð¸Ð· Ð½Ð¸Ñ… Ñ€ÐµÐ·ÑƒÐ»ÑŒÑ‚Ð°Ñ‚ Ñ€Ð°Ð±Ð¾Ñ‚Ñ‹ Ð²Ð¸Ñ€Ñ‚ÑƒÐ°Ð»ÑŒÐ½Ð¾Ð¹ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸ voice.
// Ð Ð°Ð·Ð´ÐµÐ»ÑÐ¹Ñ‚Ðµ Ð³Ð¾Ð»Ð¾ÑÐ° Ñ€Ð°Ð·Ð½Ñ‹Ñ… Ð·Ð²ÐµÑ€ÐµÐ¹ ÑÐ¸Ð¼Ð²Ð¾Ð»Ð¾Ð¼ Ð¿ÐµÑ€ÐµÐ²Ð¾Ð´Ð° ÑÑ‚Ñ€Ð¾ÐºÐ¸ '\n'.
void Process(const Zoo& zoo, ostream& out) {
  for (const auto& animal : zoo) {
    out << animal.Voice() << "\n";
  }
}

void TestZoo() {
  istringstream input("Tiger Wolf Fox Tiger");
  ostringstream output;
  Process(CreateZoo(input), output);

  const string expected =
    "Rrrr\n"
    "Wooo\n"
    "Tyaf\n"
    "Rrrr\n";

  ASSERT_EQUAL(output.str(), expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestZoo);
}
