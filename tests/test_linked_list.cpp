#include "cs1521/linked_list.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

struct Probe {
    static inline int alive = 0;
    int id = 0;

    explicit Probe(int identifier) : id(identifier) { ++alive; }

    Probe(const Probe& other) : id(other.id) { ++alive; }

    Probe& operator=(const Probe& other) {
        if (this != &other) {
            id = other.id;
        }
        return *this;
    }

    Probe(Probe&& other) noexcept : id(other.id) { ++alive; }

    Probe& operator=(Probe&& other) noexcept {
        if (this != &other) {
            id = other.id;
        }
        return *this;
    }

    ~Probe() { --alive; }

    friend bool operator==(const Probe& lhs, const Probe& rhs) { return lhs.id == rhs.id; }
};

std::vector<int> collect(const cs1521::SinglyLinkedList<int>& list) {
    std::vector<int> values;
    for (const int value : list) {
        values.push_back(value);
    }
    return values;
}

}  // namespace

TEST_CASE("empty list has no elements", "[linked-list]") {
    cs1521::SinglyLinkedList<int> list;
    const cs1521::SinglyLinkedList<int>& view = list;
    REQUIRE(view.empty());
    REQUIRE(view.size() == 0);
    REQUIRE_FALSE(view.contains(1));
    REQUIRE(view.find(1) == view.end());
    REQUIRE(view.begin() == view.end());
    REQUIRE_FALSE(list.remove(1));
    REQUIRE_THROWS_AS(view.front(), std::out_of_range);
    REQUIRE_THROWS_AS(view.back(), std::out_of_range);
}

TEST_CASE("insertion removal lookup and traversal", "[linked-list]") {
    cs1521::SinglyLinkedList<int> list;
    list.push_front(1);
    list.push_front(2);
    REQUIRE(collect(list) == std::vector<int>{2, 1});

    list.push_back(3);
    list.insert(1, 9);
    REQUIRE(collect(list) == std::vector<int>{2, 9, 1, 3});
    REQUIRE(list.size() == 4);
    REQUIRE(list.front() == 2);
    REQUIRE(list.back() == 3);
    REQUIRE(list.contains(9));
    REQUIRE(*list.find(1) == 1);
    REQUIRE(list.find(8) == list.end());

    REQUIRE(list.remove(9));
    REQUIRE_FALSE(list.contains(9));
    REQUIRE(list.remove(3));
    list.erase(0);
    REQUIRE(collect(list) == std::vector<int>{1});

    list.clear();
    REQUIRE(list.empty());
    list.push_back(4);
    REQUIRE(list.front() == 4);
    REQUIRE(list.back() == 4);
}

TEST_CASE("list index errors and reuse after draining", "[linked-list]") {
    cs1521::SinglyLinkedList<int> list{1, 2};
    list.pop_front();
    list.pop_front();
    REQUIRE_THROWS_AS(list.pop_front(), std::out_of_range);
    REQUIRE_THROWS_AS(list.insert(1, 5), std::out_of_range);
    REQUIRE_THROWS_AS(list.erase(0), std::out_of_range);

    list.push_back(1);
    list.push_back(2);
    list.pop_front();
    list.pop_front();
    list.push_back(3);
    REQUIRE(collect(list) == std::vector<int>{3});

    list.push_back(4);
    REQUIRE(list.remove(3));
    list.push_back(5);
    REQUIRE(collect(list) == std::vector<int>{4, 5});
}

TEST_CASE("list copy is deep and move leaves the source empty", "[linked-list]") {
    cs1521::SinglyLinkedList<int> original{1, 2, 3};
    cs1521::SinglyLinkedList<int> copy = original;
    REQUIRE(copy == original);
    copy.pop_front();
    copy.push_back(9);
    REQUIRE(collect(original) == std::vector<int>{1, 2, 3});
    REQUIRE(collect(copy) == std::vector<int>{2, 3, 9});

    original = original;
    REQUIRE(collect(original) == std::vector<int>{1, 2, 3});
    cs1521::SinglyLinkedList<int>* alias = &original;
    *alias = std::move(original);
    REQUIRE(collect(original) == std::vector<int>{1, 2, 3});

    cs1521::SinglyLinkedList<int> shorter{7};
    original = shorter;
    REQUIRE(original == shorter);

    cs1521::SinglyLinkedList<int> moved = std::move(copy);
    REQUIRE(copy.empty());
    REQUIRE(collect(moved) == std::vector<int>{2, 3, 9});
    copy.push_back(1);
    REQUIRE(collect(copy) == std::vector<int>{1});

    cs1521::SinglyLinkedList<int> assigned{4};
    assigned = std::move(moved);
    REQUIRE(moved.empty());
    REQUIRE(collect(assigned) == std::vector<int>{2, 3, 9});
}

TEST_CASE("list owns its elements", "[linked-list]") {
    {
        cs1521::SinglyLinkedList<Probe> list;
        list.push_back(Probe{1});
        list.push_back(Probe{2});
        REQUIRE(Probe::alive == 2);
        REQUIRE(list.front().id == 1);
        REQUIRE(list.back().id == 2);
    }
    REQUIRE(Probe::alive == 0);

    cs1521::SinglyLinkedList<std::unique_ptr<int>> pointers;
    pointers.push_back(std::make_unique<int>(5));
    auto moved = std::move(pointers);
    REQUIRE(pointers.empty());
    REQUIRE(*moved.front() == 5);
}

TEST_CASE("destructor walks a long chain", "[linked-list]") {
    cs1521::SinglyLinkedList<int> list;
    for (int value = 0; value < 10000; ++value) {
        list.push_back(value);
    }
    REQUIRE(list.size() == 10000);
    REQUIRE(list.front() == 0);
    REQUIRE(list.back() == 9999);
    list.clear();
    REQUIRE(list.empty());
}
