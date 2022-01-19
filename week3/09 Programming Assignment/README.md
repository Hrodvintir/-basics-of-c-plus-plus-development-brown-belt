В этой задаче мы воспользуемся умным указателем unique_ptr для управления временем жизни дерева полиморфных объектов. А конкретнее, будем работать с деревом арифметического выражения. Узлами дерева будут числа и операции.

Например, выражение "2*(3+4)" будет представлено вот таким деревом:

![alt text](https://github.com/Hrodvintir/-basics-of-c-plus-plus-development-brown-belt/blob/main/week3/09%20Programming%20Assignment/source/EeaWw_ZJEeilxxL_ZeRz_A_75fbca3acbde22cfd3e622279aadab79_tree.png)

В программе узлы дерева представляются объектами типов, унаследованных от интерфейса Expression, который объявлен в файле Common.h. У интерфейса есть два метода:

 - Evaluate() возвращает численное значение выражения. Для нашего примера это будет 14.

 - ToString() форматирует выражение как строку. Для простоты реализации, чтобы не учитывать приоритеты операций, каждый узел берётся в скобки. То есть для нашего примера этот метод вернёт "(2)*((3)+(4))".

Так как Expression — это абстрактный класс, работать с ним можно только через указатель или ссылку. Чтобы не заниматься ручным управлением памятью, будем использовать умный указатель unique_ptr. Чтобы не загромождать код выражениями unique_ptr<Expression>, в файле Common.h для этого выражения предоставлен синоним ExpressionPtr.
  
  Реализуйте функции, которые позволяют создавать такое дерево выражения. Они объявлены в файле Common.h, который приведён ниже:

 - Value() возвращает дерево из одного узла, представляющего целое число.

 - Sum() возвращает новое дерево, которое представляет сумму двух переданных выражений.

 - Product() возвращает новое дерево, которое представляет произведение двух переданных выражений.

Таким образом, следующий код создаст дерево для выражения "2*(3+4)":
  
  ```c++ 
  Product(Value(2), Sum(Value(3), Value(4)));
  ```
  На проверку пришлите cpp-файл, который

 - подключает заголовочный файл Common.h

 - содержит реализацию функций Product(), Value() и Sum()

Заготовка решения содержится в файле main.cpp
  ```c++
  #include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}
  ```
  ### Header file ### 
  ```c++
  #pragma once

#include <memory>
#include <string>

// Ð‘Ð°Ð·Ð¾Ð²Ñ‹Ð¹ ÐºÐ»Ð°ÑÑ Ð°Ñ€Ð¸Ñ„Ð¼ÐµÑ‚Ð¸Ñ‡ÐµÑÐºÐ¾Ð³Ð¾ Ð²Ñ‹Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ
class Expression {
public:
  virtual ~Expression() = default;

  // Ð’Ñ‹Ñ‡Ð¸ÑÐ»ÑÐµÑ‚ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ðµ Ð²Ñ‹Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ
  virtual int Evaluate() const = 0;

  // Ð¤Ð¾Ñ€Ð¼Ð°Ñ‚Ð¸Ñ€ÑƒÐµÑ‚ Ð²Ñ‹Ñ€Ð°Ð¶ÐµÐ½Ð¸Ðµ ÐºÐ°Ðº ÑÑ‚Ñ€Ð¾ÐºÑƒ
  // ÐšÐ°Ð¶Ð´Ñ‹Ð¹ ÑƒÐ·ÐµÐ» Ð±ÐµÑ€Ñ‘Ñ‚ÑÑ Ð² ÑÐºÐ¾Ð±ÐºÐ¸, Ð½ÐµÐ·Ð°Ð²Ð¸ÑÐ¸Ð¼Ð¾ Ð¾Ñ‚ Ð¿Ñ€Ð¸Ð¾Ñ€Ð¸Ñ‚ÐµÑ‚Ð°
  virtual std::string ToString() const = 0;
};

using ExpressionPtr = std::unique_ptr<Expression>;

// Ð¤ÑƒÐ½ÐºÑ†Ð¸Ð¸ Ð´Ð»Ñ Ñ„Ð¾Ñ€Ð¼Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ñ Ð²Ñ‹Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ
ExpressionPtr Value(int value);
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right);
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right);
  
  ```
  ### Подсказки ###
Для этой задачи есть набор подсказок, которые должны помочь вам с решением. Если вам не удаётся решить задачу и вы чувствуете, что у вас кончились идеи, вы можете ими воспользоваться. Но сначала обязательно попробуйте решить задачу без подсказок.
  
 <object data="https://github.com/Hrodvintir/-basics-of-c-plus-plus-development-brown-belt/blob/main/week3/09%20Programming%20Assignment/source/V6IquPbhEei5Kg7DUflKxA_57ce4310f6e111e882cf5b014df25ce8__________-1.pdf" width="1000" height="1000" type='application/pdf'></object>



