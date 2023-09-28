#pragma once
 
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iostream>
#include "array_ptr.h"
 
using namespace std;

class ReserveProxyObj {
    size_t capacity = 0;
public:
 
    ReserveProxyObj() = delete;
 
    explicit ReserveProxyObj(size_t new_capacity) : capacity(new_capacity) {}
 
    size_t GetCapacity() { return capacity; }
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
 
template <typename Type>
class SimpleVector
{
    ArrayPtr<Type> arr_;
 
    size_t size_ = 0;
    size_t capacity_ = 0;
 
    void ChangeCapacity(const size_t new_size) {
        SimpleVector new_arr(new_size);
        size_t prev_size = size_;
        std::move(begin(), end(), new_arr.begin());
        swap(new_arr);
        size_ = prev_size;
    }
 
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) :
            SimpleVector(size, Type{}) {
    }

    SimpleVector(size_t size, const Type& value) :
    arr_(size),
    size_(size),
    capacity_(size) {
        std::fill(begin(), end(), value);
    }
 
    SimpleVector(size_t size, Type&& value) :
    arr_(size),
    size_(size),
    capacity_(size) {
        for (auto it = arr_.Get(); it < arr_.Get() + size; ++it ) {
            *it = std::move(value);
            value = Type{};
        }
    }

    SimpleVector(std::initializer_list<Type> init) :
    arr_(init.size()),
    size_(init.size()),
    capacity_(init.size()) {
        std::move(init.begin(), init.end(), arr_.Get());
    }
 
    SimpleVector(const SimpleVector& other) {
        SimpleVector tmp(other.GetSize());
        std::copy(other.begin(), other.end(), tmp.begin());
        tmp.capacity_ = other.GetCapacity();
        swap(tmp);
    }

    SimpleVector(SimpleVector&& other) noexcept :
    arr_(other.size_) {
        if (this != &other)
        {
            arr_.swap(other.arr_);
            size_ = std::move(other.size_);
            capacity_ = std::move(other.capacity_);
            other.Clear();
        }
 
    }
 
    SimpleVector(ReserveProxyObj obj) :
            arr_(obj.GetCapacity()),
            size_(0),
            capacity_(obj.GetCapacity()) {
    }
 
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty())
                Clear();
            else {
                SimpleVector copy(rhs);
                swap(copy);
            }
        }
        return *this;
    }
 
    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            if (rhs.IsEmpty())
                Clear();
            else {
                swap(std::move(rhs));
                rhs.Clear();
            }
        }
        return *this;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return *(begin() + index);
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return *(begin() + index);
    }

    size_t GetSize() const noexcept { return size_; }

    size_t GetCapacity() const noexcept { return capacity_; }

    bool IsEmpty() const noexcept { return (size_ == 0); }

    Type& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range("Invalid index");
        return *(begin() + index);
    }

    const Type& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range("Invalid index");
        return *(begin() + index);
    }

    void Clear() noexcept { size_ = 0; }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            for (auto it = begin() + size_; it != begin() + new_size; ++it) {
                *it = std::move(Type{});
            }
        }
        else
            ChangeCapacity(new_size);
        size_ = new_size;
    }

    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }
        arr_[size_] = item;
        size_++;
    }
 
    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }
        arr_[size_] = std::move(item);
        item = std::move(Type{});
        size_++;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        auto index = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        auto it = begin() + index;
        std::copy_backward(it, end(), end() + 1);
        arr_[index] = std::move(value);
        ++size_;
        return Iterator(begin() + index);
    }
 
    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        auto index = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        auto it = begin() + index;
        std::move_backward(it, end(), end() + 1);
        arr_[index] = std::move(value);
        ++size_;
        value = std::move(Type{});
        return Iterator(begin() + index);
    }

    void PopBack() noexcept {
        if (IsEmpty()) return;
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        auto index = std::distance(cbegin(), pos);
        auto it = begin() + index;
        std::move((it + 1), end(), it);
        --size_;
        return Iterator(pos);
    }

    void swap(SimpleVector& other) noexcept {
        arr_.swap(other.arr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
 
    void swap(SimpleVector&& other) noexcept {
        arr_.swap(other.arr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        other.Clear();
    }

    void Reserve(size_t new_capacity) {
        if (capacity_ < new_capacity)
            ChangeCapacity(new_capacity);
    }

    Iterator begin() noexcept { return arr_.Get(); }

    Iterator end() noexcept { return arr_.Get() + size_; }
 

    ConstIterator begin() const noexcept { return ConstIterator(arr_.Get()); }
    ConstIterator end() const noexcept { return ConstIterator(arr_.Get() + size_); }
    ConstIterator cbegin() const noexcept { return begin(); }
    ConstIterator cend() const noexcept { return end(); }
};
 
template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}
 
template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}
 
template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
 
template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}
 
template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs > lhs);
}

template <typename Type>
std::ostream& operator<<(std::ostream& os, const SimpleVector<Type>& cont) {
    os << "{ ";
    for (auto it = cont.begin(); it != cont.end(); ++it) {
        os << *it << " ";
    }
    os << "}" << std::endl;
    return os;
}