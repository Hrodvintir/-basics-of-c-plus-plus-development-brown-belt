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
  explicit Synchronized(T initial = T())
    : value(move(initial))
  {
  }

  template <typename U>
  struct Access {
    U& ref_to_value;
    lock_guard<mutex> guard;
  };

  Access<T> GetAccess() {
    return {value, lock_guard(m)};
  }

  Access<const T> GetAccess() const {
    return {value, lock_guard(m)};
  }

private:
  T value;
  mutable mutex m;
};
