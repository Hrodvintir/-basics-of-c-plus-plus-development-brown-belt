#include "Common.h"
#include "Textures.h"
#include "test_runner.h"

#include <iostream>
#include <map>

using namespace std;

class Canvas {
public:
  using ShapeId = size_t;

  void SetSize(Size size) {
    size_ = size;
  }

  ShapeId AddShape(ShapeType shape_type, Point position, Size size,
                   unique_ptr<ITexture> texture) {
    auto shape = MakeShape(shape_type);
    shape->SetPosition(position);
    shape->SetSize(size);
    shape->SetTexture(move(texture));
    return InsertShape(move(shape));
  }

  ShapeId DuplicateShape(ShapeId source_id, Point target_position) {
    auto shape = GetShapeNodeById(source_id)->second->Clone();
    shape->SetPosition(target_position);
    return InsertShape(move(shape));
  }

  void RemoveShape(ShapeId id) {
    shapes_.erase(GetShapeNodeById(id));
  }

  void MoveShape(ShapeId id, Point position) {
    GetShapeNodeById(id)->second->SetPosition(position);
  }

  void ResizeShape(ShapeId id, Size size) {
    GetShapeNodeById(id)->second->SetSize(size);
  }

  int GetShapesCount() const {
    return static_cast<int>(shapes_.size());
  }

  void Print(ostream& output) const {
    Image image(size_.height, string(size_.width, ' '));

    for (const auto& [id, shape] : shapes_) {
      shape->Draw(image);
    }

    output << '#' << string(size_.width, '#') << "#\n";
    for (const auto& line : image) {
      output << '#' << line << "#\n";
    }
    output << '#' << string(size_.width, '#') << "#\n";
  }

private:
  using Shapes = map<ShapeId, unique_ptr<IShape>>;

  Shapes::iterator GetShapeNodeById(ShapeId id) {
    auto it = shapes_.find(id);
    if (it == shapes_.end()) {
      throw out_of_range("No shape with given ID");
    }
    return it;
  }
  ShapeId InsertShape(unique_ptr<IShape> shape) {
    shapes_[current_id_] = move(shape);
    return current_id_++;
  }

  Size size_ = {};
  ShapeId current_id_ = 0;
  Shapes shapes_;
};

void TestSimple() {
  Canvas canvas;
  canvas.SetSize({5, 3});

  canvas.AddShape(ShapeType::Rectangle, {1, 0}, {3, 3}, nullptr);

  stringstream output;
  canvas.Print(output);

  const auto answer =
      "#######\n"
      "# ... #\n"
      "# ... #\n"
      "# ... #\n"
      "#######\n";

  ASSERT_EQUAL(answer, output.str());
}

void TestSmallTexture() {
  Canvas canvas;
  canvas.SetSize({6, 4});

  canvas.AddShape(ShapeType::Rectangle, {1, 1}, {4, 2},
                  MakeTextureSolid({3, 1}, '*'));

  stringstream output;
  canvas.Print(output);

  const auto answer =
      "########\n"
      "#      #\n"
      "# ***. #\n"
      "# .... #\n"
      "#      #\n"
      "########\n";

  ASSERT_EQUAL(answer, output.str());
}

void TestCow() {
  Canvas canvas;
  canvas.SetSize({18, 5});

  canvas.AddShape(ShapeType::Rectangle, {1, 0}, {16, 5}, MakeTextureCow());

  stringstream output;
  canvas.Print(output);

  // Ð—Ð´ÐµÑÑŒ ÑƒÐ¼ÐµÑÑ‚Ð½Ð¾ Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ñ‚ÑŒ ÑÑ‹Ñ€Ñ‹Ðµ Ð»Ð¸Ñ‚ÐµÑ€Ð°Ð»Ñ‹, Ñ‚.Ðº. Ð² Ñ‚ÐµÐºÑÑ‚ÑƒÑ€Ðµ ÐµÑÑ‚ÑŒ ÑÐ¸Ð¼Ð²Ð¾Ð»Ñ‹ '\'
  const auto answer =
      R"(####################)""\n"
      R"(# ^__^             #)""\n"
      R"(# (oo)\_______     #)""\n"
      R"(# (__)\       )\/\ #)""\n"
      R"(#     ||----w |    #)""\n"
      R"(#     ||     ||    #)""\n"
      R"(####################)""\n";

  ASSERT_EQUAL(answer, output.str());
}

void TestCpp() {
  Canvas canvas;
  canvas.SetSize({77, 17});

  // Ð‘ÑƒÐºÐ²Ð° "C" ÐºÐ°Ðº Ñ€Ð°Ð·Ð½Ð¾ÑÑ‚ÑŒ Ð´Ð²ÑƒÑ… ÑÐ»Ð»Ð¸Ð¿ÑÐ¾Ð², Ð¾Ð´Ð¸Ð½ Ð¸Ð· ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ñ… Ð½Ð°Ñ€Ð¸ÑÐ¾Ð²Ð°Ð½ Ñ†Ð²ÐµÑ‚Ð¾Ð¼ Ñ„Ð¾Ð½Ð°
  canvas.AddShape(ShapeType::Ellipse, {2, 1}, {30, 15},
                  MakeTextureCheckers({100, 100}, 'c', 'C'));
  canvas.AddShape(ShapeType::Ellipse, {8, 4}, {30, 9},
                  MakeTextureSolid({100, 100}, ' '));

  // Ð“Ð¾Ñ€Ð¸Ð·Ð¾Ð½Ñ‚Ð°Ð»ÑŒÐ½Ñ‹Ðµ Ñ‡Ñ‘Ñ€Ñ‚Ð¾Ñ‡ÐºÐ¸ Ð¿Ð»ÑŽÑÐ¾Ð²
  auto h1 = canvas.AddShape(ShapeType::Rectangle, {54, 7}, {22, 3},
                            MakeTextureSolid({100, 100}, '+'));
  auto h2 = canvas.DuplicateShape(h1, {30, 7});

  // Ð’ÐµÑ€Ñ‚Ð¸ÐºÐ°Ð»ÑŒÐ½Ñ‹Ðµ Ñ‡Ñ‘Ñ€Ñ‚Ð¾Ñ‡ÐºÐ¸ Ð¿Ð»ÑŽÑÐ¾Ð²
  auto v1 = canvas.DuplicateShape(h1, {62, 3});
  canvas.ResizeShape(v1, {6, 11});
  auto v2 = canvas.DuplicateShape(v1, {38, 3});

  stringstream output;
  canvas.Print(output);

  const auto answer =
    "###############################################################################\n"
    "#                                                                             #\n"
    "#            cCcCcCcCcC                                                       #\n"
    "#        CcCcCcCcCcCcCcCcCc                                                   #\n"
    "#      cCcCcCcCcCcCcCcCcCcCcC          ++++++                  ++++++         #\n"
    "#    CcCcCcCcCcCc                      ++++++                  ++++++         #\n"
    "#   CcCcCcCcC                          ++++++                  ++++++         #\n"
    "#   cCcCcCc                            ++++++                  ++++++         #\n"
    "#  cCcCcC                      ++++++++++++++++++++++  ++++++++++++++++++++++ #\n"
    "#  CcCcCc                      ++++++++++++++++++++++  ++++++++++++++++++++++ #\n"
    "#  cCcCcC                      ++++++++++++++++++++++  ++++++++++++++++++++++ #\n"
    "#   cCcCcCc                            ++++++                  ++++++         #\n"
    "#   CcCcCcCcC                          ++++++                  ++++++         #\n"
    "#    CcCcCcCcCcCc                      ++++++                  ++++++         #\n"
    "#      cCcCcCcCcCcCcCcCcCcCcC          ++++++                  ++++++         #\n"
    "#        CcCcCcCcCcCcCcCcCc                                                   #\n"
    "#            cCcCcCcCcC                                                       #\n"
    "#                                                                             #\n"
    "###############################################################################\n";

  ASSERT_EQUAL(answer, output.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  RUN_TEST(tr, TestSmallTexture);
  RUN_TEST(tr, TestCow);
  RUN_TEST(tr, TestCpp);
  return 0;
}
