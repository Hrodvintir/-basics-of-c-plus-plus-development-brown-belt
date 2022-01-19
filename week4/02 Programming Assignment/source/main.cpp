#include "Common.h"
#include "test_runner.h"

#include <atomic>
#include <future>
#include <numeric>
#include <random>
#include <sstream>

using namespace std;

// Ð”Ð°Ð½Ð½Ð°Ñ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ Ð¸Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹ÑÐ° IBook Ð¿Ð¾Ð·Ð²Ð¾Ð»ÑÐµÑ‚ Ð¾Ñ‚ÑÐ»ÐµÐ´Ð¸Ñ‚ÑŒ Ð¾Ð±ÑŠÑ‘Ð¼ Ð¿Ð°Ð¼ÑÑ‚Ð¸, Ð² Ð´Ð°Ð½Ð½Ñ‹Ð¹
// Ð¼Ð¾Ð¼ÐµÐ½Ñ‚ Ð·Ð°Ð½Ð¸Ð¼Ð°ÐµÐ¼Ñ‹Ð¹ Ð²ÑÐµÐ¼Ð¸ Ð·Ð°Ð³Ñ€ÑƒÐ¶ÐµÐ½Ð½Ñ‹Ð¼Ð¸ ÐºÐ½Ð¸Ð³Ð°Ð¼Ð¸. Ð”Ð»Ñ Ñ‚ÐµÑÑ‚Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ñ ÑÐ²Ð¾ÐµÐ¹
// Ð¿Ñ€Ð¾Ð³Ñ€Ð°Ð¼Ð¼Ñ‹ Ð²Ñ‹ Ð¼Ð¾Ð¶ÐµÑ‚Ðµ Ð½Ð°Ð¿Ð¸ÑÐ°Ñ‚ÑŒ Ð´Ñ€ÑƒÐ³ÑƒÑŽ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸ÑŽ, ÐºÐ¾Ñ‚Ð¾Ñ€Ð°Ñ Ð¿Ð¾Ð·Ð²Ð¾Ð»Ð¸Ñ‚ Ñ‚Ð°ÐºÐ¶Ðµ
// ÑƒÐ±ÐµÐ´Ð¸Ñ‚ÑŒÑÑ, Ñ‡Ñ‚Ð¾ Ð¸Ð· ÐºÑÑˆÐ° Ð²Ñ‹Ð³Ñ€ÑƒÐ¶Ð°ÑŽÑ‚ÑÑ Ð² Ð¿ÐµÑ€Ð²ÑƒÑŽ Ð¾Ñ‡ÐµÑ€ÐµÐ´ÑŒ Ð½Ð°Ð¸Ð¼ÐµÐ½ÐµÐµ Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·ÑƒÐµÐ¼Ñ‹Ðµ
// ÑÐ»ÐµÐ¼ÐµÐ½Ñ‚Ñ‹. Ð¡Ð¾Ð±ÑÑ‚Ð²ÐµÐ½Ð½Ð¾, Ñ‚ÐµÑÑ‚Ð¸Ñ€ÑƒÑŽÑ‰Ð°Ñ ÑÐ¸ÑÑ‚ÐµÐ¼Ð° ÐºÑƒÑ€ÑÐµÑ€Ñ‹ Ð¸Ð¼ÐµÐµÑ‚ ÐºÐ°Ðº Ñ€Ð°Ð· Ð±Ð¾Ð»ÐµÐµ
// Ð¿Ñ€Ð¾Ð´Ð²Ð¸Ð½ÑƒÑ‚ÑƒÑŽ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸ÑŽ.
class Book : public IBook {
public:
  Book(
      string name,
      string content,
      atomic<size_t>& memory_used_by_books
  )
    : name_(move(name))
    , content_(move(content))
    , memory_used_by_books_(memory_used_by_books)
  {
    memory_used_by_books_ += content_.size();
  }

  ~Book() {
    memory_used_by_books_ -= content_.size();
  }

  const string& GetName() const override {
    return name_;
  }

  const string& GetContent() const override {
    return content_;
  }

private:
  string name_;
  string content_;
  atomic<size_t>& memory_used_by_books_;
};

// Ð”Ð°Ð½Ð½Ð°Ñ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ Ð¸Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹ÑÐ° IBooksUnpacker Ð¿Ð¾Ð·Ð²Ð¾Ð»ÑÐµÑ‚ Ð¾Ñ‚ÑÐ»ÐµÐ´Ð¸Ñ‚ÑŒ Ð¾Ð±ÑŠÑ‘Ð¼ Ð¿Ð°Ð¼ÑÑ‚Ð¸,
// Ð² Ð´Ð°Ð½Ð½Ñ‹Ð¹ Ð¼Ð¾Ð¼ÐµÐ½Ñ‚ Ð·Ð°Ð½Ð¸Ð¼Ð°ÐµÐ¼Ñ‹Ð¹ Ð²ÑÐµÐ¼Ð¸ Ð·Ð°Ð³Ñ€ÑƒÐ¶ÐµÐ½Ð½Ñ‹Ð¼Ð¸ ÐºÐ½Ð¸Ð³Ð°Ð¼Ð¸ Ð¸ Ð·Ð°Ð¿Ñ€Ð¾ÑÐ¸Ñ‚ÑŒ ÐºÐ¾Ð»Ð¸Ñ‡ÐµÑÑ‚Ð²Ð¾
// Ð¾Ð±Ñ€Ð°Ñ‰ÐµÐ½Ð¸Ð¹ Ðº Ð¼ÐµÑ‚Ð¾Ð´Ñƒ UnpackBook(). Ð”Ð»Ñ Ñ‚ÐµÑÑ‚Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ñ ÑÐ²Ð¾ÐµÐ¹ Ð¿Ñ€Ð¾Ð³Ñ€Ð°Ð¼Ð¼Ñ‹ Ð²Ñ‹ Ð¼Ð¾Ð¶ÐµÑ‚Ðµ
// Ð½Ð°Ð¿Ð¸ÑÐ°Ñ‚ÑŒ Ð´Ñ€ÑƒÐ³ÑƒÑŽ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸ÑŽ. Ð¡Ð¾Ð±ÑÑ‚Ð²ÐµÐ½Ð½Ð¾, Ñ‚ÐµÑÑ‚Ð¸Ñ€ÑƒÑŽÑ‰Ð°Ñ ÑÐ¸ÑÑ‚ÐµÐ¼Ð° ÐºÑƒÑ€ÑÐµÑ€Ñ‹ Ð¸Ð¼ÐµÐµÑ‚ ÐºÐ°Ðº
// Ñ€Ð°Ð· Ð±Ð¾Ð»ÐµÐµ Ð¿Ñ€Ð¾Ð´Ð²Ð¸Ð½ÑƒÑ‚ÑƒÑŽ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸ÑŽ.
class BooksUnpacker : public IBooksUnpacker {
public:
  unique_ptr<IBook> UnpackBook(const string& book_name) override {
    ++unpacked_books_count_;
    return make_unique<Book>(
      book_name,
      "Dummy content of the book " + book_name,
      memory_used_by_books_
    );
  }

  size_t GetMemoryUsedByBooks() const {
    return memory_used_by_books_;
  }

  int GetUnpackedBooksCount() const {
    return unpacked_books_count_;
  }

private:
  // Ð¨Ð°Ð±Ð»Ð¾Ð½Ð½Ñ‹Ð¹ ÐºÐ»Ð°ÑÑ atomic Ð¿Ð¾Ð·Ð²Ð¾Ð»ÑÐµÑ‚ Ð±ÐµÐ·Ð¾Ð¿Ð°ÑÐ½Ð¾ Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ñ‚ÑŒ ÑÐºÐ°Ð»ÑÑ€Ð½Ñ‹Ð¹ Ñ‚Ð¸Ð¿ Ð¸Ð·
  // Ð½ÐµÑÐºÐ¾Ð»ÑŒÐºÐ¸Ñ… Ð¿Ð¾Ñ‚Ð¾ÐºÐ¾Ð². Ð’ Ð¿Ñ€Ð¾Ñ‚Ð¸Ð²Ð½Ð¾Ð¼ ÑÐ»ÑƒÑ‡Ð°Ðµ Ñƒ Ð½Ð°Ñ Ð±Ñ‹Ð»Ð¾ Ð±Ñ‹ ÑÐ¾ÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð³Ð¾Ð½ÐºÐ¸.
  atomic<size_t> memory_used_by_books_ = 0;
  atomic<int> unpacked_books_count_ = 0;
};

struct Library {
  vector<string> book_names;
  unordered_map<string, unique_ptr<IBook>> content;
  size_t size_in_bytes = 0;

  explicit Library(vector<string> a_book_names, IBooksUnpacker& unpacker)
    : book_names(std::move(a_book_names))
  {
    for (const auto& book_name : book_names) {
      auto& book_content = content[book_name];
      book_content = unpacker.UnpackBook(book_name);
      size_in_bytes += book_content->GetContent().size();
    }
  }
};


void TestUnpacker(const Library& lib) {
  BooksUnpacker unpacker;
  for (const auto& book_name : lib.book_names) {
    auto book = unpacker.UnpackBook(book_name);
    ASSERT_EQUAL(book->GetName(), book_name);
  }
}


void TestMaxMemory(const Library& lib) {
  auto unpacker = make_shared<BooksUnpacker>();
  ICache::Settings settings;
  settings.max_memory = lib.size_in_bytes / 2;
  auto cache = MakeCache(unpacker, settings);

  for (const auto& [name, book] : lib.content) {
    cache->GetBook(name);
    ASSERT(unpacker->GetMemoryUsedByBooks() <= settings.max_memory);
  }
}


void TestCaching(const Library& lib) {
  auto unpacker = make_shared<BooksUnpacker>();
  ICache::Settings settings;
  settings.max_memory = lib.size_in_bytes;
  auto cache = MakeCache(unpacker, settings);

  // Ð•ÑÐ»Ð¸ Ð·Ð°Ð¿Ñ€Ð°ÑˆÐ¸Ð²Ð°Ñ‚ÑŒ Ð¾Ð´Ð½Ñƒ Ð¸ Ñ‚Ñƒ Ð¶Ðµ ÐºÐ½Ð¸Ð³Ñƒ Ð¿Ð¾Ð´Ñ€ÑÐ´, Ñ‚Ð¾ Ð¾Ð½Ð° Ð¾Ð¿Ñ€ÐµÐ´ÐµÐ»Ñ‘Ð½Ð½Ð¾ Ð´Ð¾Ð»Ð¶Ð½Ð°
  // Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°Ñ‚ÑŒÑÑ Ð¸Ð· ÐºÑÑˆÐ°. Ð—Ð°Ð¼ÐµÑ‚ÑŒÑ‚Ðµ, Ñ‡Ñ‚Ð¾ ÑÑ‚Ð¾Ð³Ð¾ Ð¿Ñ€Ð¾ÑÑ‚Ð¾Ð³Ð¾ Ñ‚ÐµÑÑ‚Ð° Ð²Ð¾Ð²ÑÐµ
  // Ð½ÐµÐ´Ð¾ÑÑ‚Ð°Ñ‚Ð¾Ñ‡Ð½Ð¾, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿Ð¾Ð»Ð½Ð¾ÑÑ‚ÑŒÑŽ Ð¿Ñ€Ð¾Ð²ÐµÑ€Ð¸Ñ‚ÑŒ Ð¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½Ð¾ÑÑ‚ÑŒ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ ÑÑ‚Ñ€Ð°Ñ‚ÐµÐ³Ð¸Ð¸
  // Ð·Ð°Ð¼ÐµÑ‰ÐµÐ½Ð¸Ñ ÑÐ»ÐµÐ¼ÐµÐ½Ñ‚Ð¾Ð² Ð² ÐºÑÑˆÐµ. Ð”Ð»Ñ ÑÑ‚Ð¸Ñ… Ñ†ÐµÐ»ÐµÐ¹ Ð¼Ð¾Ð¶ÐµÑ‚Ðµ Ð½Ð°Ð¿Ð¸ÑÐ°Ñ‚ÑŒ Ñ‚ÐµÑÑ‚
  // ÑÐ°Ð¼Ð¾ÑÑ‚Ð¾ÑÑ‚ÐµÐ»ÑŒÐ½Ð¾.
  cache->GetBook(lib.book_names[0]);
  cache->GetBook(lib.book_names[0]);
  cache->GetBook(lib.book_names[0]);
  ASSERT_EQUAL(unpacker->GetUnpackedBooksCount(), 1);
}


void TestSmallCache(const Library& lib) {
  auto unpacker = make_shared<BooksUnpacker>();
  ICache::Settings settings;
  settings.max_memory =
      unpacker->UnpackBook(lib.book_names[0])->GetContent().size() - 1;
  auto cache = MakeCache(unpacker, settings);

  cache->GetBook(lib.book_names[0]);
  ASSERT_EQUAL(unpacker->GetMemoryUsedByBooks(), size_t(0));
}


void TestAsync(const Library& lib) {
  static const int tasks_count = 10;
  static const int trials_count = 10000;

  auto unpacker = make_shared<BooksUnpacker>();
  ICache::Settings settings;
  settings.max_memory = lib.size_in_bytes - 1;
  auto cache = MakeCache(unpacker, settings);

  vector<future<void>> tasks;

  for (int task_num = 0; task_num < tasks_count; ++task_num) {
    tasks.push_back(async([&cache, &lib, task_num] {
      default_random_engine gen;
      uniform_int_distribution<size_t> dis(0, lib.book_names.size() - 1);
      for (int i = 0; i < trials_count; ++i) {
        const auto& book_name = lib.book_names[dis(gen)];
        ASSERT_EQUAL(
            cache->GetBook(book_name)->GetContent(),
            lib.content.find(book_name)->second->GetContent()
        );
      }
      stringstream ss;
      ss << "Task #" << task_num << " completed\n";
      cout << ss.str();
    }));
  }

  // Ð²Ñ‹Ð·Ð¾Ð² Ð¼ÐµÑ‚Ð¾Ð´Ð° get Ð¿Ñ€Ð¾Ð±Ñ€Ð°ÑÑ‹Ð²Ð°ÐµÑ‚ Ð¸ÑÐºÐ»ÑŽÑ‡ÐµÐ½Ð¸Ñ Ð² Ð¾ÑÐ½Ð¾Ð²Ð½Ð¾Ð¹ Ð¿Ð¾Ñ‚Ð¾Ðº
  for (auto& task : tasks) {
    task.get();
  }
}


int main() {
  BooksUnpacker unpacker;
  const Library lib(
    // ÐÐ°Ð·Ð²Ð°Ð½Ð¸Ñ ÐºÐ½Ð¸Ð³ Ð´Ð»Ñ Ð»Ð¾ÐºÐ°Ð»ÑŒÐ½Ð¾Ð³Ð¾ Ñ‚ÐµÑÑ‚Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ñ. Ð’ Ñ‚ÐµÑÑ‚Ð¸Ñ€ÑƒÑŽÑ‰ÐµÐ¹ ÑÐ¸ÑÑ‚ÐµÐ¼Ðµ ÐºÑƒÑ€ÑÐµÑ€Ñ‹
    // Ð±ÑƒÐ´ÐµÑ‚ Ð´Ñ€ÑƒÐ³Ð¾Ð¹ Ð½Ð°Ð±Ð¾Ñ€, Ð½Ð°Ð¼Ð½Ð¾Ð³Ð¾ Ð±Ð¾Ð»ÑŒÑˆÐµ.
    {
      "Sherlock Holmes",
      "Don Quixote",
      "Harry Potter",
      "A Tale of Two Cities",
      "The Lord of the Rings",
      "Le Petit Prince",
      "Alice in Wonderland",
      "Dream of the Red Chamber",
      "And Then There Were None",
      "The Hobbit"
    },
    unpacker
  );

#define RUN_CACHE_TEST(tr, f) tr.RunTest([&lib] { f(lib); }, #f)

  TestRunner tr;
  RUN_CACHE_TEST(tr, TestUnpacker);
  RUN_CACHE_TEST(tr, TestMaxMemory);
  RUN_CACHE_TEST(tr, TestCaching);
  RUN_CACHE_TEST(tr, TestSmallCache);
  RUN_CACHE_TEST(tr, TestAsync);

#undef RUN_CACHE_TEST
  return 0;
}
