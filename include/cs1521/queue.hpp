#pragma once

#include "cs1521/linked_list.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace cs1521 {

// Queue ADT. The contract is FIFO: enqueue at the back, dequeue from the
// front. Both ends are O(1) because the list keeps a non-owning tail.
// Callers do not get iterators; walking the middle would break the abstraction.
template <typename T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue&) = default;
    Queue(Queue&&) noexcept = default;
    Queue& operator=(const Queue&) = default;
    Queue& operator=(Queue&&) noexcept = default;
    ~Queue() = default;

    void enqueue(T value) { data_.push_back(std::move(value)); }

    void dequeue() {
        if (data_.empty()) {
            throw std::underflow_error("dequeue on an empty queue");
        }
        data_.pop_front();
    }

    [[nodiscard]] T& front() {
        if (data_.empty()) {
            throw std::underflow_error("front on an empty queue");
        }
        return data_.front();
    }

    [[nodiscard]] const T& front() const {
        if (data_.empty()) {
            throw std::underflow_error("front on an empty queue");
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
