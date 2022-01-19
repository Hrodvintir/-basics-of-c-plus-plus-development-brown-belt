#include "geo2d.h"
#include "game_object.h"

#include "test_runner.h"

#include <vector>
#include <memory>

using namespace std;

template <typename T>
struct Collider : GameObject {
    bool Collide(const GameObject& that) const override {
        return that.CollideWith(static_cast<const T&>(*this));
    }
};

class Unit : public Collider<Unit> {
public:
    Unit(geo2d::Point position) : position_(position) {
    }

    geo2d::Point GetPosition() const { return position_; }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;

private:
    geo2d::Point position_;
};

class Building : public Collider<Building> {
public:
    Building(geo2d::Rectangle geometry)
            : geometry_(geometry)
    {
    }

    const geo2d::Rectangle& GetGeometry() const { return geometry_; }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;

private:
    geo2d::Rectangle geometry_;
};

class Tower : public Collider<Tower> {
public:
    Tower(geo2d::Circle geometry)
            : geometry_(geometry)
    {
    }

    const geo2d::Circle& GetGeometry() const { return geometry_; }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;

private:
    geo2d::Circle geometry_;
};

class Fence : public Collider<Fence> {
public:
    Fence(geo2d::Segment geometry)
            : geometry_(geometry)
    {
    }

    const geo2d::Segment& GetGeometry() const { return geometry_; }

    bool CollideWith(const Unit& that) const override;
    bool CollideWith(const Building& that) const override;
    bool CollideWith(const Tower& that) const override;
    bool CollideWith(const Fence& that) const override;

private:
    geo2d::Segment geometry_;
};

// Unit CollideWith implementation

bool Unit::CollideWith(const Unit& that) const {
    return geo2d::Collide(position_, that.position_);
}

bool Unit::CollideWith(const Building& that) const {
    return geo2d::Collide(position_, that.GetGeometry());
}

bool Unit::CollideWith(const Tower& that) const {
    return geo2d::Collide(position_, that.GetGeometry());
}

bool Unit::CollideWith(const Fence& that) const {
    return geo2d::Collide(position_, that.GetGeometry());
}

#define DEFINE_METHOD_COLLIDE_WITH(Class, ArgClass)       \
  bool Class::CollideWith(const ArgClass& that) const {   \
    return geo2d::Collide(geometry_, that.GetGeometry()); \
  }

// Building CollideWith implementation

bool Building::CollideWith(const Unit& that) const {
    return geo2d::Collide(geometry_, that.GetPosition());
}

DEFINE_METHOD_COLLIDE_WITH(Building, Building)
DEFINE_METHOD_COLLIDE_WITH(Building, Tower)
DEFINE_METHOD_COLLIDE_WITH(Building, Fence)

// Tower CollideWith implementation

bool Tower::CollideWith(const Unit& that) const {
    return geo2d::Collide(geometry_, that.GetPosition());
}

DEFINE_METHOD_COLLIDE_WITH(Tower, Building)
DEFINE_METHOD_COLLIDE_WITH(Tower, Tower)
DEFINE_METHOD_COLLIDE_WITH(Tower, Fence)

// Fence CollideWith implementation

bool Fence::CollideWith(const Unit& that) const {
    return geo2d::Collide(geometry_, that.GetPosition());
}

DEFINE_METHOD_COLLIDE_WITH(Fence, Building)
DEFINE_METHOD_COLLIDE_WITH(Fence, Tower)
DEFINE_METHOD_COLLIDE_WITH(Fence, Fence)

bool Collide(const GameObject& first, const GameObject& second) {
    return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
    // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то объекты,
    // и мы хотим добавить на неё новый, например, построить новое сдание или башню.
    // Мы можем его добавить, только если он не пересекается ни с одним из существующих.
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

//////////////////////////////////////////////////////////////////////////
/// Тесты ниже не являются частью решения. Они нужны для отладки!!!
//////////////////////////////////////////////////////////////////////////

void TestVectorProduct() {
    using geo2d::Vector;
    ASSERT_EQUAL((Vector{1, 0} * Vector{2, 0}), 0);
    ASSERT_EQUAL((Vector{1, 0} * Vector{-1, 0}), 0);
    ASSERT((Vector{1, 0} * Vector{1, 1} > 0));
    ASSERT((Vector{1, 1} * Vector{1, 0} < 0));
    ASSERT((Vector{1, 0} * Vector{-1, 1} > 0));
}

void TestPointSegmentCollide() {
    using geo2d::Point;
    using geo2d::Segment;

    ASSERT(geo2d::Collide(Point{1, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(geo2d::Collide(Point{0, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(geo2d::Collide(Point{10, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(geo2d::Collide(Point{3, 3}, Segment{{0, 0}, {10, 10}}));

    ASSERT(!geo2d::Collide(Point{-1, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(!geo2d::Collide(Point{11, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(!geo2d::Collide(Point{1, 1}, Segment{{0, 0}, {10, 0}}));
    ASSERT(!geo2d::Collide(Point{5, 5}, Segment{{0, 0}, {10, 0}}));
    ASSERT(!geo2d::Collide(Point{-5, 0}, Segment{{0, 0}, {10, 0}}));
    ASSERT(!geo2d::Collide(Point{5, -1}, Segment{{0, 0}, {10, 0}}));
}

void TestPointRectangleCollide() {
    using geo2d::Point;
    using geo2d::Rectangle;

    const Rectangle r{{0, 0}, {5, 3}};
    ASSERT(geo2d::Collide(Point{0, 0}, r));
    ASSERT(geo2d::Collide(Point{5, 0}, r));
    ASSERT(geo2d::Collide(Point{5, 3}, r));
    ASSERT(geo2d::Collide(Point{0, 3}, r));
    ASSERT(geo2d::Collide(Point{2, 2}, r));
    ASSERT(geo2d::Collide(Point{1, 3}, r));

    ASSERT(!geo2d::Collide(Point{-1, 0}, r));
    ASSERT(!geo2d::Collide(Point{0, -1}, r));
    ASSERT(!geo2d::Collide(Point{0, 4}, r));
    ASSERT(!geo2d::Collide(Point{6, 0}, r));
    ASSERT(!geo2d::Collide(Point{5, 4}, r));
    ASSERT(!geo2d::Collide(Point{6, 3}, r));
    ASSERT(!geo2d::Collide(Point{2, 8}, r));
}

void TestSegmentSegmentCollide() {
    using geo2d::Segment;
    ASSERT(geo2d::Collide(Segment{{0, 0}, {2, 2}}, Segment{{2, 0}, {0, 2}}));
    ASSERT(geo2d::Collide(Segment{{0, 0}, {2, 2}}, Segment{{2, 0}, {1, 1}}));
    ASSERT(geo2d::Collide(Segment{{0, 0}, {10, 6}}, Segment{{5, 3}, {15, 9}}));
    ASSERT(geo2d::Collide(Segment{{0, 0}, {6, 2}}, Segment{{4, 2}, {6, 0}}));
    ASSERT(geo2d::Collide(Segment{{0, 0}, {6, 2}}, Segment{{6, 2}, {6, 3}}));

    ASSERT(!geo2d::Collide(Segment{{0, 0}, {2, 2}}, Segment{{2, 0}, {1, 0}}));
    ASSERT(!geo2d::Collide(Segment{{0, 0}, {10, 6}}, Segment{{5, 4}, {15, 10}}));
    ASSERT(!geo2d::Collide(Segment{{0, 0}, {6, 2}}, Segment{{4, 1}, {6, 0}}));
}

void TestSegmentCircleCollide() {
    using geo2d::Circle;
    using geo2d::Segment;
    const Circle c{{0, 0}, 4};

    ASSERT(geo2d::Collide(c, Segment{{0, 0}, {1, 0}}));
    ASSERT(geo2d::Collide(c, Segment{{3, 1}, {10, 1}}));
    ASSERT(geo2d::Collide(c, Segment{{-5, 2}, {5, 2}}));
    ASSERT(geo2d::Collide(c, Segment{{-5, 3}, {5, 3}}));
    ASSERT(geo2d::Collide(c, Segment{{-5, 4}, {5, 4}}));
    ASSERT(geo2d::Collide(c, Segment{{3, 1}, {4, 5}}));
    ASSERT(geo2d::Collide(c, Segment{{5, 0}, {-2, 4}}));

    ASSERT(!geo2d::Collide(c, Segment{{4, 1}, {4, 5}}));
    ASSERT(!geo2d::Collide(c, Segment{{-5, 5}, {5, 5}}));
    ASSERT(!geo2d::Collide(c, Segment{{4, 4}, {5, 4}}));
    ASSERT(!geo2d::Collide(Circle{{10, 7}, 1}, Segment{{7, 3}, {9, 8}}));
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestAddingNewObjectOnMap);
    RUN_TEST(tr, TestVectorProduct);
    RUN_TEST(tr, TestPointSegmentCollide);
    RUN_TEST(tr, TestPointRectangleCollide);
    RUN_TEST(tr, TestSegmentSegmentCollide);
    RUN_TEST(tr, TestSegmentCircleCollide);
    return 0;
}
