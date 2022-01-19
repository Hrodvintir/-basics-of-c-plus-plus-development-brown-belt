#include "Common.h"

#include <sstream>

using namespace std;

// Класс, представляющий конкретное число - лист дерева выражения
class ValueExpr : public Expression {
public:
    ValueExpr(int value) : value_(value) {}

    int Evaluate() const override {
        return value_;
    }
    string ToString() const override {
        return to_string(value_);
    }

private:
    int value_;
};

// Базовый класс бинарных операций
class BinaryExpr : public Expression {
public:
    BinaryExpr(ExpressionPtr left, ExpressionPtr right) :
            left_(move(left)),
            right_(move(right)) {}

    // Здесь виртуальные функции переопределяются с ключевым словом "final".
    // Это то же самое, что и "override", но только мы запрещаем дальнейшее
    // их переопределение в наследниках. Действительно, мы хотим показать,
    // что наследники должны переопределить закрытые функции GetSymbol() и
    // EvaluateOnValues(), а сами функции ToString() и Evaluate() трогать больше
    // не нужно.
    string ToString() const final {
        ostringstream result;
        result << '(' << left_->ToString() << ')'
               << GetSymbol()
               << '(' << right_->ToString() << ')';
        return result.str();
    }
    int Evaluate() const final {
        return EvaluateOnValues(left_->Evaluate(), right_->Evaluate());
    }

private:
    // Введение этих новых вирутальных функций позволяет уменьшить дублирование
    // кода в наследниках, т.к. весь общий код собран в функциях ToString() и
    // Evaluate() базового (данного) класса. А наследники просто сообщают символ
    // операции и применяют её к переданным операндам.
    virtual char GetSymbol() const = 0;
    virtual int EvaluateOnValues(int l, int r) const = 0;

    ExpressionPtr left_;
    ExpressionPtr right_;
};

// Класс для операции умножения
class ProductExpr : public BinaryExpr {
public:
    ProductExpr(ExpressionPtr left, ExpressionPtr right) :
            BinaryExpr(move(left), move(right)) {}

private:
    char GetSymbol() const override {
        return '*';
    }
    int EvaluateOnValues(int left, int right) const override {
        return left * right;
    }
};

// Класс для операции сложения
class SumExpr : public BinaryExpr {
public:
    // Данное выражение позволяет унаследовать конструктор из базового класса.
    // Сравните с реализацией класса ProductExpr, где мы использовали "обычный"
    // конструктор, который принимает ровно те же параметры, что и конструктор
    // базового класса, и просто передаёт их ему. Ту же самую работу за нас
    // может проделать компилятор, если мы скажем ему унаследовать конструктор.
    using BinaryExpr::BinaryExpr;

private:
    char GetSymbol() const override {
        return '+';
    }
    int EvaluateOnValues(int left, int right) const override {
        return left + right;
    }
};

// Функции для формирования выражения
ExpressionPtr Value(int value) {
    return make_unique<ValueExpr>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<SumExpr>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
    return make_unique<ProductExpr>(move(left), move(right));
}
