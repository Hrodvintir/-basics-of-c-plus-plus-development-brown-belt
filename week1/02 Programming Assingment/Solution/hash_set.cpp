#include "test_runner.h"
//#include "gtest/gtest.h"

#include <forward_list>
#include <iterator>
#include <algorithm>

using namespace std;

template<typename Type, typename Hasher>
class HashSet {
 public:
  using BucketList = forward_list<Type>;

 public:
  explicit HashSet(
      size_t num_buckets,
      const Hasher &hasher = {}
  ) : num_buckets_(num_buckets),
      hasher_(hasher) {
      buckets.resize(num_buckets_);
  }

  void Add(const Type &value) {
//    auto index = HashFuncion(value);
    auto index = hasher_(value);
    index %= num_buckets_;
    auto it = find(buckets[index].begin(), buckets[index].end(), value);
    if (it == buckets[index].end()) {
      buckets[index].push_front(value);
    }
  }

  bool Has(const Type &value) const {
    auto index = hasher_(value);
    index %= num_buckets_;
//    auto index = HashFuncion(value);
    auto it = find(buckets[index].begin(), buckets[index].end(), value);
//    return it == buckets[index].end() ? false : true;
    if (it != buckets[index].end()) {
      return true;
    } else {
      return false;
    }
  }

  void Erase(const Type &value) {
//    auto index = HashFuncion(value);
    auto index = hasher_(value);
    index %= num_buckets_;
    auto it = find(buckets[index].begin(), buckets[index].end(), value);
    if (it != buckets[index].end()) {
      buckets[index].remove(value);
    }
  }

  const BucketList &GetBucket(const Type &value) const {
    auto index = hasher_(value);
    index %= num_buckets_;
//    auto index = HashFuncion(value);
    return buckets[index];
  }

  int HashFuncion(const Type& value) const {
    return (value % num_buckets_);
  }

 private:
  size_t num_buckets_;
  vector<BucketList> buckets;
  Hasher hasher_;
};

struct IntHasher {
  size_t operator()(int value) const {
    // Это реальная хеш-функция из libc++, libstdc++.
    // Чтобы она работала хорошо, std::unordered_map
    // использует простые числа для числа бакетов
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

int operator%(TestValue item, size_t num_bucket)
{
  return item.value % num_bucket;
}

struct TestValueHasher {
  size_t operator()(TestValue value) const {
    return value.value / 2;
  }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto &bucket = hash_set.GetBucket(TestValue{2});
  const auto &three_bucket = hash_set.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

void TestAdd() {
  HashSet<int, IntHasher> hash_set(5);
  hash_set.Add(3);
  ASSERT_EQUAL(hash_set.Has(3), true);
  hash_set.Erase(3);
  ASSERT_EQUAL(hash_set.Has(3), false);
}

void run() {
//  TestRunner tr;
////  RUN_TEST(tr, TestAdd);
//  RUN_TEST(tr, TestSmoke);
//  RUN_TEST(tr, TestEmpty);
//  RUN_TEST(tr, TestIdempotency);
//  RUN_TEST(tr, TestEquivalence);

  cout << "hash_set is running." << endl;
}


int main() {
  TestRunner tr;
//  RUN_TEST(tr, TestAdd);
  RUN_TEST(tr, TestSmoke);
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestIdempotency);
  RUN_TEST(tr, TestEquivalence);

  run();

  return 0;
}

// Compile error: b'/tmp/submissionpde1vadr/tmpi7i3qc2i.cpp: In instantiation of \xe2\x80\x98void HashSet<Type, Hasher>::Add(const Type&) [with Type = int; Hasher = IntHasher]\xe2\x80\x99:\n/tmp/submissionpde1vadr/tmpi7i3qc2i.cpp:94:17:   required from here\n/tmp/submissionpde1vadr/tmpi7i3qc2i.cpp:24:19: error: no matching function for call to \xe2\x80\x98find(std::forward_list<int, std::allocator<int> >::iterator, std::forward_list<int, std::allocator<int> >::iterator, const int&)\xe2\x80\x99\n     auto it = find(buckets[index].begin(), buckets[index].end(), value);\n               ~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\ncompilation terminated due to -Wfatal-errors.\n'
