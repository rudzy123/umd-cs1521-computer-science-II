#include "cs1521/managed_buffer.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace cs1521 {

ManagedBuffer::ManagedBuffer(std::size_t size, int fill) : data_(nullptr), size_(size) {
    if (size_ == 0) {
        return;
    }
    data_ = std::make_unique<int[]>(size_);
    std::fill_n(data_.get(), size_, fill);
}

ManagedBuffer::~ManagedBuffer() {
    // unique_ptr frees the array. This destructor exists so the ownership
    // boundary is visible, and so the class follows the Rule of Five once
    // the other special members are user-declared. There is no delete[].
    data_.reset();
    size_ = 0;
}

ManagedBuffer::ManagedBuffer(const ManagedBuffer& other) : data_(nullptr), size_(other.size_) {
    if (size_ == 0) {
        return;
    }
    data_ = std::make_unique<int[]>(size_);
    std::copy_n(other.data_.get(), size_, data_.get());
}

ManagedBuffer& ManagedBuffer::operator=(const ManagedBuffer& other) {
    if (this == &other) {
        return *this;
    }
    // Copy first. If allocation throws, *this is unchanged.
    ManagedBuffer copy(other);
    swap(copy);
    return *this;
}

ManagedBuffer::ManagedBuffer(ManagedBuffer&& other) noexcept
    : data_(std::move(other.data_)), size_(std::exchange(other.size_, 0)) {}

ManagedBuffer& ManagedBuffer::operator=(ManagedBuffer&& other) noexcept {
    if (this != &other) {
        data_ = std::move(other.data_);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}

int& ManagedBuffer::at(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("ManagedBuffer index is out of range");
    }
    return data_[index];
}

const int& ManagedBuffer::at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("ManagedBuffer index is out of range");
    }
    return data_[index];
}

void ManagedBuffer::swap(ManagedBuffer& other) noexcept {
    data_.swap(other.data_);
    std::swap(size_, other.size_);
}

}  // namespace cs1521
