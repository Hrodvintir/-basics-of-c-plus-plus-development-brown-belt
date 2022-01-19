#pragma once

#include <memory>
#include <string>
#include <vector>

struct Point {
  int x;
  int y;
};

struct Size {
  int width;
  int height;
};

// ÐŸÑ€Ð¾Ð²ÐµÑ€ÑÐµÑ‚, ÑÐ¾Ð´ÐµÑ€Ð¶Ð¸Ñ‚ÑÑ Ð»Ð¸ Ð·Ð°Ð´Ð°Ð½Ð½Ð°Ñ Ñ‚Ð¾Ñ‡ÐºÐ° Ð² ÑÐ»Ð»Ð¸Ð¿ÑÐµ Ð·Ð°Ð´Ð°Ð½Ð½Ð¾Ð³Ð¾ Ñ€Ð°Ð·Ð¼ÐµÑ€Ð°
// Ð¡Ñ‡Ð¸Ñ‚Ð°ÐµÑ‚ÑÑ, Ñ‡Ñ‚Ð¾ ÑÐ»Ð»Ð¸Ð¿Ñ Ð²Ð¿Ð¸ÑÐ°Ð½ Ð² Ð¿Ñ€ÑÐ¼Ð¾ÑƒÐ³Ð¾Ð»ÑŒÐ½Ð¸Ðº Ñ Ð²ÐµÑ€ÑˆÐ¸Ð½Ð°Ð¼Ð¸ Ð² Ñ‚Ð¾Ñ‡ÐºÐ°Ñ… (0, 0) Ð¸
// (size.width, size.height)
inline bool IsPointInEllipse(Point p, Size size) {
  // ÐÐ¾Ñ€Ð¼Ð¸Ñ€ÑƒÐµÐ¼ ÐºÐ¾Ð¾Ñ€Ð´Ð¸Ð½Ð°Ñ‚Ñ‹ Ñ‚Ð¾Ñ‡ÐºÐ¸ Ð² Ð´Ð¸Ð°Ð¿Ð°Ð·Ð¾Ð½ (-1, 1)
  // Ð¡Ð´Ð²Ð¸Ð³ Ð½Ð° 0.5 Ð½ÑƒÐ¶ÐµÐ½, Ñ‡Ñ‚Ð¾Ð±Ñ‹ ÑÑ‡Ð¸Ñ‚Ð°Ñ‚ÑŒ Ñ€Ð°ÑÑÑ‚Ð¾ÑÐ½Ð¸Ðµ Ð´Ð¾ Ñ†ÐµÐ½Ñ‚Ñ€Ð° Ð¿Ð¸ÐºÑÐµÐ»Ñ, Ñ‚Ð°Ðº
  // Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÑ‚ÑÑ Ð±Ð¾Ð»ÐµÐµ ÐºÑ€Ð°ÑÐ¸Ð²Ð°Ñ Ñ„Ð¾Ñ€Ð¼Ð°
  double x = (p.x + 0.5) / (size.width / 2.0) - 1;
  double y = (p.y + 0.5) / (size.height / 2.0) - 1;
  // ÐŸÑ€Ð¾Ð²ÐµÑ€ÑÐµÐ¼, Ð»ÐµÐ¶Ð¸Ñ‚ Ð»Ð¸ Ñ‚Ð¾Ñ‡ÐºÐ° Ð² ÐµÐ´Ð¸Ð½Ð¸Ñ‡Ð½Ð¾Ð¼ ÐºÑ€ÑƒÐ³Ðµ
  return x * x + y * y <= 1;
}

// Ð˜Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ðµ. ÐŸÐ¸ÐºÑÐµÐ»Ð¸ ÑÑ‚Ð¾ ÑÐ¸Ð¼Ð²Ð¾Ð»Ñ‹.
// ÐŸÐµÑ€Ð²Ñ‹Ð¹ Ð¸Ð½Ð´ÐµÐºÑ (Ð¿Ð¾ std::vector) - ÑÑ‚Ñ€Ð¾ÐºÐ¸ Ð¸Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ, ÐºÐ¾Ð¾Ñ€Ð´Ð¸Ð½Ð°Ñ‚Ð° y
// Ð’Ñ‚Ð¾Ñ€Ð¾Ð¹ Ð¸Ð½Ð´ÐµÐºÑ (Ð¿Ð¾ std::string) - ÑÑ‚Ð¾Ð»Ð±Ñ†Ñ‹ Ð¸Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ, ÐºÐ¾Ð¾Ñ€Ð´Ð¸Ð½Ð°Ñ‚Ð° x
// ÐŸÑ€ÐµÐ´Ð¿Ð¾Ð»Ð°Ð³Ð°ÐµÑ‚ÑÑ, Ñ‡Ñ‚Ð¾ Ð´Ð»Ð¸Ð½Ð° Ð²ÑÐµÑ… ÑÑ‚Ñ€Ð¾Ðº Ð¾Ð´Ð¸Ð½Ð°ÐºÐ¾Ð²Ð°
using Image = std::vector<std::string>;

// ÐŸÐ¾Ð´Ð´ÐµÑ€Ð¶Ð¸Ð²Ð°ÐµÐ¼Ñ‹Ðµ Ð²Ð¸Ð´Ñ‹ Ñ„Ð¸Ð³ÑƒÑ€: Ð¿Ñ€ÑÐ¼Ð¾ÑƒÐ³Ð¾Ð»ÑŒÐ½Ð¸Ðº Ð¸ ÑÐ»Ð»Ð¸Ð¿Ñ
enum class ShapeType { Rectangle, Ellipse };

// Ð˜Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ Ñ‚ÐµÐºÑÑ‚ÑƒÑ€Ñ‹
class ITexture {
public:
  virtual ~ITexture() = default;

  // Ð’Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ñ€Ð°Ð·Ð¼ÐµÑ€ Ñ…Ñ€Ð°Ð½Ð¸Ð¼Ð¾Ð³Ð¾ Ð¸Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ñ
  virtual Size GetSize() const = 0;
  // Ð’Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ñ…Ñ€Ð°Ð½Ð¸Ð¼Ð¾Ðµ Ð¸Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ðµ
  virtual const Image& GetImage() const = 0;
};

// Ð˜Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ Ñ„Ð¸Ð³ÑƒÑ€Ñ‹
class IShape {
public:
  virtual ~IShape() = default;

  // Ð’Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ Ñ‚Ð¾Ñ‡Ð½ÑƒÑŽ ÐºÐ¾Ð¿Ð¸ÑŽ Ñ„Ð¸Ð³ÑƒÑ€Ñ‹.
  // Ð•ÑÐ»Ð¸ Ñ„Ð¸Ð³ÑƒÑ€Ð° ÑÐ¾Ð´ÐµÑ€Ð¶Ð¸Ñ‚ Ñ‚ÐµÐºÑÑ‚ÑƒÑ€Ñƒ, Ñ‚Ð¾ ÑÐ¾Ð·Ð´Ð°Ð½Ð½Ð°Ñ ÐºÐ¾Ð¿Ð¸Ñ ÑÐ¾Ð´ÐµÑ€Ð¶Ð¸Ñ‚ Ñ‚Ñƒ Ð¶Ðµ ÑÐ°Ð¼ÑƒÑŽ
  // Ñ‚ÐµÐºÑÑ‚ÑƒÑ€Ñƒ. Ð¤Ð¸Ð³ÑƒÑ€Ð° Ð¸ ÐµÑ‘ ÐºÐ¾Ð¿Ð¸Ñ ÑÐ¾Ð²Ð¼ÐµÑÑ‚Ð½Ð¾ Ð²Ð»Ð°Ð´ÐµÑŽÑ‚ ÑÑ‚Ð¾Ð¹ Ñ‚ÐµÐºÑÑ‚ÑƒÑ€Ð¾Ð¹.
  virtual std::unique_ptr<IShape> Clone() const = 0;

  virtual void SetPosition(Point) = 0;
  virtual Point GetPosition() const = 0;

  virtual void SetSize(Size) = 0;
  virtual Size GetSize() const = 0;

  virtual void SetTexture(std::shared_ptr<ITexture>) = 0;
  virtual ITexture* GetTexture() const = 0;

  // Ð Ð¸ÑÑƒÐµÑ‚ Ñ„Ð¸Ð³ÑƒÑ€Ñƒ Ð½Ð° ÑƒÐºÐ°Ð·Ð°Ð½Ð½Ð¾Ð¼ Ð¸Ð·Ð¾Ð±Ñ€Ð°Ð¶ÐµÐ½Ð¸Ð¸
  virtual void Draw(Image&) const = 0;
};

// Ð¡Ð¾Ð·Ð´Ð°Ñ‘Ñ‚ Ñ„Ð¸Ð³ÑƒÑ€Ñƒ Ð·Ð°Ð´Ð°Ð½Ð½Ð¾Ð³Ð¾ Ñ‚Ð¸Ð¿Ð°. Ð’Ð°Ð¼ Ð½ÑƒÐ¶Ð½Ð¾ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð¾Ð²Ð°Ñ‚ÑŒ ÑÑ‚Ñƒ Ñ„ÑƒÐ½ÐºÑ†Ð¸ÑŽ.
std::unique_ptr<IShape> MakeShape(ShapeType shape_type);