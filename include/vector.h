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
        void copyArrayNoChecks(ConstPointer originalArray, size_t const size) {
            std::copy(originalArray, originalArray + size, array_);
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

        Vector& operator=(Vector const& other);

        Vector& operator=(Vector&& other) noexcept;

        /* ********************************************* Indexed access ********************************************* */

        reference operator[](size_t index);

        ConstReference operator[](size_t index) const;

        reference at(size_t index);

        ConstReference const& at(size_t index) const;

        /* ***************************************** Iterators and pointers ***************************************** */

        Iterator begin();

        ConstIterator begin() const;

        Iterator end();

        ConstIterator cend() const;

        Iterator front();

        ConstIterator front() const;

        Iterator back();

        ConstIterator back() const;

        Pointer data();

        ConstPointer data() const;

        /* ********************************************* Data accessors ********************************************* */

        bool empty() const noexcept;

        size_t size() const noexcept;

        size_t capacity() const noexcept;

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
