#pragma once

#include <cassert>
#include <initializer_list>
#include <array>
#include <stdexcept>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if (size > 0) {
            Type* array = new Type[size];
            raw_ptr_ = array;
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr& other) = delete;

    ArrayPtr(ArrayPtr<Type>&& other) {
        if (this != &other ) {
            raw_ptr_ = std::move(other.raw_ptr_);
            other.Release();
        }
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr& other) {
        raw_ptr_ = other.raw_ptr_;
        return *this;
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this != &other) {
            raw_ptr_ = std::move(other.raw_ptr_);
            other.Release();
        }
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* p = raw_ptr_;
        raw_ptr_ = nullptr;
        return p;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        Type* array = raw_ptr_;
        Type* item_ptr = &array[index];
        return *item_ptr;
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        Type* array = raw_ptr_;
        Type* item_ptr = &array[index];
        return *item_ptr;
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        if (raw_ptr_ == nullptr) {
            return false;
        }
        return true;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        Type* array = raw_ptr_;
        return &array[0];
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};