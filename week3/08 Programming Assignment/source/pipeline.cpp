#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // Ñ‚Ð¾Ð»ÑŒÐºÐ¾ Ð¿ÐµÑ€Ð²Ð¾Ð¼Ñƒ worker-Ñƒ Ð² Ð¿Ð°Ð¹Ð¿Ð»Ð°Ð¹Ð½Ðµ Ð½ÑƒÐ¶Ð½Ð¾ ÑÑ‚Ð¾ Ð¸Ð¼Ð¿Ð»ÐµÐ¼ÐµÐ½Ñ‚Ð¸Ñ€Ð¾Ð²Ð°Ñ‚ÑŒ
    throw logic_error("Unimplemented");
  }

protected:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ Ð´Ð¾Ð»Ð¶Ð½Ñ‹ Ð²Ñ‹Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ PassOn, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿ÐµÑ€ÐµÐ´Ð°Ñ‚ÑŒ Ð¾Ð±ÑŠÐµÐºÑ‚ Ð´Ð°Ð»ÑŒÑˆÐµ
  // Ð¿Ð¾ Ñ†ÐµÐ¿Ð¾Ñ‡ÐºÐµ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ¾Ð²
  void PassOn(unique_ptr<Email> email) const;

public:
  void SetNext(unique_ptr<Worker> next);
};


class Reader : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Copier : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Sender : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


// Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
class PipelineBuilder {
public:
  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð² ÐºÐ°Ñ‡ÐµÑÑ‚Ð²Ðµ Ð¿ÐµÑ€Ð²Ð¾Ð³Ð¾ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ° Reader
  explicit PipelineBuilder(istream& in);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Filter
  PipelineBuilder& FilterBy(Filter::Function filter);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Copier
  PipelineBuilder& CopyTo(string recipient);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Sender
  PipelineBuilder& Send(ostream& out);

  // Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ð³Ð¾Ñ‚Ð¾Ð²ÑƒÑŽ Ñ†ÐµÐ¿Ð¾Ñ‡ÐºÑƒ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ¾Ð²
  unique_ptr<Worker> Build();
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
