# Тесты к задаче «Демографические показатели»

Дано два верных решения задачи «Демографические показатели».

### (A) С минимальным количеством функций:
```c++
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename iterator_traits<InputIt>::value_type> range_copy(
      range_begin,
      range_end
  );
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

int main() {
  int person_count;
  cin >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    cin >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }

  auto females_end = partition(
      begin(persons), end(persons),
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = partition(
      begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = partition(
      females_end, end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  cout << "Median age = "
       << ComputeMedianAge(begin(persons), end(persons))         << endl
       << "Median age for females = "
       << ComputeMedianAge(begin(persons), females_end)          << endl
       << "Median age for males = "
       << ComputeMedianAge(females_end, end(persons))            << endl
       << "Median age for employed females = "
       << ComputeMedianAge(begin(persons), employed_females_end) << endl
       << "Median age for unemployed females = "
       << ComputeMedianAge(employed_females_end, females_end)    << endl
       << "Median age for employed males = "
       << ComputeMedianAge(females_end, employed_males_end)      << endl
       << "Median age for unemployed males = "
       << ComputeMedianAge(employed_males_end, end(persons))     << endl;

  return 0;
}
```
### (B) Аккуратно разбитое на функции:
```c++
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

bool operator==(const Person& lhs, const Person& rhs) {
  return lhs.age == rhs.age
      && lhs.gender == rhs.gender
      && lhs.is_employed == rhs.is_employed;
}

ostream& operator<<(ostream& stream, const Person& person) {
  return stream << "Person(age=" << person.age 
      << ", gender=" << static_cast<int>(person.gender)
      << ", is_employed=" << boolalpha << person.is_employed << ")";
}

struct AgeStats {
  int total;
  int females;
  int males;
  int employed_females;
  int unemployed_females;
  int employed_males;
  int unemployed_males;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename iterator_traits<InputIt>::value_type> range_copy(
      range_begin,
      range_end
  );
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

vector<Person> ReadPersons(istream& in_stream = cin) {
  int person_count;
  in_stream >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    in_stream >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }
  return persons;
}

AgeStats ComputeStats(vector<Person> persons) {
  //                 persons
  //                |       |
  //          females        males
  //         |       |      |     |
  //      empl.  unempl. empl.   unempl.

  auto females_end = partition(
      begin(persons), end(persons),
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = partition(
      begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = partition(
      females_end, end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  return {
       ComputeMedianAge(begin(persons), end(persons)),
       ComputeMedianAge(begin(persons), females_end),
       ComputeMedianAge(females_end, end(persons)),
       ComputeMedianAge(begin(persons),  employed_females_end),
       ComputeMedianAge(employed_females_end, females_end),
       ComputeMedianAge(females_end, employed_males_end),
       ComputeMedianAge(employed_males_end, end(persons))
  };
}

void PrintStats(const AgeStats& stats,
                ostream& out_stream = cout) {
  out_stream << "Median age = "
             << stats.total              << endl
             << "Median age for females = "
             << stats.females            << endl
             << "Median age for males = "
             << stats.males              << endl
             << "Median age for employed females = "
             << stats.employed_females   << endl
             << "Median age for unemployed females = "
             << stats.unemployed_females << endl
             << "Median age for employed males = "
             << stats.employed_males     << endl
             << "Median age for unemployed males = "
             << stats.unemployed_males   << endl;
}

int main() {
  PrintStats(ComputeStats(ReadPersons()));
  return 0;
}

```
Этой задачей мы хотим продемонстрировать, насколько легче решение (B) позволяет отлавливать ошибки с помощью юнит-тестов.

Даны 6 вариантов решения (A), в каждом из которых есть некоторая ошибка. Вам необходимо разработать юнит-тесты на решение (B), покрывающее ошибки в решении (B), полностью аналогичные продемонстрированным ошибкам в решении (A).

При написании юнит-тестов используйте библиотеку юнит-тестов test_runner.h из «Красного пояса»:
### test_runner.h

Отправленная вами на проверку программа должна содержать функцию main, прямо или косвенно вызывающую необходимые юнит-тесты.

Ваша программа с юнит-тестами будет тестироваться следующим образом:

 - Для каждого из решений bugX_bad.cpp нами создано решение bugX_good.cpp с такой же ошибкой, но в решении (B).

 - К каждому решению bugX_good.cpp добавляется отправленная вами программа, полученная большая программа запускается.

 - Каждый из запусков должен упасть, то есть юнит-тесты должны найти все 6 ошибок.

 - Аналогичный запуск для самого решения (B) должен пройти успешно.
