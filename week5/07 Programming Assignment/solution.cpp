//#include "test_runner2.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

bool isSubDomain(const string &subdomain, const string &domain) {
    int i = 0;
    int j = 0;
    while (i < subdomain.size() && j < domain.size()) {
        if (subdomain[i++] != domain[j++]) {
            return false;
        }
    }
    if (subdomain.size() != domain.size()) {
        return (j == domain.size() && subdomain[i] == '.') ||
               (i == subdomain.size() && domain[j] == '.');
    }
    return true;
}

vector<string> ReadBannedDomain(istream &is) {
    size_t count;
    is >> count;
    vector<string> result;
    for (auto i = 0; i < count; ++i) {
        string domain;
        is >> domain;
        reverse(begin(domain), end(domain));
        result.push_back(domain);

    }
    sort(begin(result), end(result));

    size_t insert_pos = 0;
    for (string& domain : result) {
        if (insert_pos == 0 || !isSubDomain(domain, result[insert_pos - 1])) {
            swap(result[insert_pos++], domain);
        }
    }
    result.resize(insert_pos);
    return result;
}


vector<string> ReadDomain(istream &is) {
    size_t count;
    is >> count;
    vector<string> result;
    for (auto i = 0; i < count; ++i) {
        string domain;
        is >> domain;
        reverse(begin(domain), end(domain));
        result.push_back(domain);
    }
    return result;
}



string CheckIfBanned(vector<string> &banned_, const string &domain) {
    if (const auto it = upper_bound(begin(banned_), end(banned_), domain);
        it != begin(banned_) && isSubDomain(domain, *prev(it))) {
        return "Bad";
    }else {
        return "Good";
    }
}

//void SimpleTest() {
//    string bdomains = "1\nya.ru\n";
//    string sdomains = "5\nya.ru\nya.com\nm.ya.ru\nm.mail.ya.ru\nru\n";
//    stringstream banned(bdomains);
//    stringstream search(sdomains);
//    vector<string> banned_ = ReadBannedDomain(banned);
//    vector<string> search_ = ReadDomain(search);
//
//    vector<string> expected = {"Bad", "Good", "Bad", "Bad", "Good"};
//
//    for (size_t i = 0; i < search_.size(); ++i) {
//        ASSERT_EQUAL(CheckIfBanned(banned_, search_[i]), expected[i]);
//    }
//}

//void TestSameLong() {
//    string bdomains = "3\nya.ru\nm.ya.ru\nmoscow.spb.ru\n";
//    string sdomains = "5\nm.maps.me\nm.ya.ru\nb.ya.ru\nm.ya\nya.ya\n";
//    stringstream banned(bdomains);
//    stringstream search(sdomains);
//    vector<string> banned_ = ReadBannedDomain(banned);
//    vector<string> search_ = ReadDomain(search);
//
//    vector<string> expected = {"Good", "Bad", "Bad", "Good", "Good"};
//
//    for (size_t i = 0; i < search_.size(); ++i) {
//        ASSERT_EQUAL(CheckIfBanned(banned_, search_[i]), expected[i]);
//    }
//}

//void TestSame() {
//    string bdomains = "1\nm.spb.ru.com\n";
//    string sdomains = "6\ncom\nru.com\nspb.ru.com\nm.spb.ru.com\ns.spb.ru.com\ns.m.spb.ru.com\n";
//    stringstream banned(bdomains);
//    stringstream search(sdomains);
//    vector<string> banned_ = ReadBannedDomain(banned);
//    vector<string> search_ = ReadDomain(search);
//
//    vector<string> expected = {"Good", "Good", "Good", "Bad", "Good", "Bad"};
//
//    for (size_t i = 0; i < search_.size(); ++i) {
//        ASSERT_EQUAL(CheckIfBanned(banned_, search_[i]), expected[i]);
//    }
//}

//void TestAllBanned() {
//    string bdomains = "2\ncom\nya.ru\n";
//    string sdomains = "7\ncom\nru.com\nspb.ru.com\nm.spb.ru.com\ns.spb.ru.com\ns.m.spb.ru.com\nspb.ya.ru\n";
//    stringstream banned(bdomains);
//    stringstream search(sdomains);
//    vector<string> banned_ = ReadBannedDomain(banned);
//    vector<string> search_ = ReadDomain(search);
//
//    vector<string> expected = {"Bad", "Bad", "Bad", "Bad", "Bad", "Bad", "Bad"};
//
//    for (size_t i = 0; i < search_.size(); ++i) {
//        ASSERT_EQUAL(CheckIfBanned(banned_, search_[i]), expected[i]);
//    }
//}

//void TestCheckSubDomains() {
//    string bdomains = "8\nya.com\nspb.com\nm.com\nm.ya.com\nya.m.com\nya.ya.com\nyaya.com\nya.ru.com\n";
//    string sdomains = "8\ncom\nru.com\nspb.ru.com\nm.spb.com\ns.spb.com\nspb.m.com\nspb.yaya.com\nmoscow.ya.ru\n";
//    stringstream banned(bdomains);
//    stringstream search(sdomains);
//    vector<string> banned_ = ReadBannedDomain(banned);
//    vector<string> search_ = ReadDomain(search);
//
//    vector<string> expected = {"Good", "Good", "Good", "Bad", "Bad", "Bad", "Bad", "Good"};
//
//    for (size_t i = 0; i < search_.size(); ++i) {
//        ASSERT_EQUAL(CheckIfBanned(banned_, search_[i]), expected[i]);
//    }
//}

int main() {
//    TestRunner tr;
//    RUN_TEST(tr, SimpleTest);
//    RUN_TEST(tr, TestSameLong);
//    RUN_TEST(tr, TestSame);
//    RUN_TEST(tr, TestAllBanned);
//    RUN_TEST(tr, TestCheckSubDomains);

    vector<string> banned_domains = ReadBannedDomain(cin);
    vector<string> domains_to_check = ReadDomain(cin);

    for (const auto& domain : domains_to_check) {
        cout << CheckIfBanned(banned_domains, domain) << '\n';
    }

    return 0;
}
