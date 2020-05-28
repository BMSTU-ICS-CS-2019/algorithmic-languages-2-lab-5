#include <iostream>

#include <ostream>
#include <vector.h>

#define _VECTOR_SHOWCASE(variable, operation)                                                                          \
    std::cout << "Performing: " #variable "." #operation << "\n\t";                                                    \
    variable.operation;                                                                                                \
    std::cout << "Vector[empty=" << variable.empty() << ", size=" << variable.size()                                   \
              << ", capacity=" << variable.capacity() << "]{\t";                                                       \
    for (auto const element : vector) std::cout << element << '\t';                                                    \
    std::cout << '}' << std::endl;

class CustomStruct {
    std::string text;
    int number;

public:
    CustomStruct(std::string text, int number) : text(text), number(number) {}

    CustomStruct() : CustomStruct("default text", 0xCAFEBABE) {}

    friend std::ostream& operator<<(std::ostream& output, CustomStruct const& customStruct);
};

std::ostream& operator<<(std::ostream& output, CustomStruct const& customStruct) {
    std::cout << "CustomStruct{name=\"" << customStruct.text << "\", number=" << customStruct.number << "}";

    return output;
}

int main() {
    std::cout << "========== Vector<int> ==========" << std::endl;
    {
        collection::Vector<int> vector;
        _VECTOR_SHOWCASE(vector, pushBack(1))
        _VECTOR_SHOWCASE(vector, pushBack(5))
        _VECTOR_SHOWCASE(vector, pushBack(127))
        _VECTOR_SHOWCASE(vector, popBack())
        _VECTOR_SHOWCASE(vector, popBack())
        _VECTOR_SHOWCASE(vector, pushBack(2))
        _VECTOR_SHOWCASE(vector, pushBack(4))
        _VECTOR_SHOWCASE(vector, pushBack(8))
        _VECTOR_SHOWCASE(vector, insert(vector.begin() + 2, 99))
        _VECTOR_SHOWCASE(vector, erase(vector.begin() + 1))
        _VECTOR_SHOWCASE(vector, erase(vector.begin() + 1, vector.begin() + 3))
        _VECTOR_SHOWCASE(vector, clear())
        _VECTOR_SHOWCASE(vector, pushBack(127))
        _VECTOR_SHOWCASE(vector, pushBack(6))
        _VECTOR_SHOWCASE(vector, pushBack(8))
        _VECTOR_SHOWCASE(vector, pushBack(1))
        _VECTOR_SHOWCASE(vector, resize(8))
        _VECTOR_SHOWCASE(vector, reserve(16))
        _VECTOR_SHOWCASE(vector, resize(2))
        _VECTOR_SHOWCASE(vector, pushBack(1))
        _VECTOR_SHOWCASE(vector, pushBack(2))
        _VECTOR_SHOWCASE(vector, pushBack(3))
        _VECTOR_SHOWCASE(vector, pushBack(4))
        _VECTOR_SHOWCASE(vector, reserve(3))
    }

    std::cout << "========== Vector<CustomStruct> ==========" << std::endl;
    {
        collection::Vector<CustomStruct> vector;
        _VECTOR_SHOWCASE(vector, pushBack({"one", 1}))
        _VECTOR_SHOWCASE(vector, pushBack({"five", 5}))
        _VECTOR_SHOWCASE(vector, pushBack({"one hundred and twenty seven", 127}))
        _VECTOR_SHOWCASE(vector, popBack())
        _VECTOR_SHOWCASE(vector, popBack())
        _VECTOR_SHOWCASE(vector, pushBack({"two", 2}))
        _VECTOR_SHOWCASE(vector, pushBack({"four", 4}))
        _VECTOR_SHOWCASE(vector, pushBack({"eight", 8}))
        _VECTOR_SHOWCASE(vector, insert(vector.begin() + 2, {"ninety nine", 99}))
        _VECTOR_SHOWCASE(vector, erase(vector.begin() + 1))
        _VECTOR_SHOWCASE(vector, erase(vector.begin() + 1, vector.begin() + 3))
        _VECTOR_SHOWCASE(vector, clear())
        _VECTOR_SHOWCASE(vector, pushBack({"one hundred and twenty seven", 127}))
        _VECTOR_SHOWCASE(vector, pushBack({"six", 6}))
        _VECTOR_SHOWCASE(vector, pushBack({"eight", 8}))
        _VECTOR_SHOWCASE(vector, pushBack({"one", 1}))
        _VECTOR_SHOWCASE(vector, resize(8))
        _VECTOR_SHOWCASE(vector, reserve(16))
        _VECTOR_SHOWCASE(vector, resize(2))
        _VECTOR_SHOWCASE(vector, pushBack({"one", 1}))
        _VECTOR_SHOWCASE(vector, pushBack({"two", 2}))
        _VECTOR_SHOWCASE(vector, pushBack({"three", 3}))
        _VECTOR_SHOWCASE(vector, pushBack({"four", 4}))
        _VECTOR_SHOWCASE(vector, reserve(3))
    }
}
