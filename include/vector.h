#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace collection {

    /**
     * @brief Vector of elements
     * @tparam Value type of stored value
     * @tparam Allocator type of used allocator
     */
    template<typename Value, typename Allocator = std::allocator<Value>>
    class Vector {
        typedef std::iterator_traits<Value*> iteratorTraits;

        typedef Value ValueType;
        typedef Value& reference;
        typedef Value const& ConstReference;
        typedef Value&& RValueReference;
        typedef Value* Pointer;
        typedef const Pointer ConstPointer;
        typedef Pointer Iterator;
        typedef ConstPointer ConstIterator;
        typedef Allocator AllocatorType;

    protected:
        static constexpr size_t DEFAULT_CAPACITY = 16;

        AllocatorType allocator_;
        Pointer array_;
        size_t size_, capacity_;

    protected:
        /* ************************************************* Checks ************************************************* */

        inline void copyArrayNoChecks(ConstPointer originalArray, size_t const size) {
            for (size_t i = 0; i < size; ++i) allocator_.construct(array_[i] = originalArray[i]);
        }

        inline void throwOutOfRange(size_t const index) {
            throw std::out_of_range("Index " + std::to_string(index) + " should be < size " + std::to_string(size_));
        }

        inline void throwOutOfRangeEmpty() { throw std::out_of_range("Vector is empty"); }

        inline void checkRange(size_t const index) {
            if (index >= size_) throwOutOfRange(index);
        }

        inline void checkNotEmpty() {
            if (size_ == 0) throwOutOfRangeEmpty();
        }

        /* ************************************************ Resizers ************************************************ */

        void resizeToBigger(size_t const newCapacity) {
            auto* const newArray = allocator_.allocate(newCapacity);
            std::move(array_, array_ + capacity_, newArray);
            allocator_.deallocate(array_, capacity_);
            array_ = newArray;
            capacity_ = newCapacity;
        }

        void resizeToSmaller(size_t const newCapacity) {
            auto* const newArray = allocator_.allocate(newCapacity);
            {
                Pointer cutBegin = array_ + newCapacity;
                std::copy(array_, cutBegin, newArray);
                ConstPointer const cutEnd = array_ + capacity_;
                for (; cutBegin != cutEnd; ++cutBegin) allocator_.destroy(cutBegin);
            }
            allocator_.deallocate(array_, capacity_);
            array_ = newArray;
            capacity_ = newCapacity;
        }

        static constexpr size_t increasedCapacity(size_t const capacity) {
            return capacity < 2 ? 2 : capacity + (capacity >> 1u);
        }

        void requireExtraSlot() {
            auto const currentCapacity = capacity_;
            if (size_ < currentCapacity) resizeToBigger(increasedCapacity(currentCapacity));
        }

        /* ****************************************** Internal constructor ****************************************** */

        // this does not perform object construction
        Vector(size_t capacity, size_t const size)
            : array_(allocator_.allocate(capacity)), size_(size), capacity_(capacity) {}

        // this does not perform object construction
        Vector(size_t const size) : Vector(size, size) {}

    public:
        /* *********************************************** Destructor *********************************************** */

        ~Vector() {
            for (size_t i = 0; i < size_; ++i) allocator_.destroy(array_ + i);
            allocator_.deallocate(array_, capacity_);
        }

        /* ********************************************** Constructors ********************************************** */

        Vector() : Vector(DEFAULT_CAPACITY, 0) {}

        Vector(Vector const& original) : Vector(original.size_) { copyArrayNoChecks(original.array_); }

        Vector(Vector&& original) noexcept
            : array_(std::exchange(original.array_, nullptr)), size_(std::exchange(original.size_, 0)),
              capacity_(std::exchange(original.capacity_, 0)) {}

        /* ****************************************** Assignment operators ****************************************** */

        Vector& operator=(Vector const& other) {
            if (this != *other) {
                if (size_ < other.size_) {
                    array_ = allocator_.allocate(other.size_);
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

        void swap(Vector& other) noexcept {
            std::swap(array_, other.array_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

        /* ********************************************* Indexed access ********************************************* */

        reference operator[](size_t index) { return array_[index]; }

        ConstReference operator[](size_t index) const { return array_[index]; }

        reference at(size_t const index) {
            checkRange(index);

            return array_[index];
        }

        ConstReference at(size_t const index) const {
            checkRange(index);

            return array_[index];
        }

        /* ***************************************** Iterators and pointers ***************************************** */

        constexpr Iterator begin() { return array_; }

        constexpr ConstIterator cbegin() const { return array_; }

        constexpr Iterator end() { return array_ + size_; }

        constexpr ConstIterator cend() const { return array_ + size_; }

        constexpr Iterator front() { return array_; }

        constexpr ConstIterator front() const { return array_; }

        constexpr Iterator back() { return size_ == 0 ? end() : array_ + size_ - 1; }

        constexpr ConstIterator back() const { return size_ == 0 ? cend() : array_ + size_ - 1; }

        constexpr Pointer data() { return array_; }

        constexpr ConstPointer data() const { return array_; }

        /* ********************************************* Data accessors ********************************************* */

        bool empty() const noexcept { return size_ == 0; }

        size_t size() const noexcept { return size_; }

        size_t capacity() const noexcept { return capacity_; }

        /* *********************************************** Modifiers *********************************************** */

        void reserve(size_t const FIXME_NEWCapacity) {
            if (capacity_ < FIXME_NEWCapacity) resizeToBigger(FIXME_NEWCapacity);
        }

        void resize(size_t const FIXME_NEWSize, ValueType value /* = {}*/) {
            auto const currentSize = size_;
            if (FIXME_NEWSize < currentSize) resizeToSmaller(FIXME_NEWSize);
            else if (FIXME_NEWSize > currentSize) {
                resizeToBigger(FIXME_NEWSize);
                std::fill(array_ + currentSize, array_ + FIXME_NEWSize, value);
            }
        }

        void resize(size_t const FIXME_NEWSize) {
            auto const currentSize = size_;
            if (FIXME_NEWSize < currentSize) resizeToSmaller(FIXME_NEWSize);
            else if (FIXME_NEWSize > currentSize)
                resizeToBigger(FIXME_NEWSize);
        }

        void resize(size_t const FIXME_NEWSize, ConstReference value) {
            auto const currentSize = size_;
            if (FIXME_NEWSize < currentSize) resizeToSmaller(FIXME_NEWSize);
            else if (FIXME_NEWSize > currentSize) {
                resizeToBigger(FIXME_NEWSize);
                std::fill(array_ + currentSize, array_ + FIXME_NEWSize, value);
            }
        }

        // differs from standard implementation as it
        void clear() {
            for (size_t i = 0; i < size_; ++size_) allocator_.destroy(array_ + i);
            size_ = 0;
        }

        void insert(Iterator position, ConstReference value) {
            requireExtraSlot();
            // move all elements to the right
            for (auto iterator = array_ + ++size_; iterator != position;) *iterator = *(iterator - 1);
            *position = value;
        }

        void insert(Iterator position, RValueReference value) {
            auto const offset = position - array_;
            requireExtraSlot();
            position = array_ + offset;
            // move all elements to the right
            for (auto iterator = array_ + ++size_; iterator != position; --iterator) *iterator = *(iterator - 1);
            *position = std::move(value);
        }

        void erase(Iterator from, Iterator to) { // spec does not require any explicit exceptions
            auto const delta = to - from;
            auto const arrayEnd = end();
            // move all elements after (to) to the right
            for (auto target = from, source = to; source != arrayEnd;) *(target++) = *(source++);
            // destroy tail
            for (auto iterator = arrayEnd - delta; iterator != arrayEnd;) allocator_.destroy(iterator++);

            size_ -= delta;
        }

        void erase(Iterator position) { erase(position, position + 1); }

        void pushBack(ConstReference value) {
            requireExtraSlot();

            array_[size_++] = value;
        }

        void pushBack(RValueReference value) {
            requireExtraSlot();

            array_[size_++] = std::move(value);
        }

        void popBack() {
            checkNotEmpty();

            resizeToSmaller(--size_);
        }
    };
} // namespace collection


#endif //INCLUDE_VECTOR_H_
