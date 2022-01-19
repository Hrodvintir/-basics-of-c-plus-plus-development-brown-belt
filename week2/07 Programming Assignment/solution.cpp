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

struct HttpRequest {
    string method, path, body;
    map<string, string> get_params;
};

pair<string, string> SplitBy(const string& what, const string& by) {
    size_t pos = what.find(by);
    if (by.size() < what.size() && pos < what.size() - by.size()) {
        return {what.substr(0, pos), what.substr(pos + by.size())};
    } else {
        return {what, {}};
    }
}

template<typename T>
T FromString(const string& s) {
    T x;
    istringstream is(s);
    is >> x;
    return x;
}

enum class HttpCode {
    Ok = 200,
    NotFound = 404,
    Found = 302,
};

ostream& operator<<(ostream& output, HttpCode code) {
    switch (code) {
        case HttpCode::Ok:
            output << "200 OK";
            break;
        case HttpCode::Found:
            output << "302 Found";
            break;
        case HttpCode::NotFound:
            output << "404 Not found";
            break;
        default:
            throw invalid_argument("Unknown http code");
    }
    return output;
}

struct HttpHeader {
    string name, value;
};

ostream& operator<<(ostream& output, const HttpHeader& h) {
    return output << h.name << ": " << h.value;
}

class HttpResponse {
public:
    explicit HttpResponse(HttpCode code) : code(code) {
    }

    HttpResponse& AddHeader(string name, string value) {
        headers.push_back(HttpHeader{std::move(name), std::move(value)});
        return *this;
    }

    HttpResponse& SetContent(string a_content) {
        content = std::move(a_content);
        return *this;
    }

    HttpResponse& SetCode(HttpCode a_code) {
        code = a_code;
        return *this;
    }

    friend ostream& operator << (ostream& output, const HttpResponse& resp) {
        output << "HTTP/1.1 " << resp.code << '\n';
        for (const auto& header : resp.headers) {
            output << header << '\n';
        }
        if (!resp.content.empty()) {
            output << "Content-Length: " << resp.content.size() << '\n';
        }
        return output << '\n' << resp.content;
    }

private:
    HttpCode code;
    vector<HttpHeader> headers;
    string content;
};

pair<size_t, string> ParseIdAndContent(const string& body) {
    auto [id_string, content] = SplitBy(body, " ");
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

public:
    HttpResponse ServeRequest(const HttpRequest& req) {
        if (req.method == "POST") {
            if (req.path == "/add_user") {
                return ServeAddUser(req);
            } else if (req.path == "/add_comment") {
                return ServeAddComment(req);
            } else if (req.path == "/checkcaptcha") {
                return ServeCheckCaptcha(req);
            } else {
                return HttpResponse(HttpCode::NotFound);
            }
        } else if (req.method == "GET") {
            if (req.path == "/user_comments") {
                return ServeUserComments(req);
            } else if (req.path == "/captcha") {
                return ServeCaptcha(req);
            } else {
                return HttpResponse(HttpCode::NotFound);
            }
        } else {
            return HttpResponse(HttpCode::NotFound);
        }
    }

private:
    HttpResponse ServeAddUser(const HttpRequest& request) {
        comments_.emplace_back();
        return HttpResponse(HttpCode::Ok).SetContent(to_string(comments_.size() - 1));
    }

    HttpResponse ServeAddComment(const HttpRequest& request) {
        auto [user_id, comment] = ParseIdAndContent(request.body);

        if (!last_comment || last_comment->user_id != user_id) {
            last_comment = LastCommentInfo {user_id, 1};
        } else if (++last_comment->consecutive_count > 3) {
            banned_users.insert(user_id);
        }

        if (banned_users.count(user_id) == 0) {
            comments_[user_id].push_back(comment);
            return HttpResponse(HttpCode::Ok);
        } else {
            return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
        }
    }

    HttpResponse ServeCheckCaptcha(const HttpRequest& request) {
        if (auto [id, response] = ParseIdAndContent(request.body); response == "42") {
            banned_users.erase(id);
            if (last_comment && last_comment->user_id == id) {
                last_comment.reset();
            }
            return HttpResponse(HttpCode::Ok);
        } else {
            return HttpResponse(HttpCode::Found).AddHeader("Location", "/captcha");
        }
    }

    HttpResponse ServeUserComments(const HttpRequest& request) {
        auto user_id = FromString<size_t>(request.get_params.at("user_id"));
        string response;
        for (const string& c : comments_[user_id]) {
            response += c + '\n';
        }

        return HttpResponse(HttpCode::Ok).SetContent(std::move(response));
    }

    HttpResponse ServeCaptcha(const HttpRequest&) {
        return HttpResponse(HttpCode::Ok)
                .SetContent(
                        "What's the answer for The Ultimate Question of Life, the Universe, and Everything?"
                );
    }
};
