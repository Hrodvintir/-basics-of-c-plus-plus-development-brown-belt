### Замечание ###
Эта задача — продолжение задачи «Шаблон Synchronized» из «Красного пояса по C++». Ниже приведено её полное условие с необходимыми дополнениями. Так что если вы не проходили «Красный пояс», можете просто решать эту задачу с нуля.

Дополнительные требования по сравнению с «Красным поясом по С++»:

 - нужно добавить константный метод GetAccess, который возвращает объект, содержащий константную ссылку на хранимый объект

### Условие ###
Как известно, отсутствие синхронизации доступа нескольких потоков к одним и тем же данным может фатально сказаться на работе программы. Поэтому очень важно не забывать захватывать мьютекс при обращении к разделяемым данным. Но если у нас есть большой проект, в котором доступ к общим данным осуществляется из большого количества мест в коде, то не так уж и трудно забыть сделать это, внося очередные правки.

Чтобы защититься от такой забывчивости, можно написать специальную обёртку над данными, которая будет предоставлять к ним доступ только под мьютексом. В этой задаче вам надо разработать такую обёртку — шаблон класса Synchronized вот с таким интерфейсом:
```objectivec
template <typename T>
class Synchronized {
public:
  explicit Synchronized(T initial = T());

  Access1 GetAccess();
  Access2 GetAccess() const;
private:
  T value;
};
```
Типы Access1 и Access2 должны быть структурами, в которых есть поля T& ref_to_value и const T& ref_to_valueсоответственно. В обоих случаях поле ref_to_value должно ссылаться на поле value класса Synchronized<T>. Кроме того, объекты классов Access1 и Access2 должны вести себя аналогично классу lock_guard<mutex>: захватывать мьютекс в своём конструкторе и освобождать в деструкторе. Пример:
  
```objectivec
Synchronized<int> s_int;
{
  auto access = s_int.GetAccess();
  // Критическая секция начинается отсюда и продолжается до конца блока
  access.ref_to_value = 5;
}
ASSERT_EQUAL(s_int.GetAccess().ref_to_value, 5);
```
  
Вам можно (и даже нужно) добавлять необходимые поля в классы Access1, Access2 и шаблон Synchronized. Более подробные примеры использования шаблона Synchronized смотрите в заготовке решения.

Как видите, единственный способ получить доступ к полю value класса Synchronized<T> — это обратиться к полю ref_to_value классов Access1 и Access2, которые гарантируют, что этот доступ осуществляется под мьютексом. Таким образом, если разделяемые данные обернуть в шаблон Synchronized, вы никогда не забудете захватить мьютекс при обращении к ним.
### Примечание ###
Проверяющий код никак не закладывается на имена типов, возвращаемых методами GetAccess, поэтому вы можете назвать их не Access1 и Access2, а как-то более осмысленно.
### Заготовка решения задачи ###
  ```objectivec
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
  ```
