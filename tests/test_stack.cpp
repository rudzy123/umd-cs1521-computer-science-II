#include "cs1521/stack.hpp"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <utility>

TEST_CASE("empty stack rejects pop and top", "[stack]") {
    cs1521::Stack<int> stack;
    REQUIRE(stack.empty());
    REQUIRE(stack.size() == 0);
    REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);
    REQUIRE_THROWS_AS(stack.top(), std::underflow_error);

    const cs1521::Stack<int> constant;
    REQUIRE_THROWS_AS(constant.top(), std::underflow_error);
}

TEST_CASE("stack is last in first out", "[stack]") {
    cs1521::Stack<std::string> stack;
    stack.push("a");
    stack.push("b");
    stack.push("c");
    REQUIRE(stack.size() == 3);
    REQUIRE(stack.top() == "c");
    stack.pop();
    REQUIRE(stack.top() == "b");
    stack.top() = "B";
    stack.pop();
    REQUIRE(stack.top() == "a");
    stack.pop();
    REQUIRE(stack.empty());
    REQUIRE_THROWS_AS(stack.pop(), std::underflow_error);
}

TEST_CASE("stack copy and move keep ownership separate", "[stack]") {
    cs1521::Stack<int> original;
    original.push(1);
    original.push(2);

    cs1521::Stack<int> copy = original;
    copy.pop();
    REQUIRE(copy.top() == 1);
    REQUIRE(original.top() == 2);

    cs1521::Stack<int> moved = std::move(original);
    REQUIRE(original.empty());
    REQUIRE(moved.top() == 2);
    original.push(7);
    REQUIRE(original.top() == 7);
    REQUIRE(moved.size() == 2);

    moved.clear();
    REQUIRE(moved.empty());
    REQUIRE_THROWS_AS(moved.top(), std::underflow_error);
}
