#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
    string from;
    string to;
    string body;

    Email() = default;

    Email(string from_, string to_, string body_)
            : from(move(from_)), to(move(to_)), body(move(body_)) {
    }
};


class Worker {
public:
    virtual ~Worker() = default;

    virtual void Process(unique_ptr<Email> email) = 0;

    virtual void Run() {
        // только первому worker-у в пайплайне нужно это имплементировать
        throw logic_error("Unimplemented");
    }

protected:
    // реализации должны вызывать PassOn, чтобы передать объект дальше
    // по цепочке обработчиков
    void PassOn(unique_ptr<Email> email) const {
        if (worker != nullptr) {
            worker->Process(move(email));
        }
    }

public:
    void SetNext(unique_ptr<Worker> next) {
        if (worker == nullptr) {
            worker = move(next);
        }
    }

private:
    unique_ptr<Worker> worker;
};


class Reader : public Worker {
public:
    // реализуйте класс
    Reader(istream &input) : input_(input) {
    }

    void Process(unique_ptr<Email> email) override {
        PassOn(move(email));
    }

    void Run() override {
        while (input_) {
            Email email;
            getline(input_, email.from);
            if (input_.eof()) continue;
            getline(input_, email.to);
            if (input_.eof()) continue;
            getline(input_, email.body);
            if (input_.eof()) continue;

            Process(make_unique<Email>(move(email)));
        }
    }

private:
    istream &input_;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email &)>;

public:
    // реализуйте класс
    Filter(Function function) : function_(move(function)) {
    }

    void Process(unique_ptr<Email> email) override {
        if (email && function_(*email)) {
            PassOn(move(email));
        }
    }

private:
    Function function_;
};


class Copier : public Worker {
public:
    // реализуйте класс
    Copier(const string copier) : copier_(copier) {
    }

    void Process(unique_ptr<Email> email) override {
        if (email && email->to != copier_) {
            auto copy = make_unique<Email>(email->from, copier_, email->body);
            PassOn(move(email));
            PassOn(move(copy));
        } else {
            PassOn(move(email));
        }
    }

private:
    string copier_;
};


class Sender : public Worker {
public:
    // реализуйте класс
    Sender(ostream &out) : output_(out) {
    }

    void Process(unique_ptr<Email> email) override {
        output_ << email->from << '\n';
        output_ << email->to << '\n';
        output_ << email->body << '\n';
        PassOn(move(email));
    }

private:
    ostream &output_;
};


// реализуйте класс
class PipelineBuilder {
public:
    // добавляет в качестве первого обработчика Reader
    explicit PipelineBuilder(istream &in) {
        workers.emplace_back(make_unique<Reader>(in));
    }

    // добавляет новый обработчик Filter
    PipelineBuilder &FilterBy(Filter::Function filter) {
        workers.emplace_back(make_unique<Filter>(move(filter)));
        return *this;
    }

    // добавляет новый обработчик Copier
    PipelineBuilder &CopyTo(string recipient) {
        workers.emplace_back(make_unique<Copier>(move(recipient)));
        return *this;
    }

    // добавляет новый обработчик Sender
    PipelineBuilder &Send(ostream &out) {
        workers.emplace_back(make_unique<Sender>(out));
        return *this;
    }

    // возвращает готовую цепочку обработчиков
    unique_ptr<Worker> Build() {
        for (size_t i = workers.size() - 1; i-- > 0;) {
            workers[i]->SetNext(move(workers[i + 1]));
        }
        return move(workers.front());
    }

private:
    vector<unique_ptr<Worker>> workers;
};


void TestSanity() {
    string input = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "ralph@example.com\n"
            "erich@example.com\n"
            "I do not make mistakes of that kind\n"
    );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email &email) {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
            "erich@example.com\n"
            "richard@example.com\n"
            "Hello there\n"

            "erich@example.com\n"
            "ralph@example.com\n"
            "Are you sure you pressed the right button?\n"

            "erich@example.com\n"
            "richard@example.com\n"
            "Are you sure you pressed the right button?\n"
    );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
