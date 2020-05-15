#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

namespace collection {

    template<typename T>
    class Vector {
        typedef std::iterator_traits<T*> iteratorTraits;

        typedef T ValueType;
        typedef T& reference;
        typedef T const& ConstReference;
        typedef T&& RValueReference;
        typedef T* Pointer;
        typedef const Pointer ConstPointer;
        typedef Pointer Iterator;
        typedef ConstPointer ConstIterator;

        static_assert(std::is_assignable<Iterator, ConstIterator>::value,
                      "Iterator should be assignable from ConstIterator");

    protected:

        static constexpr size_t DEFAULT_CAPACITY = 16;

        Pointer array_;
        size_t size_, capacity_;

    protected:

        inline void copyArrayNoChecks(ConstPointer originalArray, size_t const size) {
            std::copy(originalArray, originalArray + size, array_);
        }

        inline void checkRange(size_t const index) {
            if (index >= size_) throw std::out_of_range(
                    "Index " + std::to_string(index) + " should be less than size " + std::to_string(size_));
                    "Index " + std::to_string(index) + " should be less than size " + std::to_string(size_));
        }

        Vector(size_t const size) : array_(new T[size]), size_(size), capacity_(size) {}

    public:
        /* ********************************************** Constructors ********************************************** */

        Vector() : Vector(DEFAULT_CAPACITY) {}

        Vector(Vector const& original) : Vector(original.size_) {
            copyArrayNoChecks(original.array_);
        }

        Vector(Vector&& original) noexcept
            : array_(std::exchange(original.array_, nullptr)),
              size_(std::exchange(original.size_, 0)),
              capacity_(std::exchange(original.capacity_, 0)) {}

        /* ****************************************** Assignment operators ****************************************** */

        Vector& operator=(Vector const& other) {
            if (this != *other) {
                if (size_ < other.size_) {
                    array_ = new T[other.size_];
                    size_ = other.size_;
                    capacity_ = other.capacity_;
                }
                copyArrayNoChecks(other.array_, other.size_);
            }

            return *this;
        }

        Vector& operator=(Vector&& other) noexcept {
            std::swap(array_, other.array_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);

            return *this;
        }

        /* ********************************************* Indexed access ********************************************* */

        reference operator[](size_t index) {
            return array_[index];
        }

        ConstReference operator[](size_t index) const {
            return array_[index];
        }

        reference at(size_t const index) {
            checkRange(index);

            return array_[index];
        }

        ConstReference at(size_t const index) const {
            checkRange(index);

            return array_[index];
        }

        /* ***************************************** Iterators and pointers ***************************************** */

        Iterator begin() {
            return array_;
        }

        ConstIterator cbegin() const {
            return array_;
        }

        Iterator end() {
            return array_ + size_;
        }

        ConstIterator cend() const {
            return array_ + size_;
        }

        Iterator front() {
            return array_;
        }

        ConstIterator front() const {
            return array_;
        }

        Iterator back() {
            return array_ + size_ - 1;
        }

        ConstIterator back() const {
            return array_ + size_ - 1;
        }

        Pointer data() {
            return array_;
        }

        ConstPointer data() const {
            return array_;
        }

        /* ********************************************* Data accessors ********************************************* */

        bool empty() const noexcept {
            return size_ == 0;
        }

        size_t size() const noexcept {
            return size_;
        }

        size_t capacity() const noexcept {
            return capacity_;
        }

        /* *********************************************** Modifiers *********************************************** */

        void reserve(size_t newCapacity);

        void resize(size_t size);

        void clear();

        void insert(Iterator position, ConstReference value);

        void insert(Iterator position, RValueReference value);

        void erase(Iterator position);

        void pushBack(ConstReference value);

        void pushBack(RValueReference value);

        void popBack();
    };
} // namespace collection


#endif //INCLUDE_VECTOR_H_
