#pragma once

#include <cstddef>
#include <memory>
#include <utility>

namespace cs1521 {

// RAII buffer that shows every special member in a .cpp file.
// The element type is int on purpose: a class template would have to keep
// those bodies in a header. The array is owned by unique_ptr, so this class
// never calls delete. Custom copy is still required because unique_ptr is
// move-only and size_ must travel with the allocation.
class ManagedBuffer {
public:
    ManagedBuffer() = default;
    explicit ManagedBuffer(std::size_t size, int fill = 0);

    ~ManagedBuffer();

    ManagedBuffer(const ManagedBuffer& other);
    ManagedBuffer& operator=(const ManagedBuffer& other);

    ManagedBuffer(ManagedBuffer&& other) noexcept;
    ManagedBuffer& operator=(ManagedBuffer&& other) noexcept;

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

    // Non-owning view of the array owned by data_. Null when size() == 0.
    // Do not delete it. A move leaves the source null.
    [[nodiscard]] int* data() noexcept { return data_.get(); }
    [[nodiscard]] const int* data() const noexcept { return data_.get(); }

    [[nodiscard]] int& at(std::size_t index);
    [[nodiscard]] const int& at(std::size_t index) const;

    void swap(ManagedBuffer& other) noexcept;

private:
    std::unique_ptr<int[]> data_;
    std::size_t size_ = 0;
};

inline void swap(ManagedBuffer& lhs, ManagedBuffer& rhs) noexcept { lhs.swap(rhs); }

}  // namespace cs1521
