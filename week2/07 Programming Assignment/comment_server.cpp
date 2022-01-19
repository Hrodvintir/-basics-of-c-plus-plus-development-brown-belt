#include "test_runner.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <optional>
#include <unordered_set>

using namespace std;

enum class HttpCode {
    Ok = 200,
    NotFound = 404,
    Found = 302,
};

struct HttpHeader {
    string name, value;
};

class HttpResponse {
public:
    explicit HttpResponse(HttpCode code_) {
        code = static_cast<int>(code_);
    }

    HttpResponse &AddHeader(string name, string value) {
        headers.emplace_back(HttpHeader{move(name), move(value)});
        return *this;
    }

    HttpResponse &SetContent(string a_content) {
        content = move(a_content);
        return *this;
    }

    HttpResponse &SetCode(HttpCode a_code) {
        code = static_cast<int>(a_code);
        return *this;
    }

    friend ostream &operator<<(ostream &output, const HttpResponse &resp) {
        string comment;
        if (resp.code == 200) {
            comment = "OK";
        } else if (resp.code == 404) {
            comment = "Not found";
        } else if (resp.code == 302) {
            comment = "Found";
        }
        output << "HTTP/1.1 " << resp.code << " " << comment << '\n';
        if (!resp.content.empty()) {
            output << "Content-Length: " << to_string(resp.content.size()) << '\n';
        }
        for (const auto &i : resp.headers) {
            output << i.name << ": " << i.value << '\n';
        }
        output << '\n';
        output << resp.content;
        return output;
    }

public:
    int code;
    string content;
    vector<HttpHeader> headers;
};

struct HttpRequest {
    string method, path, body;
    map<string, string> get_params;
};

pair<string, string> SplitBy(const string &what, const string &by) {
    size_t pos = what.find(by);
    if (by.size() < what.size() && pos < what.size() - by.size()) {
        return {what.substr(0, pos), what.substr(pos + by.size())};
    } else {
        return {what, {}};
    }
}

template<typename T>
T FromString(const string &s) {
    T x;
    istringstream is(s);
    is >> x;
    return x;
}

pair<size_t, string> ParseIdAndContent(const string &body) {
    auto[id_string, content] = SplitBy(body, " ");
    return {FromString<size_t>(id_string), content};
}

struct LastCommentInfo {
    size_t user_id, consecutive_count;
};

class CommentServer {
private:
    vector<vector<string>> comments_;
    std::optional<LastCommentInfo> last_comment;
    unordered_set<size_t> banned_users;
    HttpResponse resp;

public:
    CommentServer() : resp(HttpCode::Ok){}
    HttpResponse ServeRequest(const HttpRequest &req) {
        if (req.method == "POST") {
            if (req.path == "/add_user") {
                resp.content = "";
                comments_.emplace_back();
                resp.SetCode(HttpCode::Ok);
                auto id = to_string(comments_.size() - 1);
//                resp.SetCode(HttpCode::Ok).AddHeader("Content-Length", to_string(id.size()))
//                        .SetContent(to_string(comments_.size() - 1));
                resp.SetCode(HttpCode::Ok).SetContent(to_string(comments_.size() - 1));
            } else if (req.path == "/add_comment") {
                auto[user_id, comment] = ParseIdAndContent(req.body);
                resp.content = "";
                resp.headers.clear();
//                cout << user_id << '\n';
//                cout << comment << '\n';

                if (!last_comment || last_comment->user_id != user_id) {
                    last_comment = LastCommentInfo{user_id, 1};
                } else if (++last_comment->consecutive_count > 3) {
                    banned_users.insert(user_id);
                }

                if (banned_users.count(user_id) == 0) {
//                    cout << "zero" << '\n';
//                    cout << user_id << '\n';
                    comments_[user_id].push_back(string(comment));
                    resp.SetCode(HttpCode::Ok);
//                    cout << req.path << '\n';
                } else {
//                    cout << "not zero" << '\n';
//                    cout << banned_users.count(user_id) << '\n';
//                    cout << last_comment->consecutive_count << '\n';
//                    cout << user_id << '\n';

                    resp.SetCode(HttpCode::Found).AddHeader("Location", "/captcha");
                }
            } else if (req.path == "/checkcaptcha") {
                resp.content = "";
                resp.headers.clear();
                if (auto[id, response] = ParseIdAndContent(req.body); response == "42") {
                    banned_users.erase(id);
                    if (last_comment && last_comment->user_id == id) {
                        last_comment.reset();
                    }
                    resp.SetCode(HttpCode::Ok);
                } else {
                    resp.SetCode(HttpCode::Found).AddHeader("Location", "/captcha");
                }
            } else {
                resp.SetCode(HttpCode::NotFound);
            }
        } else if (req.method == "GET") {
            if (req.path == "/user_comments") {
                resp.content = "";
                resp.headers.clear();
                auto user_id = FromString<size_t>(req.get_params.at("user_id"));
                string response;
                for (const string &c : comments_[user_id]) {
                    response += c + '\n';
                }
                resp.SetCode(HttpCode::Ok).SetContent(response);
            } else if (req.path == "/captcha") {
                resp.SetCode(HttpCode::Ok).SetContent(
                                "What's the answer for The Ultimate Question of Life, the Universe, and Everything?");
            } else {
                resp.content = "";
                resp.headers.clear();
                resp.SetCode(HttpCode::NotFound);
            }
        }
        return resp;
    }
};



ostream &operator<<(ostream &output, const HttpHeader &h) {
    return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader &lhs, const HttpHeader &rhs) {
    return lhs.name == rhs.name && lhs.value == rhs.value;
}

struct ParsedResponse {
    int code;
    vector<HttpHeader> headers;
    string content;
};

istream &operator>>(istream &input, ParsedResponse &r) {
    string line;
    getline(input, line);

    {
        istringstream code_input(line);
        string dummy;
        code_input >> dummy >> r.code;
    }

    size_t content_length = 0;

    r.headers.clear();
    while (getline(input, line) && !line.empty()) {
        if (auto[name, value] = SplitBy(line, ": "); name == "Content-Length") {
            istringstream length_input(value);
            length_input >> content_length;
        } else {
            r.headers.push_back({std::move(name), std::move(value)});
        }
    }

    r.content.resize(content_length);
    input.read(r.content.data(), r.content.size());
    return input;
}

void Test(CommentServer &srv, const HttpRequest &request, const ParsedResponse &expected) {
    HttpResponse resp = srv.ServeRequest(request);
//    cout << resp;
//    cout << resp.code << '\n';
//    cout << resp.headers << '\n';
//    cout << resp.content << '\n';
//    cout << expected.code << '\n';
//    cout << expected.headers << '\n';
//    cout << "expected : " << expected.content << '\n';
    ASSERT_EQUAL(resp.code, expected.code);
    ASSERT_EQUAL(resp.headers, expected.headers);
    ASSERT_EQUAL(resp.content, expected.content);
//    cout << resp << '\n';
}

template<typename CommentServer>
void TestServer() {
    CommentServer cs;

    const ParsedResponse ok{200};
    const ParsedResponse redirect_to_captcha{302, {{"Location", "/captcha"}}, {}};
    const ParsedResponse not_found{404};

    Test(cs, {"POST", "/add_user"}, {200, {}, "0"});
    Test(cs, {"POST", "/add_user"}, {200, {}, "1"});
    Test(cs, {"POST", "/add_comment", "0 Hello"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Hi"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Enlarge"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
    Test(cs, {"POST", "/add_comment", "0 What are you selling?"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
    Test(
            cs,
            {"GET", "/user_comments", "", {{"user_id", "0"}}},
            {200, {}, "Hello\nWhat are you selling?\n"}
    );
    Test(
            cs,
            {"GET", "/user_comments", "", {{"user_id", "1"}}},
            {200, {}, "Hi\nBuy my goods\nEnlarge\n"}
    );
    Test(
            cs,
            {"GET", "/captcha"},
            {200, {}, {"What's the answer for The Ultimate Question of Life, the Universe, and Everything?"}}
    );
    Test(cs, {"POST", "/checkcaptcha", "1 24"}, redirect_to_captcha);
    Test(cs, {"POST", "/checkcaptcha", "1 42"}, ok);
    Test(cs, {"POST", "/add_comment", "1 Sorry! No spam any more"}, ok);
    Test(
            cs,
            {"GET", "/user_comments", "", {{"user_id", "1"}}},
            {200, {}, "Hi\nBuy my goods\nEnlarge\nSorry! No spam any more\n"}
    );

    Test(cs, {"GET", "/user_commntes"}, not_found);
    Test(cs, {"POST", "/add_uesr"}, not_found);


}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestServer<CommentServer>);
}
