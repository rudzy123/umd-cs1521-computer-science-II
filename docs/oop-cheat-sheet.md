# OOP and memory cheat sheet

Compact syntax for the Day 2 types. The design notes match the code under `include/cs1521` and `src`.

## Class shape

```cpp
class ManagedBuffer {
public:
    explicit ManagedBuffer(std::size_t size, int fill = 0);
    ~ManagedBuffer();
    ManagedBuffer(const ManagedBuffer& other);
    ManagedBuffer& operator=(const ManagedBuffer& other);
    ManagedBuffer(ManagedBuffer&& other) noexcept;
    ManagedBuffer& operator=(ManagedBuffer&& other) noexcept;

private:
    std::unique_ptr<int[]> data_;
    std::size_t size_ = 0;
};
```

- `explicit` blocks `ManagedBuffer buffer = 4;`.
- One-argument constructors that are not a second name for the same type should be `explicit`.
- Members stay private. Callers use `at`, `size`, and `data`.

## Polymorphism

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
protected:
    Shape() = default;
};

class Circle final : public Shape {
public:
    explicit Circle(double radius);
    double area() const override;
};
```

```cpp
std::unique_ptr<Shape> shape = std::make_unique<Circle>(1.0);
double value = shape->area();  // calls Circle::area
```

| Keyword | Role |
| --- | --- |
| `virtual` | Dispatch uses the dynamic type |
| `= 0` | Pure virtual. The base cannot be constructed on its own |
| `override` | The signature must match a base virtual. A typo becomes a compile error |
| `final` | No further derived class, or no further override |
| virtual destructor | `delete` through `Shape*` runs the derived destructor |

Store polymorphic objects by `std::unique_ptr<Shape>` or bind a `Shape&`. A `Shape` value would slice off the derived part, and this base cannot be sliced because it is abstract.

## ADT versus representation

```cpp
template <typename T>
class Stack {
public:
    void push(T value);
    void pop();             // void: do not copy T after it has been removed
    T& top();
private:
    SinglyLinkedList<T> data_;  // composition, not inheritance
};
```

The ADT is the contract (LIFO). The list is one representation. Callers of `Stack` cannot walk the nodes. That is the abstraction.

## Rule of Zero, Three, and Five

```cpp
// Rule of Zero. The list already knows how to copy, move, and destroy.
Stack(const Stack&) = default;
Stack(Stack&&) noexcept = default;
~Stack() = default;

// Rule of Five. Copy is custom, so all five are written.
~ManagedBuffer();
ManagedBuffer(const ManagedBuffer&);
ManagedBuffer& operator=(const ManagedBuffer&);
ManagedBuffer(ManagedBuffer&&) noexcept;
ManagedBuffer& operator=(ManagedBuffer&&) noexcept;
```

Rule of Three is the C++98 set: destructor, copy constructor, copy assignment. Rule of Five adds move construction and move assignment. If you declare any of the five, declare or delete the rest. If the members already do the right thing, declare none of them.

`Shape` default all five because a virtual destructor is user-declared and that suppresses implicit moves. The bodies stay defaulted: the base owns nothing.

## Copy and move

```cpp
ManagedBuffer copy = original;             // second array, same values
ManagedBuffer moved = std::move(original); // steals the pointer; original is empty
```

Copy assignment allocates the replacement before it changes `*this`. If allocation throws, the target is unchanged. Move assignment is `noexcept` so standard containers can move instead of copy.

## Ownership

```cpp
auto node = std::make_unique<Node>(value); // sole owner
Node* raw = node.get();                    // non-owning observer, do not delete
std::unique_ptr<Shape> shape = std::make_unique<Circle>(1.0);

std::shared_ptr<Node> shared = std::make_shared<Node>(value);
std::weak_ptr<Node> observer = shared;     // does not keep the node alive
if (std::shared_ptr<Node> locked = observer.lock()) {
    // use locked
}
```

| Type | Owns? | May be null? | Copy |
| --- | --- | --- | --- |
| `T&` | No | No | Aliases the same object |
| `T*` non-owning | No | Yes | Copies the address |
| `unique_ptr<T>` | Sole owner | Yes | Move only |
| `shared_ptr<T>` | Shared | Yes | Increments the count |
| `weak_ptr<T>` | No | Expires | Observes a `shared_ptr` |

Prefer `unique_ptr`. Reach for `shared_ptr` only when the lifetime is shared. Use `weak_ptr` for the back edge of a cycle and for caches that must not keep an object alive.

## Recursion skeleton

```cpp
long long factorial(int n) {
    if (n < 0) throw std::invalid_argument("negative");
    if (n == 0 || n == 1) return 1;          // base case
    return static_cast<long long>(n) * factorial(n - 1);
}
```

Every recursive function in this repo has three parts: reject illegal input, stop at the base case, then reduce the problem. Memoize when the same arguments repeat, as Fibonacci does.

## Design rules used here

- A stack is not a list. Compose the list inside the stack.
- Do not return a removed element by value from `pop` or `dequeue`. Read `top` or `front`, then remove. A throwing copy cannot put the element back.
- Do not call `delete` on `tail_`, an iterator, or `ManagedBuffer::data()`.
- Do not `head_.reset()` a long chain. Unlink in a loop.
- Mark move operations `noexcept` when they only transfer pointers.
- Keep the element type of `ManagedBuffer` concrete so the special members stay in a `.cpp` file you can step through. The list is a template, so its bodies stay in the header.
