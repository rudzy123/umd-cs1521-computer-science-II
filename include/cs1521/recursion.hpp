#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace cs1521 {

// n == 0 and n == 1 are the base cases. Negative input and results that do
// not fit in a signed 64-bit integer throw.
[[nodiscard]] long long factorial(int n);

// Top-down recursion with memoization. fib(0) and fib(1) are the base cases.
// Overlapping subproblems are stored so the work is linear instead of exponential.
[[nodiscard]] long long fibonacci(int n);

// Recursive binary search over a span sorted in nondecreasing order.
// The empty half (low >= high) is the base case. An unsorted span has no
// promised result. The returned index refers into data and owns nothing.
template <typename T>
[[nodiscard]] std::optional<std::size_t> binary_search(std::span<const T> data, const T& target) {
    struct Search {
        static std::optional<std::size_t> run(std::span<const T> values, const T& key, std::size_t low,
                                              std::size_t high) {
            if (low >= high) {
                return std::nullopt;
            }
            const std::size_t mid = low + (high - low) / 2;
            if (values[mid] == key) {
                return mid;
            }
            if (key < values[mid]) {
                return run(values, key, low, mid);
            }
            return run(values, key, mid + 1, high);
        }
    };
    return Search::run(data, target, 0, data.size());
}

// In-memory tree used as a stand-in for a directory. Children live on the
// heap and are owned by unique_ptr. add_child returns a reference to the new
// node; that node stays put when the vector reallocates because the node
// itself is not stored inside the vector, only a pointer to it.
// The structure is a tree. A cycle would recurse forever.
class DirectoryNode {
public:
    explicit DirectoryNode(std::string name) : name_(std::move(name)) {}

    DirectoryNode(const DirectoryNode&) = delete;
    DirectoryNode& operator=(const DirectoryNode&) = delete;
    DirectoryNode(DirectoryNode&&) noexcept = default;
    DirectoryNode& operator=(DirectoryNode&&) noexcept = default;
    // Defined out of line so a deep chain is destroyed iteratively. A defaulted
    // destructor would recurse through unique_ptr and can overflow the stack.
    ~DirectoryNode();

    [[nodiscard]] const std::string& name() const noexcept { return name_; }

    DirectoryNode& add_child(std::string child_name) {
        children_.push_back(std::make_unique<DirectoryNode>(std::move(child_name)));
        return *children_.back();
    }

    [[nodiscard]] const std::vector<std::unique_ptr<DirectoryNode>>& children() const noexcept {
        return children_;
    }

private:
    std::string name_;
    std::vector<std::unique_ptr<DirectoryNode>> children_;
};

// Longest path in edges. A node with no children is the base case and has depth 0.
[[nodiscard]] std::size_t directory_depth(const DirectoryNode& node);

}  // namespace cs1521
