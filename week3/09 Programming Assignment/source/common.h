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
