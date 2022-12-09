/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
 * contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * \file
 * \brief picolibrary::Array interface.
 */

#ifndef PICOLIBRARY_ARRAY_H
#define PICOLIBRARY_ARRAY_H

#include <cstddef>
#include <iterator>
#include <type_traits>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"

namespace picolibrary {

/**
 * \brief Array.
 *
 * \tparam T The array element type.
 * \tparam N The number of elements in the array.
 */
template<typename T, std::size_t N>
class Array {
  public:
    /**
     * \brief The array element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the array.
     */
    using Size = std::size_t;

    /**
     * \brief An array element position.
     */
    using Position = std::size_t;

    /**
     * \brief A reference to an array element.
     */
    using Reference = Value &;

    /**
     * \brief A reference to a const array element.
     */
    using Const_Reference = Value const &;

    /**
     * \brief A pointer to an array element.
     */
    using Pointer = Value *;

    /**
     * \brief A pointer to a const array element.
     */
    using Const_Pointer = Value const *;

    /**
     * \brief An array iterator.
     */
    using Iterator = Pointer;

    /**
     * \brief A const array iterator.
     */
    using Const_Iterator = Const_Pointer;

    /**
     * \brief An array reverse iterator.
     */
    using Reverse_Iterator = std::reverse_iterator<Iterator>;

    /**
     * \brief A const array reverse iterator.
     */
    using Const_Reverse_Iterator = std::reverse_iterator<Const_Iterator>;

    /**
     * \brief The underlying array.
     */
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    Value m_array[ N ];

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \warning Bounds checking is not performed.
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \param[in] position The position of the array element to access.
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto operator[]( Position position ) noexcept -> Reference
    {
        if constexpr ( N > 0 ) {
            return data()[ position ];
        } else {
            return data()[ 0 ];
        } // else
    }

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \warning Bounds checking is not performed.
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \param[in] position The position of the array element to access.
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto operator[]( Position position ) const noexcept -> Const_Reference
    {
        if constexpr ( N > 0 ) {
            return data()[ position ];
        } else {
            return data()[ 0 ];
        } // else
    }

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \param[in] position The position of the array element to access.
     *
     * \pre position < picolibrary::Array::size()
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto at( Position position ) noexcept -> Reference
    {
        PICOLIBRARY_EXPECT( position < size(), Generic_Error::OUT_OF_RANGE );

        return data()[ position ];
    }

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \param[in] position The position of the array element to access.
     *
     * \pre position < picolibrary::Array::size()
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto at( Position position ) const noexcept -> Const_Reference
    {
        PICOLIBRARY_EXPECT( position < size(), Generic_Error::OUT_OF_RANGE );

        return data()[ position ];
    }

    /**
     * \brief Access the first element of the array.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The first element of the array.
     */
    constexpr auto front() noexcept -> Reference
    {
        return *begin();
    }

    /**
     * \brief Access the first element of the array.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The first element of the array.
     */
    constexpr auto front() const noexcept -> Const_Reference
    {
        return *begin();
    }

    /**
     * \brief Access the last element of the array.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The last element of the array.
     */
    constexpr auto back() noexcept -> Reference
    {
        if constexpr ( N > 0 ) {
            return *( end() - 1 );
        } else {
            return *end();
        } // else
    }

    /**
     * \brief Access the last element of the array.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The last element of the array.
     */
    constexpr auto back() const noexcept -> Const_Reference
    {
        if constexpr ( N > 0 ) {
            return *( end() - 1 );
        } else {
            return *end();
        } // else
    }

    /**
     * \brief Access the underlying array.
     *
     * \return A null pointer if the array us empty.
     * \return The underlying array if the array is not empty.
     */
    constexpr auto data() noexcept -> Pointer
    {
        if constexpr ( N > 0 ) {
            return &m_array[ 0 ];
        } else {
            return nullptr;
        } // else
    }

    /**
     * \brief Access the underlying array.
     *
     * \return A null pointer if the array us empty.
     * \return The underlying array if the array is not empty.
     */
    constexpr auto data() const noexcept -> Const_Pointer
    {
        if constexpr ( N > 0 ) {
            return &m_array[ 0 ];
        } else {
            return nullptr;
        } // else
    }

    /**
     * \brief Get an iterator to the first element of the array.
     *
     * \return An iterator to the first element of the array.
     */
    constexpr auto begin() noexcept -> Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the array.
     *
     * \return An iterator to the first element of the array.
     */
    constexpr auto begin() const noexcept -> Const_Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the array.
     *
     * \return An iterator to the first element of the array.
     */
    constexpr auto cbegin() const noexcept -> Const_Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the element following the last element of the array.
     *
     * \warning Attempting to access the element following the last element of an array
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the array.
     */
    constexpr auto end() noexcept -> Iterator
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the element following the last element of the array.
     *
     * \warning Attempting to access the element following the last element of an array
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the array.
     */
    constexpr auto end() const noexcept -> Const_Iterator
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the element following the last element of the array.
     *
     * \warning Attempting to access the element following the last element of an array
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the array.
     */
    constexpr auto cend() const noexcept -> Const_Iterator
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto rbegin() noexcept -> Reverse_Iterator
    {
        return Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto rbegin() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto crbegin() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        array.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          array results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         array.
     */
    constexpr auto rend() noexcept -> Reverse_Iterator
    {
        return Reverse_Iterator{ begin() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        array.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          array results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         array.
     */
    constexpr auto rend() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ begin() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        array.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          array results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         array.
     */
    constexpr auto crend() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ begin() };
    }

    /**
     * \brief Check if the array is empty.
     *
     * \return true if the array is empty.
     * \return false if the array is not empty.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return not size();
    }

    /**
     * \brief Get the number of elements in the array.
     *
     * \return The number of elements in the array.
     */
    constexpr auto size() const noexcept -> Size
    {
        return N;
    }
};

/**
 * \brief Get the size of an array.
 *
 * \relatedalso picolibrary::Array
 *
 * \tparam T The array type whose size is to be gotten.
 */
template<typename T>
struct array_size {
};

/**
 * \brief Get the size of an array.
 *
 * \relatedalso picolibrary::Array
 *
 * \tparam T The array type whose size is to be gotten.
 */
template<typename T>
inline constexpr auto array_size_v = array_size<T>::value;

/**
 * \brief Get the size of an array.
 *
 * \relatedalso picolibrary::Array
 *
 * \tparam T The array type whose size is to be gotten.
 */
template<typename T, std::size_t N>
struct array_size<Array<T, N>> : std::integral_constant<std::size_t, N> {
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Array
 *
 * \tparam T The array element type.
 * \tparam N The number of elements in the array.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
template<typename T, std::size_t N>
constexpr auto operator==( Array<T, N> const & lhs, Array<T, N> const & rhs ) noexcept -> bool
{
    return ::picolibrary::equal( lhs.begin(), lhs.end(), rhs.begin() );
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Array
 *
 * \tparam T The array element type.
 * \tparam N The number of elements in the array.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
template<typename T, std::size_t N>
constexpr auto operator!=( Array<T, N> const & lhs, Array<T, N> const & rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

} // namespace picolibrary

#endif // PICOLIBRARY_ARRAY_H
