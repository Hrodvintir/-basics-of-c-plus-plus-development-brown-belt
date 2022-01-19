# Ranges

Язык активно развивается, и в него регулярно добавляются новые возможности, существенно влияющие на стиль и архитектуру кода. Среди них можно выделить нововведения стандарта 2011 года (C++11), с которыми мы познакомили вас ещё в «Белом поясе»:

 - Лямбда-функции, благодаря которым отпала необходимость в создании отдельной функции для каждого компаратора или предиката.

 - Range-based for, позволивший лишний раз не думать об итераторах и писать понятное for (int number : numbers) вместо громоздкого for (auto it = numbers.begin(); it != numbers.end(); ++i). (Справедливости ради, auto тоже появилось в C++11 — без него понадобилось бы полное название типа vector<int>::iterator.)

В этой статье мы рассмотрим нововведение похожего масштаба — Ranges. В отличие от вышеупомянутых примеров, все изменения касаются лишь стандартной библиотеки и не вводят в язык новых конструкций, но при этом ranges существенно опираются на концепты (concepts) из C++20, а они, в свою очередь, вводят новые синтаксические конструкции и существенно дополняют механизмы шаблонов и перегрузки функций.
  
 ### Как попробовать
#### Компилятор
Установите компилятор GCC версии 10 или выше. Например, для Windows достаточно установить GCC 10 с сайта http://winlibs.com. После распаковки пропишите директорию bin в переменную окружения PATH: это нужно как для exe-файла компилятора, так и для dll-библиотек.

#### Запуск простейшей программы
Создайте файл с простейшей программой, сортирующей и выводящей на экран набор чисел:
  ```c++
  #include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>

namespace rng = std::ranges;

template <rng::input_range Range>
void Print(const Range& range) {
  for (const auto& x : range) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::vector v = {4, 1, 7, 2, 3, 8};
  rng::sort(v);
  Print(v);
  
  return 0;
}
  ```
  Скомпилируйте его:  
  ```c++
  >g++-10 ranges1.cpp -o test -std=c++20
  ```
  Наконец, запустите программу и увидите отсортированный набор чисел:
  ```c++
  >./test
1 2 3 4 7 8

  ```
  Если у вас возникли проблемы, воспользуйтесь поисковыми системами. Если это не помогает, можете ограничиться простым чтением этого материала: решать задачи с использованием Ranges не понадобится. Есть и компромисс — ограничиться проверкой компилируемости кода на gcc.godbolt.org.
  
  ### Диапазоны (ranges)
Основное нововведение библиотеки, анонсированное в том числе и на видеолекции, — новое понятие диапазона (range), призванное заменить собой не всегда удобные пары итераторов. В частности, все стандартные алгоритмы получили версии, принимающие range. Русский перевод этого термина пока не устоялся, поэтому мы ограничимся английской версией.

Как вы помните из «Жёлтого пояса», итератор — это не конкретный тип: это понятие, связывающее различные типы. Так же обстоит ситуация и с range: мы говорим, что можно вызвать функцию sort от одного параметра, являющегося range, но при этом без проблем передаём в неё вектор. Он, как и многие другие контейнеры, является range — что не выглядит неожиданным, так как ранее мы легко итерировались по контейнерам с помощью цикла for, а основное свойство range — это как раз возможность проитерироваться во всем элементам.

### Разбор примера
  **includes**
  ```c++
  #include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
  ```
  
  Мы подключаем все библиотеки стандартным способом.

### Пространства имён
Весь код ranges находится в пространстве имён std::ranges, что позволяет выбирать между существующими и новыми версиями алгоритмов. Поскольку директива using namespace неприменима одновременно к std и std::ranges (так как тот же sort есть и там, и там), ограничимся введением короткого синонима rng:
  ```c++
  namespace rng = std::ranges;
  ```
  Теперь в коде будет легко видно, где используются функции и классы из стандартной библиотеки (std::), а где — из ranges (rng::).

**Сортировка**
  ```c++
  std::vector v = {4, 1, 7, 2, 3, 8};
rng::sort(v);
  ```
  Этот пример демонстрирует ровно то, что обсуждалось выше: возможность вызвать сортировку от одного range вместо пары итераторов.

#### Функция Print
В качестве примера мы написали и использовали собственную функцию, принимающую range:
  
  ```c++
  template <rng::input_range Range>
void Print(const Range& range) {
  for (const auto& x : range) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

// ...

Print(v);

  ```
  Тело этой функции выглядит довольно стандартно. Но обратите внимание на первую строчку шаблона: вместо привычного typename указан rng::InputRange — это и есть типичный пример использования концепта. Мы слегка коснёмся их в следующем разделе, но подробно рассматривать не будем.

#### Применение концептов при работе с ranges
Вернёмся к определению шаблонной функции Print:
  ```c++
  template <rng::input_range Range>
void Print(const Range& range) { /* ... */ }
  ```
  Концепт — это набор требований к типу. Уже известный вам пример концептов — категории итераторов: например, функция sort требует передавать в неё итераторы произвольного доступа, потому что её реализация прибавляет к итераторам числа. При попытке передать в эту функцию итераторы списка или множества она не компилировалась при попытке сделать с итератором недоступную операцию.

Концепты по сути легализуют эту практику. Проектируя функцию Print для диапазонов элементов, мы понимаем, что в неё можно передавать не любые типы, а только те, по которым можно итерироваться. Поэтому мы указываем, что шаблонный тип Range должен удовлетворять стандартному концепту InputRange, то есть являться объектом, по которому можно проитерироваться с помощью input-итератора.

Попробуйте добавить в функцию main строчку Print(2); — и получите ошибку компиляции, сигнализирующую о несоответствии типа int концепту InputRange:
  ```c++
  ranges1.cpp: In function ‘int main()’:
ranges1.cpp:20:10: error: use of function ‘void Print(const Range&) [with Range = int]’ with unsatisfied constraints
   20 |   Print(2);
      |          ^
ranges1.cpp:9:6: note: declared here
    9 | void Print(const Range& range) {
      |      ^~~~~
ranges1.cpp:9:6: note: constraints not satisfied
In file included from /usr/include/c++/10/string:54,
                 from /usr/include/c++/10/bits/locale_classes.h:40,
                 from /usr/include/c++/10/bits/ios_base.h:41,
                 from /usr/include/c++/10/ios:42,
                 from /usr/include/c++/10/ostream:38,
                 from /usr/include/c++/10/iostream:39,
                 from ranges1.cpp:1:
/usr/include/c++/10/bits/range_access.h: In instantiation of ‘void Print(const Range&) [with Range = int]’:
ranges1.cpp:20:10:   required from here
/usr/include/c++/10/bits/range_access.h:862:13:   required for the satisfaction of ‘range<_Tp>’ [with _Tp = int]
/usr/include/c++/10/bits/range_access.h:909:13:   required for the satisfaction of ‘input_range<Range>’ [with Range = int]
/usr/include/c++/10/bits/range_access.h:862:21:   in requirements with ‘_Tp& __t’ [with _Tp = int]
/usr/include/c++/10/bits/range_access.h:864:15: note: the required expression ‘std::ranges::__cust::begin(__t)’ is invalid
  864 |  ranges::begin(__t);
      |  ~~~~~~~~~~~~~^~~~~
/usr/include/c++/10/bits/range_access.h:865:13: note: the required expression ‘std::ranges::__cust::end(__t)’ is invalid
  865 |  ranges::end(__t);
      |  ~~~~~~~~~~~^~~~~
cc1plus: note: set ‘-fconcepts-diagnostics-depth=’ to at least 2 for more detail

  ```
  Есть и другой вариант синтаксиса для объявления шаблонной функции с constrained template parameter (ограниченным [концептом] шаблонным параметром) — громоздкий, но подчёркивающий наличие ограничений:
  ```c++
  template <typename Range>
requires rng::input_range<Range>
void Print(const Range& range) { /* ... */ }

  ```
  Помимо более прозрачных ошибок компиляции, концепты предоставляют возможность более гибкой перегрузки функций:
  ```c++
  template <rng::input_range Range>
void Print(const Range& range) {
  for (const auto& x : range) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

template <typename T>
void Print(const T& x) {
  std::cout << x << std::endl;
}
  ```
  Теперь вызов функции Print для ranges будет приводить к выводу всех элементов в цикле, а вызов для любых других объектов — например, Print(2) — к простому выводу с помощью cout.

Наконец, приведём без подробных комментариев способ определить свой концепт типизированного range:
  ```c++
  #include <iostream>
#include <set>
#include <vector>
#include <ranges>
#include <algorithm>

namespace rng = std::ranges;

template <typename R, typename T>
concept InputRange =
    rng::input_range<R>
    && requires(R r) {
      {*rng::begin(r)} -> std::convertible_to<T>;
    };
// Выше написано следующее:
// тип R удовлетворяет концепту InputRange<T>,
// если он удовлетворяет rng::input_range
// и для переменной r типа R
// функция begin возвращает объект,
// приводимый к типу T

template <InputRange<int> Range>
void Print(const Range& range) {
  for (const auto& x : range) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::vector v = {4, 1, 7, 2, 3, 8};
  Print(v);
  
  std::set s = {8, 6, 9, 6};
  Print(s);
  
  return 0;
}
  ```
  ### Projection-параметры
Ещё одно важное улучшение стандартных алгоритмов — projection-параметры. Они позволяют передать функцию, которая будет применяться ко всем элементам (не меняя их) перед основными действиями алгоритма. Особенно это полезно для функций вроде std::sort и std::lower_bound, принимающих компараторы: если элементы необходимо отсортировать по некоторой функции от элемента, достаточно передать projection-параметр с этой функцией; при этом отпадает необходимость в громоздком компараторе от двух параметров.

Классический пример — сортировка объектов по некоторому полю:
  
  ```c++
  #include <iostream>
#include <vector>
#include <algorithm>

namespace rng = std::ranges;

struct Lecture {
  int course;
  int local_idx;
  int complexity;
};

int main() {
  std::vector<Lecture> lectures = ReadLectures();
  
  // как раньше
  std::sort(lectures.begin(), lectures.end(),
            [](const Lecture& lhs, const Lecture& rhs) {
              return lhs.complexity < rhs.complexity;
            });
  
  // как теперь
  rng::sort(lectures, std::less<>{},
            [](const Lecture& x) { return x.complexity; });
  
  return 0;
}
  ```
  Здесь std::less<>{} — это стандартный компаратор, сравнивающий объекты с помощью оператора <. Таким образом, для сравнения двух объектов типа Lecture для каждого вызывается projection-функция, возвращающая сложность, а затем полученные сложности сравниваются с помощью оператора <.

Для сортировки лекций в порядке убывания сложности достаточно использовать std::greater вместо std::less:
  ```c++
  rng::sort(lectures, std::greater<>{},
          [](const Lecture& x) { return x.complexity; });
  ```
  Наконец, напомним, что с помощью make_tuple можно сортировать объекты по комбинации полей:  
  ```c++
  rng::sort(lectures, std::less<>{},
          [](const Lecture& x) {
            return std::make_tuple(x.course, x.local_idx);
          });
  ```
  
  В данном примере лекции сортируются по номеру курса, а при его совпадении — по внутреннему индексу.  

### Views и range adaptors
Другая важная часть Ranges TS — views и range adaptors. И то, и другое — это особые ranges, которые сами не хранят данные, а лишь преобразуют данные другого view или генерируют данные, не храня их. Рассмотрим несколько примеров.

### view::iota
Наиболее полезный в быту класс — view::iota, позволяющий перебрать все числа в заданном диапазоне с помощью range-based for. Например, с помощью такого кода можно вывести первые 5 элементов вектора:
 ```c++
  #include <iostream>
#include <vector>
#include <ranges>

namespace rng = std::ranges;
namespace view = rng::views;

int main() {
  std::vector<int> numbers = ReadNumbers();

  for (int i : view::iota(0, 5)) {
    std::cout << numbers[i] << ' ';
  }
  
  return 0;
}
  ```
  Объект view::iota можно передавать в любые алгоритмы, читающие данные. При этом числа из заданного диапазона не хранятся в памяти, а лишь последовательно генерируются с помощью итераторов этого view.

### view::take
Впрочем, есть более прозрачный способ перебора первых 5 элементов контейнера:
  ```c++
  std::vector<int> numbers = ReadNumbers();

for (int x : numbers | view::take(5)) {
  std::cout << x << ' ';
}
  ```
  view::take — это range adaptor. Будучи применённым к другому range (или view) с помощью оператора |, он позволяет перебрать указанное количество первых элементов range.

Более того, если исходный контейнер был неконстантным, то применённые к нему адаптеры сохраняют возможность изменения элементов:
  ```c++
  std::vector<int> numbers = ReadNumbers();

for (int& x : numbers | view::take(5)) {
  ++x;
}
  ```
  На этом перейдём к более близким к реальности примерам.

### view::reverse
Иногда возникает необходимость перебирать элементы в обратном порядке. Адаптер view::reverse позволяет делать это без явного привлечения обратных итераторов.

Так выглядел класс Domain из задач про запрещённые домены, написанный с использованием классического C++17 (префикс std:: опущен для краткости):
  
  ```c++
  template <typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

class Domain {
public:
  explicit Domain(string_view text) {
    vector<string_view> parts = Split(text, ".");
    parts_reversed_.assign(rbegin(parts), rend(parts));
  }

  size_t GetPartCount() const {
    return parts_reversed_.size();
  }

  auto GetParts() const {
    return Range(rbegin(parts_reversed_), rend(parts_reversed_));
  }
  auto GetReversedParts() const {
    return Range(begin(parts_reversed_), end(parts_reversed_));
  }

  bool operator==(const Domain& other) const {
    return parts_reversed_ == other.parts_reversed_;
  }

private:
  vector<string> parts_reversed_;
};
  ```
  Использование ranges и, в частности, view::reverse позволяет упростить код:  
  ```c++
  class Domain {
public:
  explicit Domain(string_view text) {
    vector<string_view> parts = Split(text, ".");
    parts_.assign(begin(parts), end(parts));
  }

  size_t GetPartCount() const {
    return parts_.size();
  }

  auto GetParts() const {
    return view::all(parts_);
  }
  auto GetReversedParts() const {
    return view::reverse(parts_);
  }

  bool operator==(const Domain& other) const {
    return parts_ == other.parts_;
  }

private:
  vector<string> parts_;
};
  ```
  Здесь же использован адаптер view::all, позволяющий перебирать все элементы указанного контейнера, но при этом не владеющий его данными. Если в данном примере заменить view::all(parts_) на parts_, метод GetParts будет возвращать копию вектора parts_.  

### view::transform
Теперь перейдём к задаче «Демографические показатели». Так выглядело её решение, предложенное в одной из предыдущих задач:
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
  Первым делом здесь, конечно, хочется переписать функцию ComputeMedianAge так, чтобы она принимала range вместо двух итераторов. Но прежде давайте задумаемся: нормально ли, что настолько общий алгоритм — вычисление медианы для набора чисел — реализован исключительно для объектов типа Person, явно обращаясь к полю age? Конечно, нет!

В реальном проекте было бы логично увидеть универсальную функцию ComputeMedian, вызываемую от набора чисел:
  ```c++
  template <typename Range>
using range_value_t = rng::iter_value_t<rng::iterator_t<Range>>;
// range_value_t<Range> позволяет получить
// тип элементов, получаемых при итерировании по Range

template <rng::InputRange Range>
range_value_t<Range> ComputeMedian(Range&& range) {
  vector range_copy(rng::begin(range), rng::end(range));
  if (range_copy.empty()) {
    return {};
  }
  const auto middle = range_copy.begin() + range_copy.size() / 2;
  rng::nth_element(range_copy, middle);
  return *middle;
}
  ```
  Но как тогда с помощью этой функции реализовать ComputeMedianAge, вызываемую от набора людей? Для этого можно преобразовать набор людей в набор возрастов — с помощью адаптера view::transform:  
  ```c++
  template <rng::input_range Range>
int ComputeMedianAge(Range&& persons) {
  return ComputeMedian(
      persons
      | view::transform([](const Person& person) { return person.age; })
  );
}
  ```
  Правда, теперь при вызове ComputeMedianAge от двух итераторов, полученных из partition, нужно формировать из этих итераторов range — тут помогает класс rng::subrange:  
  
  ```c++
  AgeStats ComputeStats(vector<Person> persons) {
  //                 persons
  //                |       |
  //          females        males
  //         |       |      |     |
  //      empl.  unempl. empl.   unempl.

  auto females_end = rng::partition(
      persons,
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = rng::partition(
      rng::begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = rng::partition(
      females_end, rng::end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  return {
       ComputeMedianAge(rng::subrange(rng::begin(persons), rng::end(persons))),
       ComputeMedianAge(rng::subrange(rng::begin(persons), females_end)),
       ComputeMedianAge(rng::subrange(females_end, rng::end(persons))),
       ComputeMedianAge(rng::subrange(rng::begin(persons),
                                      employed_females_end)),
       ComputeMedianAge(rng::subrange(employed_females_end, females_end)),
       ComputeMedianAge(rng::subrange(females_end, employed_males_end)),
       ComputeMedianAge(rng::subrange(employed_males_end, rng::end(persons)))
  };
}
  ```
  Правда, теперь ComputeStats стала однозначно более громоздкой — это мы будем исправлять в следующей части.

### view::filter
Попробуем теперь отказаться от сложных манипуляций с partition в функции ComputeStats. Всё, что нужно сделать в этой функции — это вызвать ComputeMedianAge от различных наборов людей: женщин, мужчин, трудоустроенных женщин и пр.

Используем адаптер view::filter для фильтрации людей по полу и трудоустроенности:
  ```c++
  template <rng::ForwardRange Range>
AgeStats ComputeStats(Range&& persons) {
  auto females = view::filter([](const Person& person) {
    return person.gender == Gender::FEMALE; 
  });
  auto males = view::filter([](const Person& person) {
    return person.gender == Gender::MALE; 
  });
  auto employed = view::filter([](const Person& person) {
    return person.is_employed; 
  });
  auto unemployed = view::filter([](const Person& person) {
    return !person.is_employed; 
  });

  return {
       ComputeMedianAge(persons),
       ComputeMedianAge(persons | females),
       ComputeMedianAge(persons | males),
       ComputeMedianAge(persons | females | employed),
       ComputeMedianAge(persons | females | unemployed),
       ComputeMedianAge(persons | males | employed),
       ComputeMedianAge(persons | males | unemployed)
  };
}
  ```
  Здесь, например, persons | females | employed — это view, при проходе по которому будут перебираться только трудоустроенные женщины.

От небольшого дублирования кода можно избавиться, выделив генерацию адаптеров по полу и трудоустроенности в отдельные функции:
  ```c++
  auto ByGender(Gender gender) {
  return view::filter([gender](const Person& person) {
    return person.gender == gender; 
  });
}

auto ByEmployment(bool is_employed) {
  return view::filter([is_employed](const Person& person) {
    return person.is_employed == is_employed; 
  });
};

template <rng::ForwardRange Range>
AgeStats ComputeStats(Range&& persons) {
  auto females = ByGender(Gender::FEMALE);
  auto males = ByGender(Gender::MALE);
  
  auto employed = ByEmployment(true);
  auto unemployed = ByEmployment(false);

  return {
       ComputeMedianAge(persons),
       ComputeMedianAge(persons | females),
       ComputeMedianAge(persons | males),
       ComputeMedianAge(persons | females | employed),
       ComputeMedianAge(persons | females | unemployed),
       ComputeMedianAge(persons | males | employed),
       ComputeMedianAge(persons | males | unemployed)
  };
}
  ```
  Обратите внимание, что сам по себе объект males не несёт самостоятельного смысла. По сути это фильтр, который необходимо к чему-нибудь применить с помощью |, например, к контейнеру с людьми или любому другому view.
  
