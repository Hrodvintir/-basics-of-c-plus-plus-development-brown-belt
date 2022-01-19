#include <mutex>
#include <unordered_map>
#include <vector>
#include <utility>
using namespace std;

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
public:
    using MapType = unordered_map<K, V, Hash>;

private:
    struct Bucket {
        MapType data;
        mutable mutex m;
    };

    Hash hasher;
    vector<Bucket> buckets;

public:
    // Выполняем наследование от lock_guard<mutex>, чтобы гарантировать, что мьютекс
    // будет захвачен до модификации bucket'а. Вместо наследования можно было бы
    // просто объявить поле типа lock_guard<mutex>, но такой подход полагается
    // на порядок инициализации полей и не гарантирует, что в будущем он случайно не поменяется
    struct WriteAccess : lock_guard<mutex> {
        V& ref_to_value;

        WriteAccess(const K& key, Bucket& bucket)
                : lock_guard(bucket.m)
                , ref_to_value(bucket.data[key])
        {
        }
    };

    struct ReadAccess : lock_guard<mutex> {
        const V& ref_to_value;

        ReadAccess(const K& key, const Bucket& bucket)
                : lock_guard(bucket.m)
                , ref_to_value(bucket.data.at(key))
        {
        }
    };

    explicit ConcurrentMap(size_t bucket_count)
            : buckets(bucket_count)
    {
    }

    WriteAccess operator[](const K& key) {
        return {key, buckets[GetIndex(key)]};
    }

    ReadAccess At(const K& key) const {
        return {key, buckets[GetIndex(key)]};
    }

    bool Has(const K& key) const {
        auto& bucket = buckets[GetIndex(key)];
        lock_guard g(bucket.m);
        return bucket.data.count(key) > 0;
    }

    MapType BuildOrdinaryMap() const {
        MapType result;
        for (auto& [data, mtx] : buckets) {
            lock_guard g(mtx);
            result.insert(begin(data), end(data));
        }
        return result;
    }

private:
    size_t GetIndex(const K& key) const {
        return hasher(key) % buckets.size();
    }
};
