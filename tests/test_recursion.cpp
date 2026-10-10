#include "cs1521/recursion.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <span>
#include <stdexcept>
#include <vector>

TEST_CASE("factorial base cases and defined values", "[recursion]") {
    REQUIRE(cs1521::factorial(0) == 1);
    REQUIRE(cs1521::factorial(1) == 1);
    REQUIRE(cs1521::factorial(5) == 120);
    REQUIRE(cs1521::factorial(10) == 3628800);
    REQUIRE(cs1521::factorial(20) == 2432902008176640000LL);
}

TEST_CASE("factorial rejects values outside the domain", "[recursion]") {
    REQUIRE_THROWS_AS(cs1521::factorial(-1), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::factorial(21), std::overflow_error);
}

TEST_CASE("fibonacci base cases and memoized values", "[recursion]") {
    REQUIRE(cs1521::fibonacci(0) == 0);
    REQUIRE(cs1521::fibonacci(1) == 1);
    REQUIRE(cs1521::fibonacci(2) == 1);
    REQUIRE(cs1521::fibonacci(10) == 55);
    REQUIRE(cs1521::fibonacci(20) == 6765);
    // Naive recursion of fib(40) is exponential. Memoization makes this cheap.
    REQUIRE(cs1521::fibonacci(40) == 102334155);
}

TEST_CASE("fibonacci rejects values outside the domain", "[recursion]") {
    REQUIRE_THROWS_AS(cs1521::fibonacci(-1), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::fibonacci(93), std::overflow_error);
}

TEST_CASE("binary search base case is the empty range", "[recursion]") {
    const std::span<const int> empty;
    REQUIRE_FALSE(cs1521::binary_search(empty, 1).has_value());

    const std::array<int, 1> single{4};
    REQUIRE(cs1521::binary_search(std::span<const int>{single}, 4) == 0);
    REQUIRE_FALSE(cs1521::binary_search(std::span<const int>{single}, 5).has_value());
}

TEST_CASE("binary search finds elements in a sorted span", "[recursion]") {
    const std::vector<int> values{1, 3, 5, 7, 9};
    const std::span<const int> span{values};
    REQUIRE(cs1521::binary_search(span, 1) == 0);
    REQUIRE(cs1521::binary_search(span, 7) == 3);
    REQUIRE(cs1521::binary_search(span, 9) == 4);
    REQUIRE_FALSE(cs1521::binary_search(span, 0).has_value());
    REQUIRE_FALSE(cs1521::binary_search(span, 4).has_value());
    REQUIRE_FALSE(cs1521::binary_search(span, 10).has_value());
}

TEST_CASE("directory depth base case is a leaf", "[recursion]") {
    const cs1521::DirectoryNode leaf("leaf");
    REQUIRE(cs1521::directory_depth(leaf) == 0);
}

TEST_CASE("directory depth follows the longest in-memory branch", "[recursion]") {
    cs1521::DirectoryNode root("root");
    auto& src = root.add_child("src");
    src.add_child("demo.cpp");
    root.add_child("include");

    REQUIRE(cs1521::directory_depth(src) == 1);
    REQUIRE(cs1521::directory_depth(root) == 2);
    REQUIRE(root.name() == "root");
    REQUIRE(root.children().size() == 2);
}
