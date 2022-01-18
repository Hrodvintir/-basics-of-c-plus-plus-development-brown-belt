### Введение
Многие программы создаются для пакетной обработки большого числа однородных записей. Часто при этом обработка этих записей может быть разделена на логически самостоятельные этапы, это упрощает совместную разработку новых фич и позволяет легко комбинировать эти этапы в новые конфигурации. Такую обработку можно мысленно вообразить себе как поток объектов, идущих по конвейеру от одного обработчика к другому. В этой задаче вам предстоит закончить реализацию такого конвейера.

Основная идея конвейера в том, чтобы использовать функции, "поглощающие" объекты и вызывающие друг друга:

```c++
void ConsumeObject(unique_ptr<SomeObject> obj) {
    // обработка
    ConsumeObject2(move(obj));  // теряем владение объектом
}
```

Возможен вариант архитектуры с shared_ptr, но обычно достаточно unique_ptr, и он в этой ситуации обладает преимуществами:

 - уменьшает накладные расходы

 - однозначно передает владение: предполагается, что отдав объект, обработчик уже не будет его модифицировать, что упрощает рассуждения о корректности программы

Использование классов вместо функций позволяет отвязать обработчиков друг от друга и упрощает поддержку нескольких классов обрабатываемых объектов.

### Постановка задачи
В этой задаче вам предлагается разработать конвейер обработчиков электронных писем:

```c++
struct Email {
  string from;
  string to;
  string body;
};
```
Обработчики должны реализовывать абстрактный класс Worker:
```c++
class Worker {
public:
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно реализовать этот метод
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const;

public:
  void SetNext(unique_ptr<Worker> worker);
};
```
Также реализуйте обработчики Reader, Filter, Copier и Sender, наследующие от класса Worker, а также класс PipelineBuilder, конструирующий цепочку обработчиков:  
```c++
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in);

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter);

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient);

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out);

  // строит цепочку с помощью метода Worker::SetNext и возвращает первый обработчик в построенной цепочке
  unique_ptr<Worker> Build();
};
```
Требования:

Worker

 - метод SetNext должен принимать во владение следующий обработчик из цепочки

 - метод PassOn должен вызывать метод Process следующего обработчика, если он был установлен

Reader

 - конструктор должен принимать входной поток (istream&), из которого нужно читать сообщения

 - метод Run должен читать из входного потока все e-mail'ы и передавать дальше каждый из них. Один e-mail представляется во входном потоке полями from, to, body, каждое из которых находится на своей строке.

Filter

 - конструктор должен принимать предикат типа function<bool(const Email&)>

 - метод Process должен передавать дальше только сообщения, удовлетворяющие предикату
Copier

 - конструктор должен принимать строку-адрес получателя

 - метод Process должен передавать сообщение как исходному получателю, так и указанному в конструкторе. В случае, если они совпадают, дублировать сообщение не нужно

Sender

 - конструктор должен принимать поток для записи сообщений

 - метод Process должен записывать сообщения в поток в том же формате, в котором они были прочитаны

 - Все обработчики должны передавать сообщения дальше, если обратное не требуется явно.

PipelineBuilder

 - Конструктор должен инициализировать цепочку обработчиков обработчиком Reader с переданным потоком.

 - Методы с типом возврата PipelineBuilder& должны возвращать ссылку на экземпляр класса, для которого они были вызваны, а также добавлять в конец цепочки новый обработчик, соответствующий вызванному методу (Filter для метода FilterBy, Sender для метода Send и т.д.).

 - Метод Build должен возвращать готовую цепочку обработчиков; подразумевается, что после его завершения экземпляр PipelineBuilder больше использоваться не будет

## Заготовка решения

```c++
#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // Ñ‚Ð¾Ð»ÑŒÐºÐ¾ Ð¿ÐµÑ€Ð²Ð¾Ð¼Ñƒ worker-Ñƒ Ð² Ð¿Ð°Ð¹Ð¿Ð»Ð°Ð¹Ð½Ðµ Ð½ÑƒÐ¶Ð½Ð¾ ÑÑ‚Ð¾ Ð¸Ð¼Ð¿Ð»ÐµÐ¼ÐµÐ½Ñ‚Ð¸Ñ€Ð¾Ð²Ð°Ñ‚ÑŒ
    throw logic_error("Unimplemented");
  }

protected:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ð¸ Ð´Ð¾Ð»Ð¶Ð½Ñ‹ Ð²Ñ‹Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ PassOn, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¿ÐµÑ€ÐµÐ´Ð°Ñ‚ÑŒ Ð¾Ð±ÑŠÐµÐºÑ‚ Ð´Ð°Ð»ÑŒÑˆÐµ
  // Ð¿Ð¾ Ñ†ÐµÐ¿Ð¾Ñ‡ÐºÐµ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ¾Ð²
  void PassOn(unique_ptr<Email> email) const;

public:
  void SetNext(unique_ptr<Worker> next);
};


class Reader : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Copier : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


class Sender : public Worker {
public:
  // Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
};


// Ñ€ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ ÐºÐ»Ð°ÑÑ
class PipelineBuilder {
public:
  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð² ÐºÐ°Ñ‡ÐµÑÑ‚Ð²Ðµ Ð¿ÐµÑ€Ð²Ð¾Ð³Ð¾ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ° Reader
  explicit PipelineBuilder(istream& in);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Filter
  PipelineBuilder& FilterBy(Filter::Function filter);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Copier
  PipelineBuilder& CopyTo(string recipient);

  // Ð´Ð¾Ð±Ð°Ð²Ð»ÑÐµÑ‚ Ð½Ð¾Ð²Ñ‹Ð¹ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸Ðº Sender
  PipelineBuilder& Send(ostream& out);

  // Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ð³Ð¾Ñ‚Ð¾Ð²ÑƒÑŽ Ñ†ÐµÐ¿Ð¾Ñ‡ÐºÑƒ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ñ‡Ð¸ÐºÐ¾Ð²
  unique_ptr<Worker> Build();
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
```
