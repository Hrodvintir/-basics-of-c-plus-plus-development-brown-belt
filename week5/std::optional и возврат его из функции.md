# std::optional и возврат его из функции
### Пример: валидация объекта
Представьте себе функцию, валидирующую некоторый объект — например, дату — и возвращающую не просто bool с результатом валидации, а ещё и причину непрохождения валидации:

```c++
struct Date {
  int year, month, day;
};

enum class InvalidDateReason {
  NONPOSITIVE_YEAR,
  MONTH_TOO_SMALL,
  MONTH_TOO_BIG,
  // ...
};

/* ??? */ CheckDate(const Date& date) {
  if (year < 1) {
    // InvalidDateReason::NONPOSITIVE_YEAR
  else if (month < 1) {
    // InvalidDateReason::MONTH_TOO_SMALL
  } else if (month > 12) {
    // InvalidDateReason::MONTH_TOO_BIG
  } // ...
  
  // Если не вернулись с ошибкой, вернуть «успех»
}
```
Каким должно быть возвращаемое значение функции? Первое, что приходит в голову после прочтения постановки задачи («не просто bool, а ещё и причину»), — это пара из bool и InvalidDateReason:  
```c++
pair<bool, InvalidDateReason> CheckDate(const Date& date) {
  if (date.year < 1) {
    return {false, InvalidDateReason::NONPOSITIVE_YEAR};
  } else if (date.month < 1) {
    return {false, InvalidDateReason::MONTH_TOO_SMALL};
  } else if (date.month > 12) {
    return {false, InvalidDateReason::MONTH_TOO_BIG};
  } // ...
  
  return {true, {}};
}
```
Функцию мы как-то написали, но теперь поставим себя на место разработчика, который хочет эту функцию использовать и видит только сигнатуру (так обычно и бывает, например, в подсказках IDE: заставлять людей читать реализацию функции часто считается неприличным):  
```c++
pair<bool, InvalidDateReason> CheckDate(const Date& date);
```
К этой функции могут возникнуть следующие вопросы:

 - bool в возвращаемом значении означает, что дата хорошая или плохая? (Тут помогло бы использовать вместо пары структуру, в которой bool-поле называлось бы is_date_ok, но см. следующий пункт.)

 - Если дата хорошая, содержится ли какая-то информация в InvalidDateReason? Не надо ли её проверить на всякий случай? Внутренний параноик подсказывает, что в функции с такой сигнатурой эта проблема рано или поздно случится: либо на вызывающей стороне кто-то будет смотреть только на InvalidDateReason и находить нам валидное значение при корректной дате, либо при доработке функции кто-то решит добавить причину InvalidDateReason::OK и забудет про bool.

Итак, нужно чуть критичнее отнестись к возвращаемому значению функции. Формально говоря, в нём находится либо false и причина отклонения, либо true без дополнительной информации. Можно договориться, что отсутствие причины отклонения — это автоматически успех, и сформулировать так: если дата не прошла валидацию, функция возвращает причину отклонения, иначе не возвращает ничего.

### Возврат optional из функции
Для того, чтобы хранить «T или ничего», отлично подходит шаблон optional<T> из библиотеки <optional>(появившийся в C++17), в данном случае — optional<FailReason>. Заметим, что FailReason неявно приводится к optional<FailReason>, а для инициализации пустого optional используется специальный объект nullopt. Перепишем функцию с учётом этого:
  ```c++
  optional<InvalidDateReason> CheckDate(const Date& date) {
  if (date.year < 1) {
    return InvalidDateReason::NONPOSITIVE_YEAR;
  } else if (date.month < 1) {
    return InvalidDateReason::MONTH_TOO_SMALL;
  } else if (date.month > 12) {
    return InvalidDateReason::MONTH_TOO_BIG;
  } // ...
  
  return nullopt;
}
  ```
  Даже код самой функции стал чище и понятнее. Возникает, однако, один вопрос: раз уж у optional есть конструктор по умолчанию, создающий пустой optional, зачем использовать nullopt? Нельзя ли просто использовать return {}?  
  
  ```c++
  optional<InvalidDateReason> CheckDate(const Date& date) {
  if (date.year < 1) {
    return InvalidDateReason::NONPOSITIVE_YEAR;
  } else if (date.month < 1) {
    return InvalidDateReason::MONTH_TOO_SMALL;
  } else if (date.month > 12) {
    return InvalidDateReason::MONTH_TOO_BIG;
  } // ...
  
  return {};
}
  ```
  Такой код будет компилироваться и корректно работать, но nullopt ввели неспроста. Представьте себе следующий рефакторинг этой функции:

Будем возвращать из функции дополнительную информацию не только для некорректных дат, но и для корректных.

InvalidDateReason переименуем в DateDescription и добавим в enum «хорошие» причины.

Теперь функция будет возвращать не optional, а просто DateDescription, но при этом в рефакторинг может закрасться ошибка: в одной из веток функции останется return {}.
  ```c++
  DateDescription CheckDate(const Date& date) {
  if (date.year < 1) {
    return DateDescription::NONPOSITIVE_YEAR;
  } else if (date.month < 1) {
    return DateDescription::MONTH_TOO_SMALL;
  } else if (date.month > 12) {
    return DateDescription::MONTH_TOO_BIG;
  } // ...
  
  if (IsWeekend(date)) {
    return DateDescription::WEEKEND;
  } else {
    return {};
  }
 }
  ```
  Такой код всё равно скомпилируется, но при этом return {} будет означать «вернуть DateDescription со значением по умолчанию» — а это уже будет не «отсутствие объекта», а например, DateDescription::NONPOSITIVE_YEAR.

Итак, практика возвращать nullopt страхует от случайных ошибок при рефакторинге функции.

### Обработка optional, полученного из функции
Кратко разберём, как можно коротко и понятно использовать полученный из функции optional. Более полную информацию можно найти на cppreference.com.

Как правило, при вызове функции подобного рода нужно проверить успешность валидации и в случае проблем куда-то залогировать причину. Здесь помогут два метода шаблона optional:

 - operator bool. Любой объект optional может неявно приводиться к bool, что позволяет использовать его непосредственно в условии if: если optional содержит значение, он приведётся к true, в противном случае — к false.

 - operator*. Получить значение из optional (при его наличии) можно с помощью компактного синтаксиса — оператора *.

По сути эти операторы позволяют использовать optional как указатель: простым if проверяем, есть ли значение, и если есть — получаем его «разыменованием». Итого, код вызова CheckDate и использования результата может выглядеть следующим образом:
```c++
  if (const auto invalid_date_reason = CheckDate(date)) {
  LogFailure(date, *invalid_date_reason);
  return;
}
  ```
  Здесь использован удобный синтаксис инициализации переменной непосредственно в условии if. Он работает следующим образом:

 - Переменная invalid_date_reason создаётся и инициализируется с помощью вызова CheckDate(date). Область видимости переменной — весь if вместе с последующими else if и else.

 - Переменная invalid_date_reason проверяется на истинность, в данном случае — с помощью упомянутого выше operator bool.

### Эффективность возврата через optional
Коротко обсудим, насколько эффективен возврат тяжёлых объектов через optional.

Рассмотрим такой код:
  ```c++
  optional<HeavyObject> ReturnTemporary() {
  return HeavyObject(/* ... */);
}

optional<HeavyObject> ReturnVariable() {
  HeavyObject heavy(/* ... */);
  return heavy;
}
  ```
  По сути в обоих случаях при возврате из функции осуществляется неявный вызов конструктора optional:  
```c++
  optional<HeavyObject> ReturnTemporary() {
  return optional<HeavyObject>(HeavyObject(/* ... */));
}

optional<HeavyObject> ReturnVariable() {
  HeavyObject heavy(/* ... */);
  return optional<HeavyObject>(heavy);
}
  ```
 В функции ReturnTemporary ожидаемо происходит следующее:

 - optional инициализируется временным объектом, так что HeavyObject перемещается в объект optional.

 - Из функции возвращается временный объект optional, так что срабатывает copy elision.

В функции ReturnVariable второй пункт остаётся в силе, но в первом происходит инициализация с помощью обычной переменной, которая — при отсутствии move — может лишь честно скопироваться. Тем не менее, практика показывает, что в варианте ReturnVariable при использовании простой версии return — return heavy; — современные компиляторы с лёгкостью избегают лишних копирований, даже если речь идёт об объектах с большим количеством данных на стеке, для которых перемещение не срабатывает.

### Передача optional в функцию
Обсудим ещё одно применение optional в контексте функций.

При необходимости передать в функцию простой объект — например, число — со значением по умолчанию, сигнализирующим об отсутствии числа, это значение по умолчанию может выглядеть неестественно. Рассмотрим, например, функцию, печатающую содержимое числового вектора, с дополнительным необязательным аргументом — количеством первых чисел, которым нужно ограничить вывод: 
  ```c++
  void PrintNumbers(const vector<int>& numbers, int first_element_count = -1) {
  for (int i = 0;
       i < static_cast<int>(numbers.size()) && (first_element_count == -1 || i < first_element_count);
       ++i) {
    cout << numbers[i] << ' ';
  }
}
  ```
Во-первых, сразу возникли проблемы с беззнаковостью. numbers.size() имеет беззнаковый тип size_t и поэтому может с ходу (то есть без предупреждений компилятора) сравниваться лишь с беззнаковыми числами. Но если сделать переменную цикла i беззнаковой, нужно делать беззнаковым и first_element_count — чего сделать нельзя, потому что он может иметь специальное ничего не значащее значение -1. Можно, конечно, вместо -1 использовать numeric_limits<size_t>::max() (как сделано для string::npos), но тогда мы в качестве бонуса получим ещё и громоздкое значение по умолчанию.

Наконец, бывают ситуации, когда специальное «невалидное» значение по умолчанию для числового параметра придумать просто нельзя — например, если это число является хешом и легко может быть совершенно произвольным набором из 64 бит.

В этих ситуациях тоже помогает optional:
  ```c++
  void PrintNumbers(const vector<int>& numbers,
                  optional<size_t> first_element_count = nullopt) {
  for (size_t i = 0;
       i < numbers.size() && (!first_element_count || i < *first_element_count);
       ++i) {
    cout << numbers[i] << ' ';
  }
}
  ```
Вариант более изящного кода — вычислить границу один раз:  
  ```c++
  void PrintNumbers(const vector<int>& numbers, optional<size_t> 
                  first_element_count = nullopt) {
  const size_t bound = min(numbers.size(),
                           first_element_count.value_or(
                               numeric_limits<size_t>::max()));
  for (size_t i = 0; i < bound; ++i) {
    cout << numbers[i] << ' ';
  }
}
  ```
                          
С учётом того, что optional<size_t> хранит лишь bool и size_t, его спокойно можно передавать по значению.

### Итоги
Шаблон optional позволяет без лишних костылей реализовать семантику «храню объект или ничего». Это актуально как для возврата значений из функций, так и для передачи параметров и даже для обычных переменных. Если вы поймали себя на том, что пытаетесь выдумать нетривиальное значение по умолчанию для числовой переменной, рассмотрите возможность оборачивания её в optional.

