#include "cs1521/queue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <utility>

TEST_CASE("empty queue rejects dequeue and front", "[queue]") {
    cs1521::Queue<int> queue;
    REQUIRE(queue.empty());
    REQUIRE(queue.size() == 0);
    REQUIRE_THROWS_AS(queue.dequeue(), std::underflow_error);
    REQUIRE_THROWS_AS(queue.front(), std::underflow_error);

    const cs1521::Queue<int> constant;
    REQUIRE_THROWS_AS(constant.front(), std::underflow_error);
}

TEST_CASE("queue is first in first out", "[queue]") {
    cs1521::Queue<std::string> queue;
    queue.enqueue("a");
    queue.enqueue("b");
    queue.enqueue("c");
    REQUIRE(queue.size() == 3);
    REQUIRE(queue.front() == "a");
    queue.dequeue();
    REQUIRE(queue.front() == "b");
    queue.front() = "B";
    queue.dequeue();
    REQUIRE(queue.front() == "c");
    queue.dequeue();
    REQUIRE(queue.empty());
    REQUIRE_THROWS_AS(queue.dequeue(), std::underflow_error);
}

TEST_CASE("queue copy and move keep ownership separate", "[queue]") {
    cs1521::Queue<int> original;
    original.enqueue(1);
    original.enqueue(2);
    original.enqueue(3);

    cs1521::Queue<int> copy = original;
    copy.dequeue();
    REQUIRE(copy.front() == 2);
    REQUIRE(original.front() == 1);
    REQUIRE(original.size() == 3);

    cs1521::Queue<int> moved = std::move(original);
    REQUIRE(original.empty());
    REQUIRE(moved.front() == 1);
    original.enqueue(9);
    REQUIRE(original.front() == 9);
    REQUIRE(moved.size() == 3);

    moved.clear();
    REQUIRE(moved.empty());
    REQUIRE_THROWS_AS(moved.front(), std::underflow_error);
}
