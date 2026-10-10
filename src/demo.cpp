#include "cs1521/linked_list.hpp"
#include "cs1521/managed_buffer.hpp"
#include "cs1521/queue.hpp"
#include "cs1521/recursion.hpp"
#include "cs1521/shape.hpp"
#include "cs1521/stack.hpp"

#include <iostream>
#include <memory>
#include <span>
#include <vector>

int main() {
    cs1521::SinglyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    std::cout << "list:";
    for (const int value : list) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';

    cs1521::Stack<int> stack;
    stack.push(10);
    stack.push(30);
    std::cout << "stack top: " << stack.top() << '\n';

    cs1521::Queue<int> queue;
    queue.enqueue(10);
    queue.enqueue(20);
    std::cout << "queue front: " << queue.front() << '\n';

    std::vector<std::unique_ptr<cs1521::Shape>> shapes;
    shapes.push_back(std::make_unique<cs1521::Circle>(1.0));
    shapes.push_back(std::make_unique<cs1521::Rectangle>(3.0, 4.0));
    shapes.push_back(std::make_unique<cs1521::Triangle>(3.0, 4.0, 5.0));
    std::cout << "shapes:";
    for (const auto& shape : shapes) {
        std::cout << ' ' << shape->name() << '=' << shape->area();
    }
    std::cout << "\ntotal area: " << cs1521::total_area(shapes) << '\n';

    std::cout << "factorial(5): " << cs1521::factorial(5) << '\n';
    std::cout << "fibonacci(10): " << cs1521::fibonacci(10) << '\n';

    const int sorted[] = {1, 3, 5, 7, 9};
    const auto index = cs1521::binary_search(std::span<const int>{sorted}, 7);
    std::cout << "binary search index: " << index.value() << '\n';

    cs1521::DirectoryNode root("root");
    auto& src = root.add_child("src");
    src.add_child("demo.cpp");
    root.add_child("include");
    std::cout << "directory depth: " << cs1521::directory_depth(root) << '\n';

    cs1521::ManagedBuffer source(4, 8);
    cs1521::ManagedBuffer destination(std::move(source));
    std::cout << "buffer size after move: source=" << source.size()
              << " destination=" << destination.size() << '\n';
    return 0;
}
