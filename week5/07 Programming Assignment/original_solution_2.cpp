#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;


bool IsSubdomain(string_view subdomain_view, string_view domain_view) {
    const string subdomain(rbegin(subdomain_view), rend(subdomain_view));
    const string domain(rbegin(domain_view), rend(domain_view));
    int i = subdomain.size() - 1;  // TODO: make auto
    int j = domain.size() - 1;     // TODO: make auto
    while (i >= 0 && j >= 0) {
        if (subdomain[i--] != domain[j--]) {
            return false;
        }
    }
    return (i < 0 && j < 0)  // TODO: do not check if i and j < 0
           || (i < 0 && domain[j] == '.')
           || (j < 0 && subdomain[i] == '.');
}


vector<string> ReadDomains() {
    vector<string> domains;

    size_t count;
    cin >> count;
    domains.reserve(count);

    string _;
    getline(cin, _);  // TODO: remove

    for (size_t i = 0; i < count; ++i) {
        string domain;
        getline(cin, domain);  // BUG: empty string is read at first step
        domains.push_back(domain);
    }
    return domains;
}


int main() {
    vector<string> banned_domains = ReadDomains();  // TODO: add const
    vector<string> domains_to_check = ReadDomains();  // TODO: add const

    for (string& domain : banned_domains) {  // TODO: iterate with string_view
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

    for (string &domain : domains_to_check) {
        reverse(begin(domain), end(domain));  // TODO: remove
        if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
                it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
            cout << "Bad" << endl;
        } else {
            cout << "Good" << endl;
        }
    }
    return 0;
}
