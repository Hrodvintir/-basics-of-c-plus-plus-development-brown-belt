#pragma once
#include <memory>
#include <string>
#include <utility>


// ������� ����� ��������������� ���������
class Expression 
{
public:
	virtual ~Expression() = default;

	// ��������� �������� ���������
	virtual int Evaluate() const = 0;

	// ����������� ��������� ��� ������
	// ������ ���� ������ � ������, ���������� �� ����������
	virtual std::string ToString() const = 0;
};


using ExpressionPtr = std::unique_ptr<Expression>;


namespace Expr
{
    class Value : public Expression
    {
    public:
        Value(const int& value) : value_(value) {}

        int Evaluate() const override;

        std::string ToString() const override;

    private:
        int value_;
    };


    class Sum : public Expression
    {
    public:
        Sum(ExpressionPtr left, ExpressionPtr right) : left_ptr_(std::move(left)), 
                                                       right_ptr_(std::move(right)) {}

        int Evaluate() const override;

        std::string ToString() const override;

    private:
        ExpressionPtr left_ptr_, right_ptr_;
    };


    class Product : public Expression
    {
    public:
        Product(ExpressionPtr left, ExpressionPtr right) : left_ptr_(std::move(left)),
                                                           right_ptr_(std::move(right)) {}

        int Evaluate() const override;

        std::string ToString() const override;

    private:
        ExpressionPtr left_ptr_, right_ptr_;
    };
}


// ������� ��� ������������ ���������
ExpressionPtr Value(int value);
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right);
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right);


void TestCommon();
