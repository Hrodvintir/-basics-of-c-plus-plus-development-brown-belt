#include "Common.h"

#include <unordered_map>
#include <list>
#include <mutex>

using namespace std;

class LruCache : public ICache {
public:
    LruCache(
            shared_ptr<IBooksUnpacker> books_unpacker, const Settings &settings) :
            books_unpacker_(move(books_unpacker)), settings_(settings) {}

    BookPtr GetBook(const string &book_name) override {
        lock_guard<mutex> lock(mx);

        if (auto it = cache_.find(book_name); it != cache_.end()) {
            rate_.splice(rate_.begin(), rate_, it->second);
            return rate_.front();
        }

        auto book = books_unpacker_->UnpackBook(book_name);

        if (book->GetContent().size() > settings_.max_memory) {
            rate_.clear();
            cache_.clear();
            total_size = 0;

            return move(book);
        }

        total_size += book->GetContent().size();

        if (total_size > settings_.max_memory) {
            while (total_size > settings_.max_memory) {
                total_size -= rate_.back()->GetContent().size();
                cache_.erase(rate_.back()->GetName());
                rate_.pop_back();
            }
        }

        rate_.emplace_front(move(book));
        cache_[rate_.front()->GetName()] = rate_.begin();

        return *cache_[book_name];
    }

    ~LruCache() override {

    }

private:
    shared_ptr<IBooksUnpacker> books_unpacker_;
    const Settings settings_;
    list<BookPtr> rate_;
    unordered_map<string, list<BookPtr>::iterator> cache_;
    size_t total_size = 0;
    mutable mutex mx;
};


unique_ptr<ICache> MakeCache(
        shared_ptr<IBooksUnpacker> books_unpacker,
        const ICache::Settings &settings
) {
    return make_unique<LruCache>(books_unpacker, settings);
}
