#include <cstddef>
#include <utility>

template <typename T>
class UniquePtr {
private:
    T* ptr_;

public:
    UniquePtr() : ptr_(nullptr) {}
    UniquePtr(T * ptr) : ptr_(ptr) {}
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr(UniquePtr&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    UniquePtr& operator = (const UniquePtr&) = delete;
    UniquePtr& operator = (std::nullptr_t) {
        Reset(nullptr);
        return *this;
    }
    UniquePtr& operator = (UniquePtr&& other) {
        if (this != &other) {
            Reset(other.ptr_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    ~UniquePtr() {
        delete ptr_;
    }
    T& operator * () const {
        return *ptr_;
    }
    T * operator -> () const {
        return ptr_;
    }
    T * Release() {
        auto result = ptr_;
        ptr_ = nullptr;
        return result;
    }
    void Reset(T * ptr) {
        delete ptr_;
        ptr_ = ptr;
    }
    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }
    T * Get() const {
        return ptr_;
    }
};
