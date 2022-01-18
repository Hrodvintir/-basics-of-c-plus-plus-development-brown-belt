#include "test_runner.h"

#include <numeric>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <queue>
using namespace std;

template <typename T>
class Synchronized {
public:
  explicit Synchronized(T initial = T());

  ??? GetAccess();
  ??? GetAccess() const;

private:
  T value;
};

void TestConcurrentUpdate() {
  Synchronized<string> common_string;

  const size_t add_count = 50000;
  auto updater = [&common_string, add_count] {
    for (size_t i = 0; i < add_count; ++i) {
      auto access = common_string.GetAccess();
      access.ref_to_value += 'a';
    }
  };

  auto f1 = async(updater);
  auto f2 = async(updater);

  f1.get();
  f2.get();

  ASSERT_EQUAL(common_string.GetAccess().ref_to_value.size(), 2 * add_count);
}

vector<int> Consume(Synchronized<deque<int>>& common_queue) {
  vector<int> got;

  for (;;) {
    deque<int> q;

    {
      // ÐœÑ‹ ÑÐ¿ÐµÑ†Ð¸Ð°Ð»ÑŒÐ½Ð¾ Ð·Ð°ÐºÐ»ÑŽÑ‡Ð¸Ð»Ð¸ ÑÑ‚Ð¸ Ð´Ð²Ðµ ÑÑ‚Ñ€Ð¾Ñ‡ÐºÐ¸ Ð² Ð¾Ð¿ÐµÑ€Ð°Ñ‚Ð¾Ñ€Ð½Ñ‹Ðµ ÑÐºÐ¾Ð±ÐºÐ¸, Ñ‡Ñ‚Ð¾Ð±Ñ‹
      // ÑƒÐ¼ÐµÐ½ÑŒÑˆÐ¸Ñ‚ÑŒ Ñ€Ð°Ð·Ð¼ÐµÑ€ ÐºÑ€Ð¸Ñ‚Ð¸Ñ‡ÐµÑÐºÐ¾Ð¹ ÑÐµÐºÑ†Ð¸Ð¸. ÐŸÐ¾Ñ‚Ð¾Ðº-Ð¿Ð¾Ñ‚Ñ€ÐµÐ±Ð¸Ñ‚ÐµÐ»ÑŒ Ð·Ð°Ñ…Ð²Ð°Ñ‚Ñ‹Ð²Ð°ÐµÑ‚
      // Ð¼ÑŒÑŽÑ‚ÐµÐºÑ, Ð¿ÐµÑ€ÐµÐ¼ÐµÑ‰Ð°ÐµÑ‚ Ð²ÑÑ‘ ÑÐ¾Ð´ÐµÑ€Ð¶Ð¸Ð¼Ð¾Ðµ Ð¾Ð±Ñ‰ÐµÐ¹ Ð¾Ñ‡ÐµÑ€ÐµÐ´Ð¸ Ð² ÑÐ²Ð¾ÑŽ
      // Ð»Ð¾ÐºÐ°Ð»ÑŒÐ½ÑƒÑŽ Ð¿ÐµÑ€ÐµÐ¼ÐµÐ½Ð½ÑƒÑŽ Ð¸ Ð¾Ñ‚Ð¿ÑƒÑÐºÐ°ÐµÑ‚ Ð¼ÑŒÑŽÑ‚ÐµÐºÑ. ÐŸÐ¾ÑÐ»Ðµ ÑÑ‚Ð¾Ð³Ð¾ Ð¾Ð½ Ð¾Ð±Ñ€Ð°Ð±Ð°Ñ‚Ñ‹Ð²Ð°ÐµÑ‚
      // Ð¾Ð±ÑŠÐµÐºÑ‚Ñ‹ Ð² Ð¾Ñ‡ÐµÑ€ÐµÐ´Ð¸ Ð·Ð° Ð¿Ñ€ÐµÐ´ÐµÐ»Ð°Ð¼Ð¸ ÐºÑ€Ð¸Ñ‚Ð¸Ñ‡ÐµÑÐºÐ¾Ð¹ ÑÐµÐºÑ†Ð¸Ð¸, Ð¿Ð¾Ð·Ð²Ð¾Ð»ÑÑ
      // Ð¿Ð¾Ñ‚Ð¾ÐºÑƒ-Ð¿Ñ€Ð¾Ð¸Ð·Ð²Ð¾Ð´Ð¸Ñ‚ÐµÐ»ÑŽ Ð¿Ð°Ñ€Ð°Ð»Ð»ÐµÐ»ÑŒÐ½Ð¾ Ð¿Ð¾Ð¼ÐµÑ‰Ð°Ñ‚ÑŒ Ð² Ð¾Ñ‡ÐµÑ€ÐµÐ´ÑŒ Ð½Ð¾Ð²Ñ‹Ðµ Ð¾Ð±ÑŠÐµÐºÑ‚Ñ‹.
      //
      // Ð Ð°Ð·Ð¼ÐµÑ€ ÐºÑ€Ð¸Ñ‚Ð¸Ñ‡ÐµÑÐºÐ¾Ð¹ ÑÐµÐºÑ†Ð¸Ð¸ ÑÑƒÑ‰ÐµÑÑ‚Ð²ÐµÐ½Ð½Ð¾ Ð²Ð»Ð¸ÑÐµÑ‚ Ð½Ð° Ð±Ñ‹ÑÑ‚Ñ€Ð¾Ð´ÐµÐ¹ÑÑ‚Ð²Ð¸Ðµ
      // Ð¼Ð½Ð¾Ð³Ð¾Ð¿Ð¾Ñ‚Ð¾Ñ‡Ð½Ñ‹Ñ… Ð¿Ñ€Ð¾Ð³Ñ€Ð°Ð¼Ð¼.
      auto access = common_queue.GetAccess();
      q = move(access.ref_to_value);
    }

    for (int item : q) {
      if (item > 0) {
        got.push_back(item);
      } else {
        return got;
      }
    }
  }
}

void Log(const Synchronized<deque<int>>& common_queue, ostream& out) {
  for (int i = 0; i < 100; ++i) {
    out << "Queue size is " << common_queue.GetAccess().ref_to_value.size() << '\n';
  }
}

void TestProducerConsumer() {
  Synchronized<deque<int>> common_queue;
  ostringstream log;

  auto consumer = async(Consume, ref(common_queue));
  auto logger = async(Log, cref(common_queue), ref(log));

  const size_t item_count = 100000;
  for (size_t i = 1; i <= item_count; ++i) {
    common_queue.GetAccess().ref_to_value.push_back(i);
  }
  common_queue.GetAccess().ref_to_value.push_back(-1);

  vector<int> expected(item_count);
  iota(begin(expected), end(expected), 1);
  ASSERT_EQUAL(consumer.get(), expected);

  logger.get();
  const string logs = log.str();
  ASSERT(!logs.empty());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestConcurrentUpdate);
  RUN_TEST(tr, TestProducerConsumer);
}
