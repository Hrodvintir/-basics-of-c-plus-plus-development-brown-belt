#include "Common.h"

using namespace std;

class LruCache : public ICache {
public:
  LruCache(
      shared_ptr<IBooksUnpacker> books_unpacker,
      const Settings& settings
  ) {
    // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ Ð¼ÐµÑ‚Ð¾Ð´
  }

  BookPtr GetBook(const string& book_name) override {
    // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ Ð¼ÐµÑ‚Ð¾Ð´
  }
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ Ñ„ÑƒÐ½ÐºÑ†Ð¸ÑŽ
}
