# Fixed Capacity Vector
The fixed capacity vector container class (`::picolibrary::Fixed_Capacity_Vector`) is
defined in the
[`include/picolibrary/fixed_capacity_vector.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/fixed_capacity_vector.h)/[`source/picolibrary/fixed_capacity_vector.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/fixed_capacity_vector.cc)
header/source file pair.

To replace the contents of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::assign()` member functions.

To access the element at a specific position in a vector without bounds checking, use the
`::picolibrary::Fixed_Capacity_Vector::operator[]()` operator overloads.

To access the element at a specific position in a vector with bounds checking, use the
`::picolibrary::Fixed_Capacity_Vector::at()` member functions.

To access the first element of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::front()` member functions.

To access the last element of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::back()` member functions.

To access the underlying array, use the `::picolibrary::Fixed_Capacity_Vector::data()`
member functions.

To get an iterator to the first element of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::begin()` and
`::picolibrary::Fixed_Capacity_Vector::cbegin()` member functions.

To get an iterator to the element following the last element of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::end()` and
`::picolibrary::Fixed_Capacity_Vector::cend()` member functions.

To get an iterator to the first element of a reversed vector, use the
`::picolibrary::Fixed_Capacity_Vector::rbegin()` and
`::picolibrary::Fixed_Capacity_Vector::crbegin()` member functions.

To get an iterator to the element following the last element of a reversed vector, use the
`::picolibrary::Fixed_Capacity_Vector::rend()` and
`::picolibrary::Fixed_Capacity_Vector::crend()` member functions.

To check if a vector is empty, use the `::picolibrary::Fixed_Capacity_Vector::empty()`
member function.

To check if a vector is full, use the `::picolibrary::Fixed_Capacity_Vector::full()`
member function.

To get the number of elements in a vector, use the
`::picolibrary::Fixed_Capacity_Vector::size()` member function.

To get the maximum number of elements a vector is able to hold, use the
`::picolibrary::Fixed_Capacity_Vector::max_size()` member function.

To get the number of elements a vector has allocated space for, use the
`::picolibrary::Fixed_Capacity_Vector::capacity()` member function; or the
`::picolibrary::fixed_capacity_vector_capacity` or
`::picolibrary::fixed_capacity_vector_capacity_v` meta functions.

To erase all vector elements, use the `::picolibrary::Fixed_Capacity_Vector::clear()`
member function.

To insert values into a vector, use the `::picolibrary::Fixed_Capacity_Vector::insert()`
member functions.

To emplace a value into a vector, use the
`::picolibrary::Fixed_Capacity_Vector::emplace()` member functions.

To remove elements from a vector, use the `::picolibrary::Fixed_Capacity_Vector::erase()`
member functions.

To append a value to a vector, use the `::picolibrary::Fixed_Capacity_Vector::push_back()`
and `::picolibrary::Fixed_Capacity_Vector::emplace_back()` member functions.

To remove the last element of a vector, use the
`::picolibrary::Fixed_Capacity_Vector::pop_back()` member function.

To resize a vector, use the `::picolibrary::Fixed_Capacity_Vector::resize()` member
functions.

Direct comparison for equality or inequality is supported.
