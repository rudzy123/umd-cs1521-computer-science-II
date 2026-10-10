#include "cs1521/managed_buffer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <utility>

TEST_CASE("empty buffer owns nothing", "[buffer]") {
    const cs1521::ManagedBuffer buffer;
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.data() == nullptr);
    REQUIRE_THROWS_AS(buffer.at(0), std::out_of_range);
}

TEST_CASE("buffer copy is independent and move transfers ownership", "[buffer]") {
    cs1521::ManagedBuffer original(3, 1);
    REQUIRE(original.size() == 3);
    REQUIRE(original.at(0) == 1);
    REQUIRE(original.at(2) == 1);
    original.at(0) = 4;

    cs1521::ManagedBuffer copy = original;
    REQUIRE(copy.data() != original.data());
    REQUIRE(copy.at(0) == 4);
    copy.at(0) = 8;
    REQUIRE(original.at(0) == 4);

    original = original;
    REQUIRE(original.at(1) == 1);
    REQUIRE(original.size() == 3);
    cs1521::ManagedBuffer* alias = &original;
    *alias = std::move(original);
    REQUIRE(original.size() == 3);
    REQUIRE(original.at(0) == 4);

    cs1521::ManagedBuffer shorter(1, 9);
    original = shorter;
    REQUIRE(original.size() == 1);
    REQUIRE(original.at(0) == 9);
    REQUIRE(shorter.at(0) == 9);

    cs1521::ManagedBuffer moved(std::move(copy));
    REQUIRE(copy.size() == 0);
    REQUIRE(copy.data() == nullptr);
    REQUIRE(moved.size() == 3);
    REQUIRE(moved.at(0) == 8);
    REQUIRE(moved.data() != nullptr);

    cs1521::ManagedBuffer assigned(2, 5);
    assigned = std::move(moved);
    REQUIRE(moved.empty());
    REQUIRE(moved.data() == nullptr);
    REQUIRE(assigned.size() == 3);
    REQUIRE(assigned.at(2) == 1);
    REQUIRE_THROWS_AS(assigned.at(3), std::out_of_range);
}
