# std::span

### Использование span для работы с C-style интерфейсами
В редких случаях возникает необходимость принять в функции набор объектов, непременно расположенных подряд в памяти (будь то стек или куча). Как правило, это обусловлено необходимостью взаимодействия с кодом на C или другим кодом с аналогичными требованиями.

Легко представить себе ситуацию, когда ваш проект постепенно переезжает с C на C++ и в процессе возникают подобные функции:
```c++
// Legacy comparator
int CompareInts(const void* lhs_ptr, const void* rhs_ptr) {
  const int lhs = *static_cast<const int*>(lhs_ptr);
  const int rhs = *static_cast<const int*>(rhs_ptr);
  if (lhs == rhs) {
    return 0;
  } else if (lhs < rhs) {
    return -1;
  } else {
    return 1;
  }
}
  
// Returns count of needles containing in sorted haystack
template <typename InputIt>
size_t CountNumbers(
    const int* haystacks, size_t count,
    InputIt needles_begin, InputIt needles_end
) {
  return count_if(
      needles_begin, needles_end,
      [haystacks, count](int needle) -> bool {
        // TODO: Rewrite me to C++ (how???)
        return bsearch(&needle, haystacks, count, sizeof(*haystacks), 
                       CompareInts);
      }
  );
}
```
Вне зависимости от размера функции, подобной CountNumbers, для читающего её сигнатуру C++-разработчика может быть неочевидным, что параметры haystack и count как-то связаны — из этого следует, что есть пространство для ошибок и неосторожных рефакторингов. Самый безобидный пример может быть таким: не разобравшийся в этой функции разработчик добавил какой-то новый числовой параметр в произвольное место — после haystack; после этого все разработчики, которые читают эту функцию, уже не видят параметры haystack и count рядом и уж точно не сочтут их связанными.

Решение этой проблемы — шаблонный класс std::span из библиотеки <span>. По сути это просто класс с двумя полями — указателем и количеством. По состоянию на 1 октября 2018 г. он доступен лишь в компиляторе clang при включении флага --std=c++2a.  

В нашем примере его можно использовать следующим образом:  
  ```c++
  #include <span>

template <typename InputIt>
size_t CountNumbers(
    span<const int> haystacks,
    InputIt needles_begin, InputIt needles_end
) {
  return count_if(
      needles_begin, needles_end,
      [haystacks](int needle) -> bool {
        return bsearch(&needle, haystacks.data(),
                       haystacks.size(), sizeof(haystacks[0]),
                       CompareInts);
      }
  );
}
  ```
  
  (Впрочем, если есть возможность, функцию CountNumbers нужно переписать, переделав на привычный binary_search из библиотеки <algorithm>. Мы рассматриваем случай, когда нужно быстро сделать сигнатуру функции более понятной, а переписать всё целиком на итераторы возможности нет.) 

 Создать объект span можно как с помощью непосредственно указателя и числа или двух указателей, так и напрямую из контейнера array или vector:  
  ```c++
  vector<int> haystacks = ReadHaystacks();
vector<int> needles = ReadNeedles();

cout << CountNumbers(
    span(haystacks.data(), haystacks.size()),
    begin(needles), end(needles)
) << endl;  // можно, но громоздко

cout << CountNumbers(
    haystacks,
    begin(needles), end(needles)
) << endl;  // так короче

  ```
  Обратите внимание, что span соотносится с vector или array так же, как string_view с string: в то время как vector владеет данными, span лишь позволяет сослаться на них.

Помимо этого, span<T> позволяет менять элементы:
  ```c++
  vector<int> numbers = {1, 2, 3, 4};
span<int> segment(numbers.data() + 1, 2);  // {2, 3}
++segment[1];
// теперь вектор содержит {1, 2, 4, 4}

  ```
  
  В первоначальном примере функция принимала span<const int>, чем гарантировала неприкосновенность исходных данных. Соответственно, span<const char> по внутреннему устройству аналогичен string_view, но второй оснащён удобными методами для работы именно со строками: find, remove_prefix и пр.  

### Использование span для работы с сериализованными данными
span может пригодиться и тем, кто не имеет необходимости взаимодействовать с C-style кодом.

Разработчикам систем, работающих с большими объёмами данных, часто приходится сталкиваться с сериализованными данными — объектами, некоторым образом упакованными в набор байт. В этом случае удобно иметь возможность сослаться на подотрезок этой памяти:
  ```c++
  #include <cstddef>
#include <fstream>
#include <iterator>
#include <vector>
#include <span>

using namespace std;

// Объекты, хранящиеся в файле в сериализованном виде
struct Object {
  // ...
};

vector<Object> DeserializeObjects(span<const char> memory);

int main() {
  ifstream bin_file("serialized_objects.bin", ios::binary);
  
  // Считываем всё содержимое файла в vector<char>.
  // char здесь используется исключительно как способ представить 1 байт
  // (можно было бы использовать std::byte из C++17,
  // но он пока не интегрирован с ifstream).
  // Здесь содержимое файла копируется в вектор,
  // но в некоторых ситуациях можно обойтись и без копирований:
  // если файл приехал непосредственно в оперативную память
  // и доступен с помощью вызова mmap, можно работать с ним напрямую
  const vector<char> buffer(
      istreambuf_iterator<char>{bin_file},
      istreambuf_iterator<char>{}
  );
  
  // Предположим, что в первом байте лежит количество объектов
  const int object_count = buffer[0];
  
  // Используем span, чтобы передать в функцию десериализации
  // конкретный блок (регион) памяти
  const vector<Object> objects =
      DeserializeObjects(span(buffer.data() + 1,
                              object_count * sizeof(Object)));
      
  ProcessObjects(objects);
 
  return 0;
}
  ```
 ### Итоги
span<T> — это удобный способ сослаться на участок памяти с объектами типа T. Два основных сценария его использования:

 - Необходимость работать с C-style интерфейсом, требующим расположение объектов подряд в памяти и, соответственно, принимающим набор объектов не в виде двух итераторов, а в виде указателя на начало диапазона и размера диапазона.

 - Работа с участками памяти, хранящими сериализованные объекты.

Напомним, что span входит в пока не принятый официально стандарт C++20 и доступен лишь в компиляторе clang при включении флага --std=c++2a. 
