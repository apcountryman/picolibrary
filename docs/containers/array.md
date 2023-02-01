# Array
The array container class (`::picolibrary::Array`) is defined in the
[`include/picolibrary/array.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/array.h)/[`source/picolibrary/array.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/array.cc)
header/source file pair.

`::picolibrary::Array` supports the following operations:
- To access the element at a specific position in an array without bounds checking, use
  the `::picolibrary::Array::operator[]()` operator overloads.
- To access the element at a specific position in an array with bounds checking, use the
  `::picolibrary::Array::at()` member functions.
- To access the first element of an array, use the `::picolibrary::Array::front()` member
  functions.
- To access the last element of an array, use the `::picolibrary::Array::back()` member
  functions.
- To access the underlying array, use the `::picolibrary::Array::data()` member functions.
- To get an iterator to the first element of an array, use the
  `::picolibrary::Array::begin()` and `::picolibrary::Array::cbegin()` member functions.
- To get an iterator to the element following the last element of an array, use the
  `::picolibrary::Array::end()` and `::picolibrary::Array::cend()` member functions.
- To get an iterator to the first element of a reversed array, use the
  `::picolibrary::Array::rbegin()` and `::picolibrary::Array::crbegin()` member functions.
- To get an iterator to the element following the last element of a reversed array, use
  the `::picolibrary::Array::rend()` and `::picolibrary::Array::crend()` member functions.
- To check if an array is empty, use the `::picolibrary::Array::empty()` member function.
- To get the number of elements in an array, use the `::picolibrary::Array::size()` member
  function; or the `::picolibrary::array_size` or `::picolibrary::array_size_v` meta
  functions.
- Direct comparison for equality or inequality is supported.
