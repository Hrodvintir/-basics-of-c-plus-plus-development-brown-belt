#include <algorithm>
#include <forward_list>
#include <iterator>
#include <vector>

using namespace std;

template <typename Type, typename Hasher = hash<Type>>
class HashSet {
 public:
  using BucketList = forward_list<Type>;

 public:
  explicit HashSet(
      size_t num_buckets,
      const Hasher& hasher = {}
  )
      : hasher_(hasher)
      , buckets_(num_buckets)
  {}

  void Add(const Type& value) {
    auto& bucket = buckets_[GetBucketIndex(value)];
    auto it = find(begin(bucket), end(bucket), value);

    if (it == bucket.end()) {
      bucket.push_front(value);
    }
  }

  bool Has(const Type& value) const {
    const auto& bucket = GetBucket(value);
    return find(begin(bucket), end(bucket), value) != bucket.end();
  }

  void Erase(const Type& value) {
    buckets_[GetBucketIndex(value)].remove(value);
  }

  const BucketList& GetBucket(const Type& value) const {
    return buckets_[GetBucketIndex(value)];
  }

 private:
  size_t GetBucketIndex(const Type& value) const {
    return hasher_(value) % buckets_.size();
  }

 private:
  Hasher hasher_;
  vector<BucketList> buckets_;
};
