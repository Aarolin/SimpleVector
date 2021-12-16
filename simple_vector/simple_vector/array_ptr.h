#pragma once

#include <cstdlib>
#include <utility>
#include <iostream>

template <typename Type>
class ArrayPtr {
public:

    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size > 0) {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ArrayPtr& operator=(ArrayPtr&& rhs) {
        if (this != &rhs) {
            raw_ptr_ = std::exchange(rhs.raw_ptr_, nullptr);
        }
        return *this;
    }

    ~ArrayPtr() {

        if (raw_ptr_ != nullptr) {

            delete[] raw_ptr_;
            raw_ptr_ = nullptr;

        }
    }

    [[nodiscard]] Type* Release() noexcept {

        Type* copy = raw_ptr_;
        raw_ptr_ = nullptr;
        return copy;

    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr ? true : false;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {

        Type* copy = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = copy;

    }

private:

    Type* raw_ptr_ = nullptr;

};