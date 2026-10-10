# Learning log

Use this file as a lab notebook for Day 2. Write short answers in your own words after you have run the tests. The goal is to say where each object lives and who frees it.

## Encapsulation and abstraction

Where is a list node visible to a caller of `Stack`? What would you give up if `Stack` inherited from `SinglyLinkedList` and exposed iterators?

(write here)

## Inheritance and polymorphism

Which function runs for `shape->area()` when `shape` has type `std::unique_ptr<Shape>` and owns a `Triangle`? What fails at compile time if you try to store a `Triangle` in a `Shape` variable?

(write here)

## Recursion and base cases

For each of `factorial`, `fibonacci`, `binary_search`, and `directory_depth`, write the base case and what goes wrong if it is removed.

(write here)

Why does memoized `fibonacci(40)` finish immediately, while an unmemoized version does not?

(write here)

## Copy and move

After `ManagedBuffer moved = std::move(original)`, which object owns the heap array? What is a legal operation on `original`? Draw the before and after, or point at the diagram in `docs/memory-model.md` that matches the test you just ran.

(write here)

## Rules of zero, three, and five

Which types in this repository use the Rule of Zero, and which use the Rule of Five? Why does declaring only a destructor force the rest of the decision?

(write here)

## Ownership

`tail_` is a raw pointer. Who owns the node it points at, and why is it set to null in the move operations?

(write here)

## Sanitizer findings

Record the command, the compiler, and whether AddressSanitizer or UndefinedBehaviorSanitizer reported anything. Paste a report here if one fires, then write the fix under it.

| Date | Build | Command | Result |
| --- | --- | --- | --- |
| 2026-10-10 | Debug + ASan and UBSan, GCC 15.2.0 | `ctest --test-dir build-san --output-on-failure` | Passed. No leak and no undefined behavior. |

### How to read a report

- `heap-use-after-free` means a non-owning pointer outlived the `unique_ptr` that owned the object.
- `heap-buffer-overflow` means an index walked off an array. `ManagedBuffer::at` should have thrown first.
- `detected memory leaks` means an owning pointer was dropped without a destructor or a `unique_ptr`.
- `signed integer overflow` means a recursive multiply passed the 64-bit range. `factorial` and `fibonacci` are supposed to throw before that happens.
- A stack overflow during destruction of a long chain means a `unique_ptr` chain was released recursively. `clear` and `DirectoryNode::~DirectoryNode` are written to avoid that.

### Notes

(paste the sanitizer report here if one fires, and the fix)

### Reference validation

Re-run the commands before you trust an old row. If your row disagrees with this one, keep your paste: your flags or edit are the interesting part.

| Date | Compiler | Build | Result |
| --- | --- | --- | --- |
| 2026-10-10 | GCC 15.2.0 | Release, `ctest --test-dir build` | 26 test cases, 172 assertions, passed |
| 2026-10-10 | GCC 15.2.0 | Debug + ASan + UBSan, `ctest --test-dir build-san` | 26 test cases, 172 assertions, passed. LeakSanitizer and UBSan reported nothing |

LeakSanitizer aborts with "does not work under ptrace" when a debugger or a sandbox is attached. That message is not a leak. Run the sanitizer binary in a normal process and read the report it prints, if any.
