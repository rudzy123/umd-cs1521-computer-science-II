#include "cs1521/shape.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <numbers>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class CountingShape final : public cs1521::Shape {
public:
    CountingShape() { ++alive; }
    ~CountingShape() override { --alive; }

    CountingShape(const CountingShape&) = delete;
    CountingShape& operator=(const CountingShape&) = delete;
    CountingShape(CountingShape&&) = delete;
    CountingShape& operator=(CountingShape&&) = delete;

    [[nodiscard]] double area() const override { return 1.0; }
    [[nodiscard]] double perimeter() const override { return 1.0; }
    [[nodiscard]] std::string_view name() const override { return "counting"; }

    static inline int alive = 0;
};

}  // namespace

TEST_CASE("concrete shapes compute area and perimeter", "[shape]") {
    const cs1521::Circle circle(1.0);
    REQUIRE(circle.area() == Catch::Approx(std::numbers::pi));
    REQUIRE(circle.perimeter() == Catch::Approx(2.0 * std::numbers::pi));
    REQUIRE(circle.radius() == Catch::Approx(1.0));
    REQUIRE(circle.name() == "circle");

    const cs1521::Rectangle rectangle(3.0, 4.0);
    REQUIRE(rectangle.width() == Catch::Approx(3.0));
    REQUIRE(rectangle.height() == Catch::Approx(4.0));
    REQUIRE(rectangle.area() == Catch::Approx(12.0));
    REQUIRE(rectangle.perimeter() == Catch::Approx(14.0));

    const cs1521::Triangle triangle(3.0, 4.0, 5.0);
    REQUIRE(triangle.area() == Catch::Approx(6.0));
    REQUIRE(triangle.perimeter() == Catch::Approx(12.0));
    REQUIRE(triangle.name() == "triangle");
}

TEST_CASE("invalid dimensions are rejected", "[shape]") {
    REQUIRE_THROWS_AS(cs1521::Circle(-1.0), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::Rectangle(-2.0, 4.0), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::Rectangle(2.0, -4.0), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::Triangle(1.0, 1.0, 3.0), std::invalid_argument);
    REQUIRE_THROWS_AS(cs1521::Triangle(0.0, 1.0, 1.0), std::invalid_argument);
}

TEST_CASE("virtual calls dispatch through base handles", "[shape]") {
    const cs1521::Rectangle rectangle(2.0, 5.0);
    const cs1521::Shape& as_shape = rectangle;
    REQUIRE(as_shape.name() == "rectangle");
    REQUIRE(as_shape.area() == Catch::Approx(10.0));

    std::vector<std::unique_ptr<cs1521::Shape>> shapes;
    shapes.push_back(std::make_unique<cs1521::Circle>(2.0));
    shapes.push_back(std::make_unique<cs1521::Rectangle>(2.0, 5.0));
    shapes.push_back(std::make_unique<cs1521::Triangle>(3.0, 4.0, 5.0));

    REQUIRE(shapes[0]->name() == "circle");
    REQUIRE(shapes[1]->perimeter() == Catch::Approx(14.0));
    REQUIRE(shapes[2]->area() == Catch::Approx(6.0));

    const double expected = 4.0 * std::numbers::pi + 10.0 + 6.0;
    REQUIRE(cs1521::total_area(shapes) == Catch::Approx(expected));

    auto moved = std::move(shapes);
    REQUIRE(shapes.empty());
    REQUIRE(moved.front()->area() == Catch::Approx(4.0 * std::numbers::pi));
}

TEST_CASE("deleting through the base runs the derived destructor", "[shape]") {
    {
        const std::unique_ptr<cs1521::Shape> shape = std::make_unique<CountingShape>();
        REQUIRE(CountingShape::alive == 1);
        REQUIRE(shape->name() == "counting");
    }
    REQUIRE(CountingShape::alive == 0);
}
