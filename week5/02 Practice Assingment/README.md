# Электронная книга
*Вам даны задача и её решение — верное, но не удовлетворяющее заданным ограничениям на время работы. Переработайте это решение и сдайте в систему.*
## Условие
Разработайте систему стимулирования чтения электронных книг. Для простоты будем считать, что книга всего одна, но её одновременно читают много людей. Необходимо следить за прогрессом чтения у всех пользователей и выводить мотивирующие уведомления. А именно, ваша программа должна обрабатывать следующие события:

 - **READ** ***userpage*** — сохранить факт того, что пользователь под номером ***user*** дочитал книгу до страницы ***page***. Если ранее такой пользователь не встречался, необходимо его добавить. Гарантируется, что в рамках одного пользователя номера страниц в соответствующих ему событиях возрастают.

 - **CHEER** ***user*** — сообщить пользователю ***user***, какая доля существующих пользователей (не считая его самого) прочитала меньшую часть книги, чем он. Если этот пользователь на данный момент единственный, доля считается равной 1. Если для данного пользователя пока не было ни одного события ***READ***, доля считается равной 0, а сам пользователь не учитывается при вычислении долей для других пользователей до тех пор, пока для него не случится событие ***READ***.
## Формат входных данных
В первой строке вводится количество запросов Q — натуральное число, не превосходящее 10^6. В следующих Q строках в соответствии с описанным выше форматом вводятся запросы. Гарантируется, что все вводимые числа целые и положительные, при этом номера пользователей не превосходят 10^5, а номера страниц не превосходят 1000.

## Формат выходных данных
Для каждого запроса **CHEER**  ***user*** выведите единственное вещественное число от 0 до 1 — долю пользователей, прочитавших меньше страниц, чем ***user***. Формат вывода этого числа должен быть в точности таким же, как в опубликованном ниже медленном решении.

## Ограничения
4 секунды на выполнение всех запросов. Все описанные в условии гарантии действительно справедливы для всех тестов, на которых будет запускаться ваша программа. Проверять корректность тестов не нужно.

## Пример
### Ввод
```c++
12
CHEER 5
READ 1 10
CHEER 1
READ 2 5
READ 3 7
CHEER 2
CHEER 3
READ 3 10
CHEER 3
READ 3 11
CHEER 3
CHEER 1

```
### Вывод
```c++
0
1
0
0.5
0.5
1
0.5

```
#### Комментарии к примеру
Пользователь 5 не учитывается при вычислении долей, потому что для него не произошло ни одного события **READ**.

Пользователь 1 изначально был единственным, но в конце его обогнал 3-й, но не обогнал 2-й, поэтому он оказался продуктивнее 50 % пользователей.

Пользователь 3 изначально обгонял только 2-го и потому получал долю 50 %, но в конце обогнал 1-го и получил долю 100 %.

### Правильное, но медленное решение
```c++
#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

class ReadingManager {
public:
  ReadingManager()
      : user_page_counts_(MAX_USER_COUNT_ + 1, 0),
        sorted_users_(),
        user_positions_(MAX_USER_COUNT_ + 1, -1) {}

  void Read(int user_id, int page_count) {
    if (user_page_counts_[user_id] == 0) {
      AddUser(user_id);
    }
    user_page_counts_[user_id] = page_count;
    int& position = user_positions_[user_id];
    while (position > 0 && page_count > user_page_counts_[sorted_users_[position - 1]]) {
      SwapUsers(position, position - 1);
    }
  }

  double Cheer(int user_id) const {
    if (user_page_counts_[user_id] == 0) {
      return 0;
    }
    const int user_count = GetUserCount();
    if (user_count == 1) {
      return 1;
    }
    const int page_count = user_page_counts_[user_id];
    int position = user_positions_[user_id];
    while (position < user_count &&
      user_page_counts_[sorted_users_[position]] == page_count) {
      ++position;
    }
    if (position == user_count) {
        return 0;
    }
    // По умолчанию деление целочисленное, поэтому
    // нужно привести числитель к типу double.
    // Простой способ сделать это — умножить его на 1.0.
    return (user_count - position) * 1.0 / (user_count - 1);
  }

private:
  // Статическое поле не принадлежит какому-то конкретному
  // объекту класса. По сути это глобальная переменная,
  // в данном случае константная.
  // Будь она публичной, к ней можно было бы обратиться снаружи
  // следующим образом: ReadingManager::MAX_USER_COUNT.
  static const int MAX_USER_COUNT_ = 100'000;

  vector<int> user_page_counts_;
  vector<int> sorted_users_;   // отсортированы по убыванию количества страниц
  vector<int> user_positions_; // позиции в векторе sorted_users_

  int GetUserCount() const {
    return sorted_users_.size();
  }
  void AddUser(int user_id) {
    sorted_users_.push_back(user_id);
    user_positions_[user_id] = sorted_users_.size() - 1;
  }
  void SwapUsers(int lhs_position, int rhs_position) {
    const int lhs_id = sorted_users_[lhs_position];
    const int rhs_id = sorted_users_[rhs_position];
    swap(sorted_users_[lhs_position], sorted_users_[rhs_position]);
    swap(user_positions_[lhs_id], user_positions_[rhs_id]);
  }
};


int main() {
  // Для ускорения чтения данных отключается синхронизация
  // cin и cout с stdio,
  // а также выполняется отвязка cin от cout
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}
```
### Подсказки
Если задача вызывает непреодолимые трудности, вы можете воспользоваться подсказками. Они помогут быстрее сдать задачу, но в итоге вы получите меньше пользы от процесса поиска решения.

Не нужно открывать все подсказки сразу. Используйте их последовательно.
