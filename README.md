# UMD CS 1521 — Computer Science II, Day 2

Object-oriented programming, recursion, abstract data types, and safe memory management in C++20.

This is a small library you can build, test, and step through. The list, stack, and queue are templates in headers. The shape hierarchy, the memoized recursion, and the Rule of Five buffer are ordinary `.cpp` files so a debugger can land in them.

## Map

| Path | What it teaches |
| --- | --- |
| `include/cs1521/linked_list.hpp` | Owning nodes, a non-owning tail, iteration, copy, and move |
| `include/cs1521/stack.hpp` | LIFO ADT built by composition |
| `include/cs1521/queue.hpp` | FIFO ADT, O(1) at both ends |
| `include/cs1521/shape.hpp`, `src/shape.cpp` | Abstract base, overrides, `unique_ptr<Shape>` |
| `include/cs1521/recursion.hpp`, `src/recursion.cpp` | Factorial, memoized Fibonacci, binary search, directory depth |
| `include/cs1521/managed_buffer.hpp`, `src/managed_buffer.cpp` | Constructor, destructor, copy, move, RAII |
| `tests/` | Empty containers, copy and move, dispatch, base cases, exceptions |
| `docs/memory-model.md` | Stack frames, heap nodes, ownership transfer |
| `docs/oop-cheat-sheet.md` | Syntax and the design rules used here |
| `docs/learning-log.md` | Prompts and a place to record sanitizer output |

## Encapsulation, abstraction, inheritance, polymorphism

Encapsulation is the private state of a type. `SinglyLinkedList` hides `Node`. Callers use `push_back`, `find`, and iterators. They cannot rewire a `next` pointer.

Abstraction is the contract you can use without knowing the representation. `Stack` promises last-in, first-out. It does not promise that the storage is a linked list. The list is a private member. Inheritance would have been the wrong tool: a stack is not a kind of list, and a public list interface would let callers break LIFO.

Inheritance is the relationship `Circle` has to `Shape`. The derived class is a `Shape` and adds the data the formula needs (a radius, two sides, three sides).

Polymorphism is the call that uses the dynamic type. `Shape::area` is pure virtual. `Circle::area`, `Rectangle::area`, and `Triangle::area` override it. This call runs the override for whatever object the pointer owns:

```cpp
std::unique_ptr<Shape> shape = std::make_unique<Triangle>(3.0, 4.0, 5.0);
double value = shape->area();
```

The same thing happens through a `Shape&` bound to a stack `Rectangle`. Dispatch depends on the object's type, not on whether the object was allocated with `new`. The pointer is how we keep a collection of different shapes. `total_area` walks that collection and calls `area()` on each element.

`Shape` has a virtual destructor. Destroying a `unique_ptr<Shape>` that owns a `Circle` then runs `~Circle` before the base destructor. The tests use a counting derived type to show the derived destructor actually runs.

## ADTs versus concrete data structures

An abstract data type is a set of operations and their meaning. A concrete data structure is a layout in memory.

| ADT | Contract | Concrete structure in this repo |
| --- | --- | --- |
| List | Insert, remove, look up, walk, report size | Singly linked nodes on the heap |
| Stack | `push`, `pop`, `top`, LIFO | The list, using the front as the top |
| Queue | `enqueue`, `dequeue`, `front`, FIFO | The list, back in and front out |

The same stack contract could be a dynamic array. The observable order would not change. The complexity of some operations would. That split is why `Stack` and `Queue` do not expose iterators. Walking the middle of a stack is not part of the contract.

`pop` and `dequeue` return `void`. The C++ standard containers do the same. If `pop` removed the element and then returned it by value, a throwing copy would lose the value. Read `top` or `front`, then remove.

## Stack and heap

The call stack holds a frame per active function: parameters, return address, and local variables. `factorial(3)` has three frames at its deepest point. They disappear on return. Diagrams are in `docs/memory-model.md`.

The heap holds memory whose size or lifetime is not tied to one frame. List nodes, directory children, shape objects owned by `unique_ptr`, the Fibonacci memo table, and the `ManagedBuffer` array all live there.

A `SinglyLinkedList` local variable is on the stack. Its nodes are on the heap. The stack object is small and fixed size. The heap chain grows with `push_back`. When the list's destructor runs, it frees the chain. You do not free the nodes yourself.

## Pointers, references, and smart pointers

A reference (`T&` or `const T&`) is another name for an existing object. It is not null and it does not own. `front()` returns a reference into a node the list owns. That reference is invalid after the node is removed.

A raw pointer may be null. In this repository every raw pointer is non-owning, and the declaration says so.

- `tail_` observes the last node so `push_back` is O(1). The node is owned by `head_` or by the previous node's `next`.
- Iterators store a `Node*`. Copying an iterator copies the address.
- `ManagedBuffer::data()` returns `int*` as a view of the array `data_` owns. Do not `delete` it.
- `previous` and `current` inside `remove` are cursors used to unlink a node. The `unique_ptr` assignment does the delete.

`std::unique_ptr<T>` is the sole owner. It is move-only. When it is destroyed, reset, or overwritten, it deletes the object. `std::make_unique` is the allocation path used here. There is no owning `new` or `delete` in the project.

`std::shared_ptr<T>` is for a resource with several owners. Copying it increments a count in a control block. The last owner deletes the object. This code does not need shared ownership. Shapes, nodes, and directory children each have one owner.

`std::weak_ptr<T>` observes a `shared_ptr` without incrementing the count. `lock()` either gives you a temporary `shared_ptr` or tells you the object is gone. It is how you break a cycle: parent owns the child with `shared_ptr`, child points back with `weak_ptr`. A cycle of `shared_ptr` never reaches a count of zero, so the objects leak.

## Recursion and base cases

A recursive function must shrink the problem and must have a case that stops.

| Function | Base case | Reduction |
| --- | --- | --- |
| `factorial` | `n == 0` or `n == 1` returns 1 | `n * factorial(n - 1)` |
| `fibonacci` | `n == 0` returns 0, `n == 1` returns 1 | sum of the two previous values, stored in a memo table |
| `binary_search` | empty half (`low >= high`) returns "not found" | search the left or right half |
| `directory_depth` | a node with no children has depth 0 | one plus the deepest child |

Negative inputs throw `std::invalid_argument`. `factorial` above 20 and `fibonacci` above 92 throw `std::overflow_error` before a signed 64-bit multiply overflows. Signed overflow is undefined behavior; the checks exist so UndefinedBehaviorSanitizer stays quiet and the failure is an exception you can test.

`directory_depth` walks an in-memory tree, `DirectoryNode`, not the real filesystem. Children are `unique_ptr`s. Depth is the number of edges on the longest path. A cycle is not representable through `add_child` and would recurse forever if one were introduced by hand.

Binary search requires a span sorted in nondecreasing order. The function does not scan for that property. An unsorted span has no promised index.

## Rule of Zero, Rule of Three, Rule of Five

Rule of Zero: if your members already copy, move, and destroy correctly, write none of the special members. `Stack` and `Queue` only contain a `SinglyLinkedList`. `Circle`, `Rectangle`, and `Triangle` only contain `double`s. The compiler-generated operations are the right ones.

Rule of Three is the older set, from a language without moves: if you write a destructor, a copy constructor, or a copy assignment operator, you write all three. You needed them when a class called `new` and had to pair it with `delete` and a deep copy.

Rule of Five adds the move constructor and the move assignment operator. If you declare any of the five, declare or `= delete` the rest. A user-declared destructor suppresses implicit moves. Leaving the moves undeclared makes "move" fall back to copy, or makes the type immovable, depending on what else you declared.

`ManagedBuffer` follows the Rule of Five. `unique_ptr` would be enough for destruction and moving, but it cannot copy, and `size_` has to stay paired with the array. The copy constructor allocates a new array. Copy assignment copies first and then swaps, so a throwing allocation leaves the target alone. The move operations transfer `data_` and set the source size to 0. The destructor resets the `unique_ptr`. There is still no `delete[]`. The destructor is written out so the ownership boundary is visible in `src/managed_buffer.cpp`.

`SinglyLinkedList` follows the Rule of Five for a second reason. A `unique_ptr` head would make the list move-only and would destroy a long chain recursively. The class defines a deep copy, an iterative `clear`, and move operations that null the non-owning `tail_`.

`Shape` default all five. The virtual destructor must be declared so deletion through a base pointer is safe. Declaring it means the copy and move operations should be declared too. They are `= default` because the base holds no resource.

## Copy versus move

Copy duplicates the resource. After `SinglyLinkedList<int> copy = original`, both lists are equal and they do not share nodes. Mutating `copy` does not change `original`.

Move transfers the resource. After `SinglyLinkedList<int> moved = std::move(original)`, `moved` owns the nodes and `original` is empty. The node objects stay where they were on the heap. Only the owning pointer and the non-owning tail move. Move is O(1) for the list and the buffer. Copy is O(n).

A moved-from object is valid and empty. Destroy it, assign to it, or `push` onto it. Do not read its old elements.

Self-assignment and self-move are guarded. Copy assignment of a list also goes through a copy, so self-assignment does not clear the object before the copy is finished.

## Common ownership errors

These are the failures this design is built to make hard to write.

Owning raw memory, then leaking it or freeing it twice:

```cpp
int* buffer = new int[n];
delete[] buffer;
delete[] buffer;  // undefined
```

The repair is `std::unique_ptr<int[]> buffer = std::make_unique<int[]>(n);`. The destructor frees it once.

`delete` on a non-owning pointer. `tail_`, iterators, and `ManagedBuffer::data()` do not own. Deleting them frees memory the `unique_ptr` will free again.

A dangling observer. Move operations set `tail_` to null and leave iterators conceptually invalid. Holding an iterator across `clear` is a use-after-free. AddressSanitizer reports it.

Returning the address of a local. The frame is gone when the function returns. Return by value and let move transfer ownership, or return a `unique_ptr`.

A missing virtual destructor. `unique_ptr<Shape>` would destroy only the base subobject if `~Shape` were not virtual. The derived part would leak or worse. `~Shape` is virtual.

Object slicing. `Shape shape = circle;` does not compile because `Shape` is abstract. If it did, `shape` would not be a circle. Keep the derived object and refer to it through `Shape&` or `unique_ptr<Shape>`.

A `shared_ptr` cycle. Each object keeps the other alive. The back pointer should be a `weak_ptr`.

Recursive destruction of a long `unique_ptr` chain. `head_.reset()` on a list of hundreds of thousands of nodes can overflow the call stack inside destructors. `clear` unlinks one node at a time. `DirectoryNode` does the same for a deep tree.

Signed overflow in a recursive multiply. That is undefined behavior, not wraparound. The factorial and Fibonacci entry points throw `std::overflow_error` first.

## Complexity

`n` is the number of elements.

| Operation | List | Stack | Queue |
| --- | --- | --- | --- |
| Push front / push | O(1) | O(1) push | — |
| Push back / enqueue | O(1) with `tail_` | — | O(1) enqueue |
| Pop front / pop / dequeue | O(1) | O(1) pop | O(1) dequeue |
| Front, back, top | O(1) | O(1) top | O(1) front |
| Insert or erase at index | O(n) | — | — |
| Remove by value, find, contains | O(n) | — | — |
| Size, empty | O(1), cached | O(1) | O(1) |
| Clear | O(n) destructors | O(n) | O(n) |
| Copy | O(n) | O(n) | O(n) |
| Move | O(1) | O(1) | O(1) |

Without `tail_`, `push_back` and queue `enqueue` would be O(n). The non-owning tail is why the queue meets its complexity contract.

Recursive costs:

| Function | Time | Extra space |
| --- | --- | --- |
| `factorial` | O(n) | O(n) stack frames |
| memoized `fibonacci` | O(n) | O(n) memo on the heap and O(n) frames on the first descent |
| `binary_search` | O(log n) | O(log n) frames |
| `directory_depth` | O(n) nodes | O(height) frames |

Unmemoized Fibonacci is exponential. The test of `fibonacci(40)` is the demonstration: it returns 102334155 immediately because each `n` is computed once.

## Build, run, test, sanitize, debug

Requirements: a C++20 compiler (GCC or Clang), CMake 3.16 or newer, and network access on the first configure so CMake can fetch Catch2 v3.8.1.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/cs1521_demo
ctest --test-dir build --output-on-failure
```

Debug build, suitable for stepping:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
gdb --args ./build-debug/cs1521_tests "empty stack"
```

Inside gdb, `catch throw` stops on the underflow exceptions, and `break cs1521::factorial` stops on the recursive function. Catch2 takes a test-name substring as its filter.

Sanitizers are a CMake option, not a different source tree. AddressSanitizer and UndefinedBehaviorSanitizer instrument every project target, including the tests. They are supported for GCC and Clang. The Linux CI job runs this configuration.

```bash
cmake -S . -B build-san -DCMAKE_BUILD_TYPE=Debug -DCS1521_ENABLE_SANITIZERS=ON
cmake --build build-san
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:symbolize=1 \
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
ctest --test-dir build-san --output-on-failure
```

A sanitizer failure is a test failure. Leak detection is on by default for AddressSanitizer on Linux. Paste the report into `docs/learning-log.md` and fix the owner, not the symptom. Do not suppress a report to make the run green.

Format the C++ sources with the repo's style file:

```bash
clang-format -i include/cs1521/*.hpp src/*.cpp tests/*.cpp
```

## Tests

| File | Coverage |
| --- | --- |
| `tests/test_linked_list.cpp` | Empty list, insert, remove, lookup, traversal, deep copy, move, long-chain `clear`, element lifetimes |
| `tests/test_stack.cpp` | Empty `pop` and `top` throw `std::underflow_error`, LIFO, copy, move |
| `tests/test_queue.cpp` | Empty `dequeue` and `front` throw `std::underflow_error`, FIFO, copy, move |
| `tests/test_shapes.cpp` | Areas, invalid dimensions, virtual dispatch, destruction through `Shape` |
| `tests/test_recursion.cpp` | Base cases, overflow guards, binary search misses, directory depth |
| `tests/test_managed_buffer.cpp` | Deep copy, distinct `data()` pointers, move leaves the source empty |

## Limits

The list is singly linked and not thread-safe. Fibonacci is top-down with a memo table, not an iterative loop. Directory depth is an in-memory tree, not `std::filesystem`. `ManagedBuffer` is an `int` array so its special members fit in one `.cpp` file. The list is the generic container.

## License

MIT. See `LICENSE`.
