#include "test_runner.h"

#include "ini.h"

#include <sstream>

using namespace std;

void TestLoadIni() {
  istringstream input(
R"([july]
food=2500
sport=12000
travel=23400
clothes=5200

[august]
food=3250
sport=10000
travel=0
clothes=8300
jewelery=25000
)"
  );

  const Ini::Document doc = Ini::Load(input);

  ASSERT_EQUAL(doc.SectionCount(), 2u);

  const Ini::Section expected_july = {
    {"food", "2500"},
    {"sport", "12000"},
    {"travel", "23400"},
    {"clothes", "5200"},
  };

  const Ini::Section expected_august = {
    {"food", "3250"},
    {"sport", "10000"},
    {"travel", "0"},
    {"clothes", "8300"},
    {"jewelery", "25000"},
  };

  ASSERT_EQUAL(doc.GetSection("july"), expected_july);
  ASSERT_EQUAL(doc.GetSection("august"), expected_august);
}

void TestDocument() {
  Ini::Document doc;
  ASSERT_EQUAL(doc.SectionCount(), 0u);

  // ÐžÐ±Ñ€Ð°Ñ‚Ð¸Ñ‚Ðµ Ð²Ð½Ð¸Ð¼Ð°Ð½Ð¸Ðµ, ÐºÐ°Ðº Ð¼Ñ‹ Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼ ÑƒÐºÐ°Ð·Ð°Ñ‚ÐµÐ»ÑŒ Ð´Ð»Ñ Ñ€Ð°Ð±Ð¾Ñ‚Ñ‹
  // Ñ Ð¿Ð¾ÑÐ»ÐµÐ´Ð½ÐµÐ¹ Ð´Ð¾Ð±Ð°Ð²Ð»ÐµÐ½Ð½Ð¾Ð¹ ÑÐµÐºÑ†Ð¸ÐµÐ¹. Ð­Ñ‚Ð° Ñ‚ÐµÑ…Ð½Ð¸ÐºÐ° Ð¼Ð¾Ð¶ÐµÑ‚ Ð²Ð°Ð¼ Ð¿Ñ€Ð¸Ð³Ð¾Ð´Ð¸Ñ‚ÑŒÑÑ
  // Ð´Ð»Ñ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸ Load
  Ini::Section* section = &doc.AddSection("one");
  ASSERT_EQUAL(doc.SectionCount(), 1u);

  section->insert({"name_1", "value_1"});
  section->insert({"name_2", "value_2"});

  section = &doc.AddSection("two");
  section->insert({"name_1", "value_1"});
  section->insert({"name_2", "value_2"});
  section->insert({"name_3", "value_3"});

  section = &doc.AddSection("three");
  section->insert({"name_1", "value_1"});

  ASSERT_EQUAL(doc.SectionCount(), 3u);
  const Ini::Section expected_one = {{"name_1", "value_1"}, {"name_2", "value_2"}};
  const Ini::Section expected_two = {
    {"name_1", "value_1"}, {"name_2", "value_2"}, {"name_3", "value_3"}
  };
  const Ini::Section expected_three = {{"name_1", "value_1"}};

  ASSERT_EQUAL(doc.GetSection("one"), expected_one);
  ASSERT_EQUAL(doc.GetSection("two"), expected_two);
  ASSERT_EQUAL(doc.GetSection("three"), expected_three);
}

void TestUnknownSection() {
  Ini::Document doc;
  doc.AddSection("primary");

  try {
    doc.GetSection("secondary");
    Assert(
      false,
      "Ini::Document::GetSection() should throw std::out_of_range for unknown section"
    );
  } catch (out_of_range&) {
  } catch (...) {
    Assert(
      false,
      "Ini::Document::GetSection() throws unexpected exception for unknown section"
    );
  }
}

void TestDuplicateSections() {
  Ini::Document doc;
  doc.AddSection("one").insert({"key_1", "value_1"});
  doc.AddSection("one").insert({"key_2", "value_2"});

  const Ini::Section expected = {{"key_1", "value_1"}, {"key_2", "value_2"}};
  ASSERT_EQUAL(doc.GetSection("one"), expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLoadIni);
  RUN_TEST(tr, TestDocument);
  RUN_TEST(tr, TestUnknownSection);
  RUN_TEST(tr, TestDuplicateSections);
  return 0;
}
