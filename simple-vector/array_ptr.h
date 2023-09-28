#pragma once

#include <cassert>
#include <initializer_list>
#include <array>
#include <stdexcept>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size > 0) {
            Type* array = new Type[size];
            raw_ptr_ = array;
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr& other) = delete;

    ArrayPtr(ArrayPtr<Type>&& other) {
        raw_ptr_ = other.raw_ptr_;
        other.Release();
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    ArrayPtr& operator=(const ArrayPtr& other) {
        raw_ptr_ = other.raw_ptr_;
        return *this;
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this != &other) {
            std::swap(this->raw_ptr_, other.raw_ptr_);
        }
        return *this;
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* p = raw_ptr_;
        raw_ptr_ = nullptr;
        return p;
    }

    Type& operator[](size_t index) noexcept {
        Type* array = raw_ptr_;
        Type* item_ptr = &array[index];
        return *item_ptr;
    }

    const Type& operator[](size_t index) const noexcept {
        Type* array = raw_ptr_;
        Type* item_ptr = &array[index];
        return *item_ptr;
    }

    explicit operator bool() const {
        if (raw_ptr_ == nullptr) {
            return false;
        }
        return true;
    }

    Type* Get() const noexcept {
        Type* array = raw_ptr_;
        return &array[0];
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};