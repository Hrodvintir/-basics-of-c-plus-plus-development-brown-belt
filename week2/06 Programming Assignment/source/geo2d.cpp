#include "geo2d.h"

#include <algorithm>
#include <cmath>

namespace geo2d {

Rectangle::Rectangle(Point p1, Point p2)
  : x_left(std::min(p1.x, p2.x))
  , x_right(std::max(p1.x, p2.x))
  , y_bottom(std::min(p1.y, p2.y))
  , y_top(std::max(p1.y, p2.y))
{
}

template <typename T>
T Sqr(T x) { return x * x; }

template <typename T>
T Sign(T x) {
  return x != 0 ? x / abs(x) : 0;
}

uint64_t DistanceSquared(Point p1, Point p2) {
  int64_t diff_x = p1.x - p2.x;
  int64_t diff_y = p1.y - p2.y;

  uint64_t result = Sqr(diff_x);
  result += Sqr(diff_y);

  return result;
}

int64_t operator * (Vector lhs, Vector rhs) {
  return static_cast<int64_t>(lhs.x) * rhs.y - static_cast<int64_t>(rhs.x) * lhs.y;
}

int64_t ScalarProduct(Vector lhs, Vector rhs) {
  return static_cast<int64_t>(lhs.x) * rhs.x + static_cast<int64_t>(lhs.y) * rhs.y;
}

bool Collide(Point p, Point q) {
  return p.x == q.x && p.y == q.y;
}

bool Collide(Point p, Segment s) {
  const Vector v1{s.p1, p};
  const Vector v2{s.p2, p};

  return ScalarProduct(v1, Vector{s.p1, s.p2}) >= 0 &&
         ScalarProduct(v2, Vector{s.p2, s.p1}) >= 0 &&
         v1 * Vector{s.p1, s.p2} == 0;
}

bool Collide(Point p, Rectangle r) {
  return r.Left() <= p.x && p.x <= r.Right() &&
         r.Bottom() <= p.y && p.y <= r.Top();
}

bool Collide(Point p, Circle c) {
  return DistanceSquared(p, c.center) <= Sqr<uint64_t>(c.radius);
}

bool Collide(Segment s1, Segment s2);
bool Collide(Circle c, Segment s);

bool Collide(Rectangle r, Point p) { return Collide(p, r); }
bool Collide(Rectangle r, Segment s) {
  return Collide(s.p1, r) ||
         Collide(s.p2, r) ||
         Collide(s, Segment{r.BottomLeft(), r.BottomRight()}) ||
         Collide(s, Segment{r.BottomRight(), r.TopRight()}) ||
         Collide(s, Segment{r.TopRight(), r.TopLeft()}) ||
         Collide(s, Segment{r.TopLeft(), r.BottomLeft()});
}

bool Collide(Rectangle r1, Rectangle r2) {
  auto max_left = std::max(r1.Left(), r2.Left());
  auto min_right = std::min(r1.Right(), r2.Right());
  auto max_bottom = std::max(r1.Bottom(), r2.Bottom());
  auto min_top = std::min(r1.Top(), r2.Top());
  return min_right >= max_left && min_top >= max_bottom;
}

bool Collide(Rectangle r, Circle c) {
  return Collide(c.center, r) ||
         Collide(c, Segment{r.BottomLeft(), r.BottomRight()}) ||
         Collide(c, Segment{r.BottomRight(), r.TopRight()}) ||
         Collide(c, Segment{r.TopRight(), r.TopLeft()}) ||
         Collide(c, Segment{r.TopLeft(), r.BottomLeft()});
}

bool Collide(Segment s, Point p) { return Collide(p, s); }
bool Collide(Segment s1, Segment s2) {
  const Rectangle first_bounding_box(s1.p1, s1.p2);
  const Rectangle second_bounding_box(s2.p1, s2.p2);
  if (!Collide(first_bounding_box, second_bounding_box)) {
    return false;
  }

  const Vector v1{s1.p1, s1.p2};
  const Vector v2{s2.p1, s2.p2};

  return Sign(v1 * Vector{s1.p1, s2.p1}) * Sign(v1 * Vector{s1.p1, s2.p2}) <= 0 &&
         Sign(v2 * Vector{s2.p1, s1.p1}) * Sign(v2 * Vector{s2.p1, s1.p2}) <= 0;
}

bool Collide(Segment s, Rectangle r) { return Collide(r, s); }
bool Collide(Segment s, Circle c) { return Collide(c, s); }

bool Collide(Circle c, Point p) { return Collide(p, c); }
bool Collide(Circle c, Rectangle r) { return Collide(r, c); }
bool Collide(Circle c, Segment s) {
  if (
    ScalarProduct(Vector{s.p1, s.p2}, Vector{s.p1, c.center}) >= 0 &&
    ScalarProduct(Vector{s.p2, s.p1}, Vector{s.p2, c.center}) >= 0
    ) {
    // Ð’Ñ‹ÑÐ¾Ñ‚Ð° Ñ‚Ñ€ÐµÑƒÐ³Ð¾Ð»ÑŒÐ½Ð¸ÐºÐ° (s.p1, s.p2, c.center), Ð¿Ñ€Ð¾Ð²ÐµÐ´Ñ‘Ð½Ð½Ð°Ñ Ð¸Ð· c.center,
    // Ð¿Ð¾Ð¿Ð°Ð´Ð°ÐµÑ‚ Ð½Ð° Ð¾Ñ‚Ñ€ÐµÐ·Ð¾Ðº (s.p1, s.p2).

    // Ð£Ð´Ð²Ð¾ÐµÐ½Ð½Ð°Ñ Ð¿Ð»Ð¾Ñ‰Ð°Ð´ÑŒ Ñ‚Ñ€ÐµÑƒÐ³Ð¾Ð»ÑŒÐ½Ð¸ÐºÐ° (s.p1, s.p2, c.center) Ñ€Ð°Ð²Ð½Ð° Ð¼Ð¾Ð´ÑƒÐ»ÑŽ
    // Ð²ÐµÐºÑ‚Ð¾Ñ€Ð½Ð¾Ð³Ð¾ Ð¿Ñ€Ð¾Ð¸Ð·Ð²ÐµÐ´ÐµÐ½Ð¸Ñ Ð½Ð¸Ð¶Ðµ, Ð¾Ð±Ð¾Ð·Ð½Ð°Ñ‡Ð¸Ð¼ ÐµÑ‘ 2S. Ð’Ñ‹ÑÐ¾Ñ‚Ð° ÑÑ‚Ð¾Ð³Ð¾ Ñ‚Ñ€ÐµÑƒÐ³Ð¾Ð»ÑŒÐ½Ð¸ÐºÐ°,
    // Ð¿Ñ€Ð¾Ð²ÐµÐ´Ñ‘Ð½Ð½Ð°Ñ Ð¸Ð· c.center, Ñ€Ð°Ð²Ð½Ð° 2S / |s.p1, s.p2|. Ð§Ñ‚Ð¾Ð±Ñ‹ Ð¾ÑÑ‚Ð°Ñ‚ÑŒÑÑ Ð² Ñ†ÐµÐ»Ñ‹Ñ…
    // Ñ‡Ð¸ÑÐ»Ð°Ñ…, Ð²Ð¾Ð·Ð²ÐµÐ´Ñ‘Ð¼ ÑÑ€Ð°Ð²Ð½Ð¸Ð²Ð°ÐµÐ¼Ñ‹Ðµ Ð²ÐµÐ»Ð¸Ñ‡Ð¸Ð½Ñ‹ Ð² ÐºÐ²Ð°Ð´Ñ€Ð°Ñ‚ Ð¸ ÑÑ€Ð°Ð²Ð½Ð¸Ð¼ (2S)^ 2 Ñ
    // R^2 * |s.p1, s.p2|^2
    uint64_t double_triangle_square = abs(Vector{s.p1, s.p2} * Vector{s.p1, c.center});
    return Sqr(double_triangle_square) <= Sqr<uint64_t>(c.radius) * DistanceSquared(s.p1, s.p2);
  } else {
    auto d = std::min(DistanceSquared(c.center, s.p1), DistanceSquared(c.center, s.p2));
    return d <= Sqr<uint64_t>(c.radius);
  }
}

bool Collide(Circle c1, Circle c2) {
  return DistanceSquared(c1.center, c2.center) <= Sqr<uint64_t>(c1.radius + c2.radius);
}

}
