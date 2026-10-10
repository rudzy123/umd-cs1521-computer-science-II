#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cs1521 {

// Singly linked list. Each node is owned by a unique_ptr, so destroying the
// list frees every node. Every raw Node* in this class, including tail_ and
// the iterators, is non-owning: it only observes a node the unique_ptr chain
// already owns. tail_ exists so push_back and back() are O(1).
//
// The destructor unlinks nodes in a loop. Resetting the head unique_ptr
// alone would destroy the chain recursively and can overflow the call stack
// on a long list.
template <typename T>
class SinglyLinkedList {
    struct Node {
        T value;
        std::unique_ptr<Node> next;

        explicit Node(T val) : value(std::move(val)) {}
    };

public:
    // Non-owning cursor. Copying an iterator copies the address, not the node.
    // Incrementing end(), or using an iterator after insert, erase, remove,
    // clear, or destruction, is undefined.
    template <bool IsConst>
    class Iterator {
        using node_pointer = std::conditional_t<IsConst, const Node*, Node*>;

        template <bool>
        friend class Iterator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        Iterator() = default;
        explicit Iterator(node_pointer node) : node_(node) {}

        template <bool OtherConst>
            requires(IsConst && !OtherConst)
        Iterator(const Iterator<OtherConst>& other) : node_(other.node_) {}

        [[nodiscard]] reference operator*() const { return node_->value; }
        [[nodiscard]] pointer operator->() const { return &node_->value; }

        Iterator& operator++() {
            node_ = node_->next.get();
            return *this;
        }

        Iterator operator++(int) {
            Iterator previous = *this;
            ++(*this);
            return previous;
        }

        template <bool OtherConst>
        [[nodiscard]] bool operator==(const Iterator<OtherConst>& other) const noexcept {
            return node_ == other.node_;
        }

    private:
        node_pointer node_ = nullptr;
    };

    using value_type = T;
    using size_type = std::size_t;
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    SinglyLinkedList() = default;

    SinglyLinkedList(std::initializer_list<T> values) {
        for (const T& value : values) {
            push_back(value);
        }
    }

    SinglyLinkedList(const SinglyLinkedList& other) {
        for (const T& value : other) {
            push_back(value);
        }
    }

    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : head_(std::move(other.head_)), tail_(std::exchange(other.tail_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}

    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList copy(other);
            swap(copy);
        }
        return *this;
    }

    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = std::move(other.head_);
            tail_ = std::exchange(other.tail_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    ~SinglyLinkedList() { clear(); }

    void push_front(T value) {
        auto node = std::make_unique<Node>(std::move(value));
        node->next = std::move(head_);
        head_ = std::move(node);
        if (tail_ == nullptr) {
            tail_ = head_.get();
        }
        ++size_;
    }

    void push_back(T value) {
        auto node = std::make_unique<Node>(std::move(value));
        // Non-owning. node still owns the allocation; raw is only the address
        // we will store in tail_ after the unique_ptr has been linked in.
        Node* raw = node.get();
        if (tail_ == nullptr) {
            head_ = std::move(node);
        } else {
            tail_->next = std::move(node);
        }
        tail_ = raw;
        ++size_;
    }

    // index == size() appends. index > size() throws.
    void insert(std::size_t index, T value) {
        if (index > size_) {
            throw std::out_of_range("insert index is past size");
        }
        if (index == 0) {
            push_front(std::move(value));
            return;
        }
        if (index == size_) {
            push_back(std::move(value));
            return;
        }

        Node* previous = head_.get();
        for (std::size_t step = 0; step + 1 < index; ++step) {
            previous = previous->next.get();
        }
        auto node = std::make_unique<Node>(std::move(value));
        node->next = std::move(previous->next);
        previous->next = std::move(node);
        ++size_;
    }

    void pop_front() {
        if (head_ == nullptr) {
            throw std::out_of_range("pop_front on an empty list");
        }
        head_ = std::move(head_->next);
        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        --size_;
    }

    // Erases the first element equal to value. Returns false if it is absent.
    // previous and current are non-owning cursors into the unique_ptr chain.
    bool remove(const T& value) {
        Node* previous = nullptr;
        Node* current = head_.get();
        while (current != nullptr) {
            if (current->value == value) {
                if (previous == nullptr) {
                    pop_front();
                } else {
                    if (current == tail_) {
                        tail_ = previous;
                    }
                    previous->next = std::move(current->next);
                    --size_;
                }
                return true;
            }
            previous = current;
            current = current->next.get();
        }
        return false;
    }

    void erase(std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("erase index is out of range");
        }
        if (index == 0) {
            pop_front();
            return;
        }

        Node* previous = head_.get();
        for (std::size_t step = 0; step + 1 < index; ++step) {
            previous = previous->next.get();
        }
        if (previous->next.get() == tail_) {
            tail_ = previous;
        }
        previous->next = std::move(previous->next->next);
        --size_;
    }

    [[nodiscard]] iterator find(const T& value) {
        for (iterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    [[nodiscard]] const_iterator find(const T& value) const {
        for (const_iterator it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    [[nodiscard]] bool contains(const T& value) const { return find(value) != end(); }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    void clear() noexcept {
        while (head_ != nullptr) {
            head_ = std::move(head_->next);
        }
        tail_ = nullptr;
        size_ = 0;
    }

    [[nodiscard]] T& front() {
        if (head_ == nullptr) {
            throw std::out_of_range("front on an empty list");
        }
        return head_->value;
    }

    [[nodiscard]] const T& front() const {
        if (head_ == nullptr) {
            throw std::out_of_range("front on an empty list");
        }
        return head_->value;
    }

    [[nodiscard]] T& back() {
        if (tail_ == nullptr) {
            throw std::out_of_range("back on an empty list");
        }
        return tail_->value;
    }

    [[nodiscard]] const T& back() const {
        if (tail_ == nullptr) {
            throw std::out_of_range("back on an empty list");
        }
        return tail_->value;
    }

    [[nodiscard]] iterator begin() { return iterator{head_.get()}; }
    [[nodiscard]] iterator end() { return iterator{nullptr}; }
    [[nodiscard]] const_iterator begin() const { return const_iterator{head_.get()}; }
    [[nodiscard]] const_iterator end() const { return const_iterator{nullptr}; }
    [[nodiscard]] const_iterator cbegin() const { return begin(); }
    [[nodiscard]] const_iterator cend() const { return end(); }

    void swap(SinglyLinkedList& other) noexcept {
        head_.swap(other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    friend bool operator==(const SinglyLinkedList& lhs, const SinglyLinkedList& rhs) {
        if (lhs.size_ != rhs.size_) {
            return false;
        }
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

private:
    std::unique_ptr<Node> head_;
    // Non-owning observer of the last node. Null when the list is empty.
    // The node is owned by head_ or by the previous node's next unique_ptr.
    // Move operations null this out so a moved-from list does not dangle.
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

template <typename T>
void swap(SinglyLinkedList<T>& lhs, SinglyLinkedList<T>& rhs) noexcept {
    lhs.swap(rhs);
}

}  // namespace cs1521
