#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <iterator>
#include <type_traits>

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

    public:
        /* ********************************************** Constructors ********************************************** */

        Vector();

        Vector(Vector const& original);

        Vector(Vector&& original);

        /* ****************************************** Assignment operators ****************************************** */

        Vector& operator=(Vector const& other);

        Vector& operator=(Vector&& other);

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
