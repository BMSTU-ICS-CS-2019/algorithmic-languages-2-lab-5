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
        typedef std::allocator_traits<AllocatorType> Memory;

    protected:
        static constexpr size_t DEFAULT_CAPACITY = 16;

        AllocatorType allocator_;
        Pointer array_;
        size_t size_, capacity_;

    protected:
        /* ************************************************* Checks ************************************************* */

        inline void copyArrayNoChecks(ConstPointer originalArray, size_t const size) {
            for (size_t i = 0; i < size; ++i) Memory::construct(allocator_, array_ + i, *(originalArray + i));
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
            // allocate new memory segment
            Pointer const newArray = Memory::allocate(allocator_, newCapacity);
            // move all currently constructed elements to the new memory location

            { // move values
                ConstPointer const sourceEnd = array_ + size_;
                for (Pointer source = array_, target = newArray; source != sourceEnd; ++source) {
                    Memory::construct(allocator_, target++, std::move(*source));
                    Memory::destroy(allocator_, source);
                }
                Memory::deallocate(allocator_, array_, capacity_);
            }

            array_ = newArray;
            capacity_ = newCapacity;
        }

        void resizeToSmaller(size_t const newCapacity) {
            Pointer const newArray = Memory::allocate(allocator_, newCapacity);
            {
                // move kept values
                Pointer source;
                {
                    Pointer target;
                    ConstPointer const moveSourceEnd = array_ + size_;
                    for (source = array_, target = newArray; source != moveSourceEnd; ++source) {
                        Memory::construct(allocator_, target++, std::move(source));
                        Memory::destroy(allocator_, source);
                    }
                }
                ConstPointer const sourceEnd = array_ + capacity_;
                for (; source != sourceEnd; ++source) Memory::destroy(allocator_, source);
            }
            Memory::deallocate(allocator_, array_, capacity_);

            array_ = newArray;
            capacity_ = newCapacity;
        }

        static constexpr size_t increasedCapacity(size_t const capacity) {
            return capacity < 2 ? 2 : capacity + (capacity >> 1u);
        }

        void requireExtraSlot() {
            auto const currentCapacity = capacity_;
            if (size_ == 0 || size_ == currentCapacity - 1) resizeToBigger(increasedCapacity(currentCapacity));
        }

        /* ****************************************** Internal constructor ****************************************** */

        // this does not perform object construction

        /**
         * @brief Creates a vector of equal size and capacity without constructing its entries
         * @param capacity capacity of the create vector, should not be less than {@code
         * @param size size of the created vector
         */
        Vector(size_t capacity, size_t const size)
            : array_(Memory::allocate(allocator_, capacity)), size_(size), capacity_(capacity) {}

        /**
         * @brief Creates a vector of equal size and capacity without constructing its entries
         * @param size size of the created vector and its capacity
         */
        Vector(size_t const size) : Vector(size, size) {}

        /* ************************************* Internal complicated modifiers ************************************* */


        void uncheckedInsert(Iterator position, ConstReference value) {
            // this should happen before reallocation as after the pointers change
            auto const index = position - array_;
            requireExtraSlot();

            auto const oldSize = size_++;
            if (index == oldSize) Memory::construct(allocator_, array_ + size_, value);
            else {
                // the element should be constructed anyway, but using previous value
                auto target = array_ + oldSize;
                Memory::construct(allocator_, target, std::move(*(target - 1)));
                // the elements after index should be moved to the right
                // target is the element to move the value into
                auto const bound = array_ + index;
                for (; target != bound;) {
                    auto const current = target--; // actual target to move the value into
                    *current = std::move(*target);
                }
                // here target is the bound and contains the value to be destroyed
                Memory::destroy(allocator_, target);
                Memory::construct(allocator_, target, value);
            }
        }

        void uncheckedInsert(ConstIterator const position, RValueReference value) {
            // this should happen before reallocation as after the pointers change
            auto const index = position - array_;
            requireExtraSlot();

            auto const oldSize = size_++;
            if (index == oldSize) Memory::construct(allocator_, array_ + oldSize, std::forward<RValueReference>(value));
            else {
                // the element should be constructed anyway, but using previous value
                auto target = array_ + oldSize;
                Memory::construct(allocator_, target, std::move(*(target - 1)));
                // the elements after index should be moved to the right
                // target is the element to move the value into
                auto const bound = array_ + index;
                for (; target != bound;) {
                    auto const current = target--; // actual target to move the value into
                    *current = std::move(*target);
                }
                // here target is the bound and contains the value to be destroyed,
                // so we perform the last exchange - with the caller
                *target = std::forward<RValueReference>(value);
            }
        }

        void uncheckedErase(ConstIterator const from, ConstIterator const to) noexcept {
            auto const end = cend();
            // move all elements after (to) to the left
            // target is the element on the left to which the element gets moved
            // source is the element on the right from which the element gets moved
            for (auto source = to, target = from; source != end; ++source) *target = std::move(*source);
            auto const delta = to - from;
            for (auto iterator = end - delta; iterator != end; ++iterator) Memory::destroy(allocator_, iterator);

            size_ -= delta;
        }

    public:
        /* *********************************************** Destructor *********************************************** */

        /**
         * @brief Destroys this vector freeing all allocated memory
         *
         * @note while marking this as {@code virtual} may bring possible overhead, this allows safe extension
         * of this class, while most (if not all) modern compilers are able to perform monomorphic inline caching
         */
        virtual ~Vector() noexcept {
            for (size_t i = 0; i < size_; ++i) Memory::destroy(allocator_, array_ + i);
            Memory::deallocate(allocator_, array_, capacity_);
        }

        /* ********************************************** Constructors ********************************************** */

        /**
         * @brief Creates a new empty vector of default capacity.
         */
        Vector() : Vector(DEFAULT_CAPACITY, 0) {}

        /**
         * @brief Copy-constructs a vector from the specified one.
         * @param original vector whose contents should be {@bold copied} into this one
         */
        Vector(Vector const& original) : Vector(original.size_) { copyArrayNoChecks(original.array_); }

        /**
         * @brief Move-constructs a vector from the specified one.
         * @param original vector whose contents should be {@bold moved} into this one
         */
        Vector(Vector&& original) noexcept
            : array_(std::exchange(original.array_, nullptr)), size_(std::exchange(original.size_, 0)),
              capacity_(std::exchange(original.capacity_, 0)) {}

        /* ****************************************** Assignment operators ****************************************** */

        Vector& operator=(Vector const& other) {
            if (this != *other) {
                if (size_ < other.size_) {
                    array_ = Memory::allocate(allocator_, other.size_);
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

        reference operator[](size_t const index) { return array_[index]; }

        ConstReference operator[](size_t const index) const { return array_[index]; }

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

        void reserve(size_t const newCapacity) {
            if (capacity_ < newCapacity) resizeToBigger(newCapacity);
        }

        void resize(size_t const newSize) {
            static_assert(std::is_default_constructible<ValueType>::value, "Type should be default-constructable");

            auto const currentSize = size_;
            if (newSize < currentSize) {
                for (size_t i = newSize; i < currentSize; ++i) Memory::destroy(allocator_, array_ + i);
                size_ = newSize; // simply decrease size
            } else if (newSize > currentSize) {
                if (newSize > capacity_) resizeToBigger(newSize);
                for (size_t i = currentSize; i < newSize; ++i) Memory::construct(allocator_, array_ + i, ValueType{});
                // allocate new, bigger array
                size_ = newSize;
            }
        }

        void resize(size_t const newSize, ConstReference value) {
            auto const currentSize = size_;
            if (newSize < currentSize) {
                for (size_t i = newSize; i < currentSize; ++i) Memory::destroy(allocator_, array_ + i);
                size_ = newSize; // simply decrease size
            } else if (newSize > currentSize) {
                if (newSize > capacity_) resizeToBigger(newSize);
                for (size_t i = currentSize; i < newSize; ++i) Memory::construct(allocator_, array_ + i, value);
                // allocate new, bigger array
                size_ = newSize;
            }
        }

        // differs from standard implementation as it
        void clear() {
            for (size_t i = 0; i < size_; ++i) Memory::destroy(allocator_, array_ + i);
            size_ = 0;
        }

        void insert(Iterator position, ConstReference value) {
            if (position < array_) throw std::range_error("`position` is out of lower range");
            if (position > array_ + 1) throw std::range_error("`position` is out of higher range");

            uncheckedInsert(position, value);
        }

        void insert(ConstIterator const position, RValueReference value) {
            if (position < array_) throw std::range_error("`position` is out of lower bound");
            if (position > cend()) throw std::range_error("`position` is out of higher bound");

            uncheckedInsert(position, std::forward<RValueReference>(value));
        }

        void erase(ConstIterator const from, ConstIterator const to) {
            if (from > to) throw std::logic_error("`from` cannot be after `to`");
            if (from < array_) throw std::range_error("`from` is out of lower bound");
            if (to < array_) throw std::range_error("`to` is out of lower bound");
            auto const end = cend();
            if (from > end) throw std::range_error("`from` is out of higher bound");
            if (to > end) throw std::range_error("`to` is out of higher bound");

            uncheckedErase(from, to);
        }

        void erase(Iterator const position) { erase(position, position + 1); }

        void pushBack(ConstReference value) {
            requireExtraSlot();

            auto* const address = array_ + size_++;
            Memory::construct(allocator_, address, value);
        }

        void pushBack(RValueReference value) {
            requireExtraSlot();

            auto* const address = array_ + size_++;
            Memory::construct(allocator_, address, std::forward<RValueReference>(value));
        }

        void popBack() {
            checkNotEmpty();

            Memory::destroy(allocator_, array_ + (--size_));
        }
    };
} // namespace collection


#endif //INCLUDE_VECTOR_H_
