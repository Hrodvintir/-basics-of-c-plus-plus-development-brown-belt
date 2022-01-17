Мы хотим научиться встраивать структуру Person в контейнер unordered_set<Person, PersonHasher>. Для этого вам нужно реализовать структуры PersonHasher, AddressHasher, и операторы сравнения:  
```objectivec
struct Address {
  string city, street;
  int building;

  bool operator==(const Address& other) const {
    // реализуйте оператор
  }
};

struct Person {
  string name;
  int height;
  double weight;
  Address address;

  bool operator==(const Person& other) const {
    // реализуйте оператор
  }
};

struct AddressHasher {
  // реализуйте структуру
};

struct PersonHasher {
  // реализуйте структуру
};
```
Для этого используйте стандартные хеш-функции std::hash и комбинируйте хеши полей с помощью полинома, как показано в лекции.

Требования:

 - хеш-функции должны зависеть от всех полей структур

 - хеш-функция должна равномерно рассеивать объекты типа Person; это её свойство проверяется тестом TestDistribution в заготовке решения.
