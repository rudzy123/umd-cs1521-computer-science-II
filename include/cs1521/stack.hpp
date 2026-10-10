#pragma once

#include "cs1521/linked_list.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace cs1521 {

// Stack ADT. The contract is LIFO: push, pop, and top. The concrete
// representation is a singly linked list, hidden from callers. Composition
// is the right relationship: a stack is not a kind of list.
//
// pop returns void, matching std::stack. Returning T by value would require
// a copy that can throw after the element has already been removed.
template <typename T>
class Stack {
public:
    Stack() = default;
    Stack(const Stack&) = default;
    Stack(Stack&&) noexcept = default;
    Stack& operator=(const Stack&) = default;
    Stack& operator=(Stack&&) noexcept = default;
    ~Stack() = default;

    void push(T value) { data_.push_front(std::move(value)); }

    void pop() {
        if (data_.empty()) {
            throw std::underflow_error("pop on an empty stack");
        }
        data_.pop_front();
    }

    [[nodiscard]] T& top() {
        if (data_.empty()) {
            throw std::underflow_error("top on an empty stack");
        }
        return data_.front();
    }

    [[nodiscard]] const T& top() const {
        if (data_.empty()) {
            throw std::underflow_error("top on an empty stack");
        }
        return data_.front();
    }

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    void clear() noexcept { data_.clear(); }

private:
    SinglyLinkedList<T> data_;
};

}  // namespace cs1521
