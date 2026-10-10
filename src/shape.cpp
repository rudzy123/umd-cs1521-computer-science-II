#include "cs1521/shape.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace cs1521 {
namespace {

void require_non_negative(double value, const char* description) {
    // The comparison rejects NaN as well as negatives.
    if (!(value >= 0.0)) {
        throw std::invalid_argument(description);
    }
}

}  // namespace

Circle::Circle(double radius) : radius_(radius) {
    require_non_negative(radius_, "circle radius must be non-negative");
}

double Circle::area() const { return std::numbers::pi * radius_ * radius_; }

double Circle::perimeter() const { return 2.0 * std::numbers::pi * radius_; }

std::string_view Circle::name() const { return "circle"; }

Rectangle::Rectangle(double width, double height) : width_(width), height_(height) {
    require_non_negative(width_, "rectangle width must be non-negative");
    require_non_negative(height_, "rectangle height must be non-negative");
}

double Rectangle::area() const { return width_ * height_; }

double Rectangle::perimeter() const { return 2.0 * (width_ + height_); }

std::string_view Rectangle::name() const { return "rectangle"; }

Triangle::Triangle(double side_a, double side_b, double side_c)
    : side_a_(side_a), side_b_(side_b), side_c_(side_c) {
    require_non_negative(side_a_, "triangle side must be positive");
    require_non_negative(side_b_, "triangle side must be positive");
    require_non_negative(side_c_, "triangle side must be positive");
    if (!(side_a_ > 0.0 && side_b_ > 0.0 && side_c_ > 0.0)) {
        throw std::invalid_argument("triangle sides must be positive");
    }
    const bool violates_inequality = side_a_ + side_b_ <= side_c_ || side_a_ + side_c_ <= side_b_ ||
                                     side_b_ + side_c_ <= side_a_;
    if (violates_inequality) {
        throw std::invalid_argument("triangle sides violate the triangle inequality");
    }
}

double Triangle::area() const {
    const double semi = (side_a_ + side_b_ + side_c_) / 2.0;
    const double area_squared = semi * (semi - side_a_) * (semi - side_b_) * (semi - side_c_);
    if (area_squared <= 0.0) {
        return 0.0;
    }
    return std::sqrt(area_squared);
}

double Triangle::perimeter() const { return side_a_ + side_b_ + side_c_; }

std::string_view Triangle::name() const { return "triangle"; }

}  // namespace cs1521
