#include <limits>
#include <random>
#include <unordered_set>

using namespace std;

using CoordType = int;

struct Point3D {
  CoordType x;
  CoordType y;
  CoordType z;

  bool operator==(const Point3D& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct Hasher {
  size_t operator()(const Point3D& point) const {
    // выбираем в качестве коэффициента довольно большое простое число;
    // свободный член можем положить равным нулю, т.к. он не влияет
    // на коллизии, а лишь циклически смещает бакеты
    const size_t coef = 2'946'901;

    const hash<CoordType> coord_hasher;

    return (
        coef * coef * coord_hasher(point.x) +
               coef * coord_hasher(point.y) +
                      coord_hasher(point.z)
    );
  }
};
