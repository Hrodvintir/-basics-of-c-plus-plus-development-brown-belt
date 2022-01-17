#include "geo2d.h"
#include "game_object.h"

#include "test_runner.h"

#include <vector>
#include <memory>

using namespace std;

// ÐžÐ¿Ñ€ÐµÐ´ÐµÐ»Ð¸Ñ‚Ðµ ÐºÐ»Ð°ÑÑÑ‹ Unit, Building, Tower Ð¸ Fence Ñ‚Ð°Ðº, Ñ‡Ñ‚Ð¾Ð±Ñ‹ Ð¾Ð½Ð¸ Ð½Ð°ÑÐ»ÐµÐ´Ð¾Ð²Ð°Ð»Ð¸ÑÑŒ Ð¾Ñ‚
// GameObject Ð¸ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð¾Ð²Ñ‹Ð²Ð°Ð»Ð¸ ÐµÐ³Ð¾ Ð¸Ð½Ñ‚ÐµÑ€Ñ„ÐµÐ¹Ñ.

class Unit {
public:
  explicit Unit(geo2d::Point position);
};

class Building {
public:
  explicit Building(geo2d::Rectangle geometry);
};

class Tower {
public:
  explicit Tower(geo2d::Circle geometry);
};

class Fence {
public:
  explicit Fence(geo2d::Segment geometry);
};

// Ð ÐµÐ°Ð»Ð¸Ð·ÑƒÐ¹Ñ‚Ðµ Ñ„ÑƒÐ½ÐºÑ†Ð¸ÑŽ Collide Ð¸Ð· Ñ„Ð°Ð¹Ð»Ð° GameObject.h

bool Collide(const GameObject& first, const GameObject& second) {
}

void TestAddingNewObjectOnMap() {
  // Ð®Ð½Ð¸Ñ‚-Ñ‚ÐµÑÑ‚ Ð¼Ð¾Ð´ÐµÐ»Ð¸Ñ€ÑƒÐµÑ‚ ÑÐ¸Ñ‚ÑƒÐ°Ñ†Ð¸ÑŽ, ÐºÐ¾Ð³Ð´Ð° Ð½Ð° Ð¸Ð³Ñ€Ð¾Ð²Ð¾Ð¹ ÐºÐ°Ñ€Ñ‚Ðµ ÑƒÐ¶Ðµ ÐµÑÑ‚ÑŒ ÐºÐ°ÐºÐ¸Ðµ-Ñ‚Ð¾ Ð¾Ð±ÑŠÐµÐºÑ‚Ñ‹,
  // Ð¸ Ð¼Ñ‹ Ñ…Ð¾Ñ‚Ð¸Ð¼ Ð´Ð¾Ð±Ð°Ð²Ð¸Ñ‚ÑŒ Ð½Ð° Ð½ÐµÑ‘ Ð½Ð¾Ð²Ñ‹Ð¹, Ð½Ð°Ð¿Ñ€Ð¸Ð¼ÐµÑ€, Ð¿Ð¾ÑÑ‚Ñ€Ð¾Ð¸Ñ‚ÑŒ Ð½Ð¾Ð²Ð¾Ðµ Ð·Ð´Ð°Ð½Ð¸Ðµ Ð¸Ð»Ð¸ Ð±Ð°ÑˆÐ½ÑŽ.
  // ÐœÑ‹ Ð¼Ð¾Ð¶ÐµÐ¼ ÐµÐ³Ð¾ Ð´Ð¾Ð±Ð°Ð²Ð¸Ñ‚ÑŒ, Ñ‚Ð¾Ð»ÑŒÐºÐ¾ ÐµÑÐ»Ð¸ Ð¾Ð½ Ð½Ðµ Ð¿ÐµÑ€ÐµÑÐµÐºÐ°ÐµÑ‚ÑÑ Ð½Ð¸ Ñ Ð¾Ð´Ð½Ð¸Ð¼ Ð¸Ð· ÑÑƒÑ‰ÐµÑÑ‚Ð²ÑƒÑŽÑ‰Ð¸Ñ….
  using namespace geo2d;

  const vector<shared_ptr<GameObject>> game_map = {
    make_shared<Unit>(Point{3, 3}),
    make_shared<Unit>(Point{5, 5}),
    make_shared<Unit>(Point{3, 7}),
    make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
    make_shared<Tower>(Circle{Point{9, 4}, 1}),
    make_shared<Tower>(Circle{Point{10, 7}, 1}),
    make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
  };

  for (size_t i = 0; i < game_map.size(); ++i) {
    Assert(
      Collide(*game_map[i], *game_map[i]),
      "An object doesn't collide with itself: " + to_string(i)
    );

    for (size_t j = 0; j < i; ++j) {
      Assert(
        !Collide(*game_map[i], *game_map[j]),
        "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
      );
    }
  }

  auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
  ASSERT(!Collide(*new_warehouse, *game_map[0]));
  ASSERT( Collide(*new_warehouse, *game_map[1]));
  ASSERT(!Collide(*new_warehouse, *game_map[2]));
  ASSERT( Collide(*new_warehouse, *game_map[3]));
  ASSERT( Collide(*new_warehouse, *game_map[4]));
  ASSERT(!Collide(*new_warehouse, *game_map[5]));
  ASSERT(!Collide(*new_warehouse, *game_map[6]));

  auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
  ASSERT(!Collide(*new_defense_tower, *game_map[0]));
  ASSERT(!Collide(*new_defense_tower, *game_map[1]));
  ASSERT(!Collide(*new_defense_tower, *game_map[2]));
  ASSERT( Collide(*new_defense_tower, *game_map[3]));
  ASSERT( Collide(*new_defense_tower, *game_map[4]));
  ASSERT(!Collide(*new_defense_tower, *game_map[5]));
  ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestAddingNewObjectOnMap);
  return 0;
}
