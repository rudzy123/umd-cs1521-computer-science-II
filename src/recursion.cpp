#include "cs1521/recursion.hpp"

#include <stdexcept>
#include <utility>
#include <vector>

namespace cs1521 {
namespace {

constexpr int kMaxFactorialArgument = 20;
constexpr int kMaxFibonacciArgument = 92;

long long fibonacci_memo(int n, std::vector<long long>& memo) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    const std::size_t index = static_cast<std::size_t>(n);
    if (memo[index] != -1) {
        return memo[index];
    }
    memo[index] = fibonacci_memo(n - 1, memo) + fibonacci_memo(n - 2, memo);
    return memo[index];
}

}  // namespace

DirectoryNode::~DirectoryNode() {
    std::vector<std::unique_ptr<DirectoryNode>> pending;
    pending.swap(children_);
    while (!pending.empty()) {
        std::unique_ptr<DirectoryNode> node = std::move(pending.back());
        pending.pop_back();
        for (auto& child : node->children_) {
            pending.push_back(std::move(child));
        }
        node->children_.clear();
    }
}

long long factorial(int n) {
    if (n < 0) {
        throw std::invalid_argument("factorial is undefined for negative values");
    }
    if (n > kMaxFactorialArgument) {
        throw std::overflow_error("factorial does not fit in a 64-bit signed integer");
    }
    if (n == 0 || n == 1) {
        return 1;
    }
    return static_cast<long long>(n) * factorial(n - 1);
}

long long fibonacci(int n) {
    if (n < 0) {
        throw std::invalid_argument("fibonacci is undefined for negative values");
    }
    if (n > kMaxFibonacciArgument) {
        throw std::overflow_error("fibonacci does not fit in a 64-bit signed integer");
    }
    std::vector<long long> memo(static_cast<std::size_t>(n) + 1, -1);
    return fibonacci_memo(n, memo);
}

std::size_t directory_depth(const DirectoryNode& node) {
    std::size_t deepest = 0;
    for (const auto& child : node.children()) {
        // child owns the node. The reference is a non-owning use of that object.
        const std::size_t through_child = directory_depth(*child) + 1;
        if (through_child > deepest) {
            deepest = through_child;
        }
    }
    return deepest;
}

}  // namespace cs1521
