# Запрещённые домены

Реализуйте фильтрацию произвольных доменов по фиксированному набору запрещённых доменов.

А именно, если запрещён домен ya.ru, то должен фильтроваться не только он сам, но и все его поддомены: ya.ru, m.ya.ru, m.mail.ya.ru и пр.

#### Входные данные
Сначала вводится количество запрещённых доменов и сами эти домены. Затем в том же формате вводятся домены, которые нужно проверить.

#### Выходные данные
Для каждого домена из второго набора выведите одну строку: Bad, если домен нужно отфильтровать (то есть он входит в список запрещённых или является поддоменом одного из них), и Good в противном случае.

### Пример
#### Ввод  
```c++
4
ya.ru
maps.me
m.ya.ru
com
7
ya.ru
ya.com
m.maps.me
moscow.m.ya.ru
maps.com
maps.ru
ya.ya
```
#### Вывод
```c++
Bad
Bad
Bad
Bad
Bad
Good
Good
```
### Ограничения
Гарантируется, что каждый домен — это набор строчных латинских букв, разделённых точками. Точка не может быть первым или последним символом домена. Две точки не могут быть соседними символами. (Иными словами, перед каждой точкой и после каждой точки находится буква.)

Длина каждого домена не более 50, в каждом наборе не более 10000 доменов.

Время работы программы должно укладываться в 3 секунды.

### Неправильное решение задачи
Вам дано решение этой задачи, однако в нём есть баги. Исправьте их либо перепишите код с нуля.
```c++
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;


bool IsSubdomain(string_view subdomain, string_view domain) {
  auto i = subdomain.size() - 1;
  auto j = domain.size() - 1;
  while (i >= 0 && j >= 0) {
    if (subdomain[i--] != domain[j--]) {
      return false;
    }
  }
  return (i < 0 && domain[j] == '.')
      || (j < 0 && subdomain[i] == '.');
}


vector<string> ReadDomains() {
  size_t count;
  cin >> count;

  vector<string> domains;
  for (size_t i = 0; i < count; ++i) {
    string domain;
    getline(cin, domain);
    domains.push_back(domain);
  }
  return domains;
}


int main() {
  const vector<string> banned_domains = ReadDomains();
  const vector<string> domains_to_check = ReadDomains();

  for (string_view domain : banned_domains) {
    reverse(begin(domain), end(domain));
  }
  sort(begin(banned_domains), end(banned_domains));

  size_t insert_pos = 0;
  for (string& domain : banned_domains) {
    if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
      swap(banned_domains[insert_pos++], domain);
    }
  }
  banned_domains.resize(insert_pos);

  for (const string_view domain : domains_to_check) {
    if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
        it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
      cout << "Good" << endl;
    } else {
      cout << "Bad" << endl;
    }
  }
  return 0;
}
```
