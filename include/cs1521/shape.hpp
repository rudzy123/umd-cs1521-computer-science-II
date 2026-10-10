#pragma once

#include <string_view>

namespace cs1521 {

// Polymorphic base. The virtual destructor is required so that deleting a
// derived object through Shape* runs the derived destructor.
// Declaring that destructor suppresses implicit moves, so the copy and move
// operations are defaulted here as well (Rule of Five). Shape owns no
// resource; the defaulted bodies are empty.
class Shape {
public:
    virtual ~Shape() = default;

    Shape(const Shape&) = default;
    Shape& operator=(const Shape&) = default;
    Shape(Shape&&) noexcept = default;
    Shape& operator=(Shape&&) noexcept = default;

    [[nodiscard]] virtual double area() const = 0;
    [[nodiscard]] virtual double perimeter() const = 0;
    [[nodiscard]] virtual std::string_view name() const = 0;

protected:
    Shape() = default;
};

// Concrete shapes follow the Rule of Zero: doubles need no custom special members.
class Circle final : public Shape {
public:
    explicit Circle(double radius);

    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimeter() const override;
    [[nodiscard]] std::string_view name() const override;
    [[nodiscard]] double radius() const noexcept { return radius_; }

private:
    double radius_;
};

class Rectangle final : public Shape {
public:
    Rectangle(double width, double height);

    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimeter() const override;
    [[nodiscard]] std::string_view name() const override;
    [[nodiscard]] double width() const noexcept { return width_; }
    [[nodiscard]] double height() const noexcept { return height_; }

private:
    double width_;
    double height_;
};

class Triangle final : public Shape {
public:
    Triangle(double side_a, double side_b, double side_c);

    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimeter() const override;
    [[nodiscard]] std::string_view name() const override;

private:
    double side_a_;
    double side_b_;
    double side_c_;
};

// Calls area() through Shape. The dynamic type selects the override.
template <typename Range>
[[nodiscard]] double total_area(const Range& shapes) {
    double sum = 0.0;
    for (const auto& shape : shapes) {
        sum += shape->area();
    }
    return sum;
}

}  // namespace cs1521
