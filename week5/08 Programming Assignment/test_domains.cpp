#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

template <typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

vector<string_view> Split(string_view s, string_view delimiter = " ") {
  vector<string_view> parts;
  if (s.empty()) {
    return parts;
  }
  while (true) {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    parts.push_back(lhs);
    if (!rhs_opt) {
      break;
    }
    s = *rhs_opt;
  }
  return parts;
}


class Domain {
public:
  explicit Domain(string_view text) {
    vector<string_view> parts = Split(text, ".");
    parts_reversed_.assign(rbegin(parts), rend(parts));
  }

  size_t GetPartCount() const {
    return parts_reversed_.size();
  }

  auto GetParts() const {
    return Range(rbegin(parts_reversed_), rend(parts_reversed_));
  }
  auto GetReversedParts() const {
    return Range(begin(parts_reversed_), end(parts_reversed_));
  }

  bool operator==(const Domain& other) const {
    return parts_reversed_ == other.parts_reversed_;
  }

private:
  vector<string> parts_reversed_;
};

ostream& operator<<(ostream& stream, const Domain& domain) {
  bool first = true;
  for (const string_view part : domain.GetParts()) {
    if (!first) {
      stream << '.';
    } else {
      first = false;
    }
    stream << part;
  }
  return stream;
}

// domain is subdomain of itself
bool IsSubdomain(const Domain& subdomain, const Domain& domain) {
  const auto subdomain_reversed_parts = subdomain.GetReversedParts();
  const auto domain_reversed_parts = domain.GetReversedParts();
  return
      subdomain.GetPartCount() >= domain.GetPartCount()
      && equal(begin(domain_reversed_parts), end(domain_reversed_parts),
               begin(subdomain_reversed_parts));
}

bool IsSubOrSuperDomain(const Domain& lhs, const Domain& rhs) {
  return lhs.GetPartCount() >= rhs.GetPartCount()
         ? IsSubdomain(lhs, rhs)
         : IsSubdomain(rhs, lhs);
}


class DomainChecker {
public:
  template <typename InputIt>
  DomainChecker(InputIt domains_begin, InputIt domains_end) {
    sorted_domains_.reserve(distance(domains_begin, domains_end));
    for (const Domain& domain : Range(domains_begin, domains_end)) {
      sorted_domains_.push_back(&domain);
    }
    sort(begin(sorted_domains_), end(sorted_domains_), IsDomainLess);
    sorted_domains_ = AbsorbSubdomains(move(sorted_domains_));
  }

  // Check if candidate is subdomain of some domain
  bool IsSubdomain(const Domain& candidate) const {
    const auto it = upper_bound(
        begin(sorted_domains_), end(sorted_domains_),
        &candidate, IsDomainLess);
    if (it == begin(sorted_domains_)) {
      return false;
    }
    return ::IsSubdomain(candidate, **prev(it));
  }

private:
  vector<const Domain*> sorted_domains_;

  static bool IsDomainLess(const Domain* lhs, const Domain* rhs) {
    const auto lhs_reversed_parts = lhs->GetReversedParts();
    const auto rhs_reversed_parts = rhs->GetReversedParts();
    return lexicographical_compare(
      begin(lhs_reversed_parts), end(lhs_reversed_parts),
      begin(rhs_reversed_parts), end(rhs_reversed_parts)
    );
  }

  static vector<const Domain*> AbsorbSubdomains(vector<const Domain*> domains) {
    domains.erase(
        unique(begin(domains), end(domains),
               [](const Domain* lhs, const Domain* rhs) {
                 return IsSubOrSuperDomain(*lhs, *rhs);
               }),
        end(domains)
    );
    return domains;
  }
};


vector<Domain> ReadDomains(istream& in_stream = cin) {
  vector<Domain> domains;

  size_t count;
  in_stream >> count;
  domains.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    string domain_text;
    in_stream >> domain_text;
    domains.emplace_back(domain_text);
  }
  return domains;
}

vector<bool> CheckDomains(const vector<Domain>& banned_domains, const vector<Domain>& domains_to_check) {
  const DomainChecker checker(begin(banned_domains), end(banned_domains));

  vector<bool> check_results;
  check_results.reserve(domains_to_check.size());
  for (const Domain& domain_to_check : domains_to_check) {
    check_results.push_back(!checker.IsSubdomain(domain_to_check));
  }

  return check_results;
}

void PrintCheckResults(const vector<bool>& check_results, ostream& out_stream = cout) {
  for (const bool check_result : check_results) {
    out_stream << (check_result ? "Good" : "Bad") << "\n";
  }
}

void TestSplit() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("m.ya.ru"), Domain("yandex.mail.ru"), Domain("gmail.com")};

    for (const auto& domain : banned_domains) {
        vector<string_view> splited_domain = Split("ya.ru");
    }
}

void TestReversedParts() {
    vector<string> domains = {"ya.ru", "mail.ru", "google.com"};
    for (const auto& domain : domains) {
        vector<string_view> splited_domain = Split(domain);
        ASSERT(!splited_domain.front().empty());
    }
}

void TestDomainIsSubDomain() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};

    for (size_t i = 0; i < banned_domains.size(); ++i) {
        ASSERT(IsSubdomain(banned_domains[i], domains_to_check[i]));
    }
}

void TestSubDomainParams() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("m.ya.ru"), Domain("yandex.mail.ru"), Domain("mail.google.com")};

    for (size_t i = 0; i < banned_domains.size(); ++i) {
        ASSERT_EQUAL(!IsSubdomain(banned_domains[i], domains_to_check[i]), IsSubdomain(domains_to_check[i], banned_domains[i]));
    }
}


void TestAbsorbSubDomains() {
    const vector<Domain> banned_domains = {Domain("m.ya.ru"), Domain("ya.ru")};
    const vector<Domain> domains_to_check = {Domain("shop.ya.ru")};

    DomainChecker dc(begin(banned_domains), end(banned_domains));

    ASSERT(dc.IsSubdomain(domains_to_check.front()));
}


void TestCheckSubdomain() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("m.ya.ru"), Domain("yandex.mail.ru"), Domain("mail.google.com")};
    DomainChecker dc(begin(banned_domains), end(banned_domains));
    for (const auto& domain : domains_to_check) {
        ASSERT(dc.IsSubdomain(domain));
    }
}


void TestGoodIsNotGood() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("m.ya.ru"), Domain("yandex.mail.ru"), Domain("mail.com")};

    string result = "Bad\nBad\nGood\n";

    stringstream ss;
    PrintCheckResults(CheckDomains(banned_domains, domains_to_check), ss);

    ASSERT_EQUAL(ss.str(), result);
}


void TestFirstElemInGetLine() {
    const vector<Domain> banned_domains = {Domain("ya.ru"), Domain("mail.ru"), Domain("google.com")};
    const vector<Domain> domains_to_check = {Domain("m.ya.ru"), Domain("yandex.mail.ru"), Domain("mail.com")};
    string s = "3\nya.ru\nmail.ru\ngoogle.com\n";
    stringstream ss(s);

    vector<Domain> result = ReadDomains(ss);

    ASSERT(!result.front().GetParts().begin()->empty());
}


int main() {
  TestRunner tr;
    RUN_TEST(tr, TestSplit);
    RUN_TEST(tr, TestReversedParts);
    RUN_TEST(tr, TestDomainIsSubDomain);
    RUN_TEST(tr, TestSubDomainParams);
    RUN_TEST(tr, TestAbsorbSubDomains);
    RUN_TEST(tr, TestCheckSubdomain);
    RUN_TEST(tr, TestGoodIsNotGood);
    RUN_TEST(tr, TestFirstElemInGetLine);

  const vector<Domain> banned_domains = ReadDomains();
  const vector<Domain> domains_to_check = ReadDomains();
  PrintCheckResults(CheckDomains(banned_domains, domains_to_check));
  return 0;
}
