#pragma once

#include "array_ptr.h"

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <algorithm>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve) : capacity_to_reserve_(capacity_to_reserve) {

    }
    size_t GetCapacityToReserve() {
        return capacity_to_reserve_;
    }
private:
    size_t capacity_to_reserve_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) {
        if (size <= 0) {
            return;
        }
        ArrayPtr<Type> tmp(size);
        vector_.swap(tmp);
        size_ = size;
        capacity_ = size_;
        FillEmptyValues(begin(), end());
    }

    SimpleVector(ReserveProxyObj reserve) {
        ArrayPtr<Type> tmp(reserve.GetCapacityToReserve());
        size_ = 0;
        vector_.swap(tmp);
        capacity_ = reserve.GetCapacityToReserve();
    }

    SimpleVector(size_t size, const Type& value) {

        if (size <= 0) {
            return;
        }
        ArrayPtr<Type> tmp(size);
        vector_.swap(tmp);
        std::fill(vector_.Get(), vector_.Get() + size, value);
        size_ = size;
        capacity_ = size;
    }

    SimpleVector(std::initializer_list<Type> init) {
        size_t size = init.size();
        ArrayPtr<Type> tmp(size);
        vector_.swap(tmp);
        std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), begin());
        size_ = size;
        capacity_ = size;
    }

    SimpleVector(const SimpleVector& other) {

        SimpleVector tmp(other.size_);
        tmp.Initialize(other.begin(), other.end());
        swap(tmp);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {

        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) {

        vector_ = std::move(other.vector_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);

    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs) {
            vector_ = std::move(rhs.vector_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }


    void swap(SimpleVector& other) noexcept {

        vector_.swap(other.vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);

    }

    void Reserve(size_t new_capacity) {

        if (new_capacity > capacity_) {

            ArrayPtr<Type> tmp(new_capacity);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());
            vector_.swap(tmp);
            capacity_ = new_capacity;
        }

    }

    void PushBack(Type item) {

        if (size_ == capacity_) {

            Resize(size_ + 1);
            vector_[size_ - 1] = std::move(item);

        }
        else {
            vector_[size_++] = std::move(item);
        }

    }


    void PopBack() noexcept {

        if (!IsEmpty()) {
            --size_;
        }

    }


    Iterator Insert(ConstIterator pos, Type value) {

        if (IsEmpty()) {
            ++size_;
            size_t new_capacity = size_ * 2;
            ArrayPtr<Type> tmp(new_capacity);
            tmp[0] = std::move(value);
            vector_.swap(tmp);
            capacity_ = new_capacity;
            return begin();
        }

        size_t distance = pos - begin();
        if (size_ == capacity_) {

            size_t new_size = size_ * 2;
            InsertValueInPosition(distance, value, new_size);
            capacity_ = new_size;
            return begin() + distance;

        }
        InsertValueInPosition(distance, value, size_ + 1);
        return begin() + distance;

    }

    Iterator Erase(ConstIterator pos) {

        size_t distance = pos - begin();
        ArrayPtr<Type> tmp(capacity_);
        std::move(begin(), begin() + distance, tmp.Get());
        std::move(begin() + distance + 1, end(), tmp.Get() + distance);
        vector_.swap(tmp);
        --size_;
        return begin() + distance;

    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0 ? true : false;
    }

    Type& operator[](size_t index) noexcept {
        return vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return vector_[index];
    }

    Type& At(size_t index) {

        if (index >= size_) {
            throw std::out_of_range("");
        }
        return vector_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("");
        }
        return vector_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {

        if (IsEmpty()) {
            size_ = new_size;
            capacity_ = size_ * 2;
            ArrayPtr<Type> new_vector(capacity_);
            vector_.swap(new_vector);
            FillEmptyValues(begin(), end());
            return;
        }

        if (new_size > capacity_) {

            size_t new_capacity = new_size * 2;
            ArrayPtr<Type> new_vector(new_capacity);
            std::move(begin(), end(), new_vector.Get());
            vector_.swap(new_vector);
            FillEmptyValues(begin() + size_, begin() + new_capacity);
            size_ = new_size;
            capacity_ = new_capacity;
            return;
        }
        else if (new_size < capacity_) {

            if (new_size <= size_) {
                size_ = new_size;
                return;
            }
            FillEmptyValues(begin() + size_, begin() + new_size);
            size_ = new_size;
            return;
        }
    }

    Iterator begin() noexcept {
        return vector_.Get();
    }

    Iterator end() noexcept {
        return vector_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return const_cast<Type*>(vector_.Get());
    }

    ConstIterator end() const noexcept {
        return const_cast<Type*>(vector_.Get() + size_);
    }

    ConstIterator cbegin() const noexcept {
        return const_cast<Type*>(vector_.Get());
    }

    ConstIterator cend() const noexcept {
        return const_cast<Type*>(vector_.Get() + size_);;
    }

private:
    ArrayPtr<Type> vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
    void InsertValueInPosition(size_t position, Type& value, size_t tmp_vector_size) {

        ArrayPtr<Type> tmp(tmp_vector_size);
        std::move(begin(), begin() + position, tmp.Get());
        tmp[position] = std::move(value);
        std::move(begin() + position, end(), tmp.Get() + position + 1);
        vector_.swap(tmp);
        ++size_;
    }

    template<typename Iterator>
    void Initialize(Iterator begin, Iterator end) {
        size_t index = 0;
        for (auto cur_pos = begin; cur_pos != end; ++cur_pos) {
            vector_[index++] = *cur_pos;
        }
    }

    template<typename Iterator>
    void FillEmptyValues(Iterator begin, Iterator end) {
        for (auto cur_pos = begin; cur_pos != end; ++cur_pos) {
            *cur_pos = std::move(Type{});
        }
    }

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
