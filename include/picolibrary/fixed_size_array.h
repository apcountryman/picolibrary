/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Fixed_Size_Array interface.
 */

#ifndef PICOLIBRARY_FIXED_SIZE_ARRAY_H
#define PICOLIBRARY_FIXED_SIZE_ARRAY_H

#include <cstddef>

#include "picolibrary/algorithm.h"
#include "picolibrary/iterator.h"

namespace picolibrary {

/**
 * \brief Fixed size array.
 *
 * \tparam T The array element type.
 * \tparam N The number of elements in the array.
 */
template<typename T, std::size_t N>
class Fixed_Size_Array {
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
    using Reverse_Iterator = ::picolibrary::Reverse_Iterator<Iterator>;

    /**
     * \brief An array const reverse iterator.
     */
    using Const_Reverse_Iterator = ::picolibrary::Reverse_Iterator<Const_Iterator>;

    /**
     * \brief The underlying array.
     */
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
        if constexpr ( static_cast<bool>( N ) ) {
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
        if constexpr ( static_cast<bool>( N ) ) {
            return data()[ position ];
        } else {
            return data()[ 0 ];
        } // else
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
        if constexpr ( static_cast<bool>( N ) ) {
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
        if constexpr ( static_cast<bool>( N ) ) {
            return *( end() - 1 );
        } else {
            return *end();
        } // else
    }

    /**
     * \brief Access the underlying array.
     *
     * \return A null pointer if the array is empty.
     * \return The underlying array if the array is not empty.
     */
    constexpr auto data() noexcept -> Pointer
    {
        if constexpr ( static_cast<bool>( N ) ) {
            return &m_array[ 0 ];
        } else {
            return nullptr;
        } // else
    }

    /**
     * \brief Access the underlying array.
     *
     * \return A null pointer if the array is empty.
     * \return The underlying array if the array is not empty.
     */
    constexpr auto data() const noexcept -> Const_Pointer
    {
        if constexpr ( static_cast<bool>( N ) ) {
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
    constexpr auto begin() noexcept
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the array.
     *
     * \return An iterator to the first element of the array.
     */
    constexpr auto begin() const noexcept
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the array.
     *
     * \return An iterator to the first element of the array.
     */
    constexpr auto cbegin() const noexcept
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
    constexpr auto end() noexcept
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
    constexpr auto end() const noexcept
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
    constexpr auto cend() const noexcept
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto rbegin() noexcept
    {
        return Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto rbegin() const noexcept
    {
        return Const_Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed array.
     *
     * \return An iterator to the first element of the reversed array.
     */
    constexpr auto crbegin() const noexcept
    {
        return Const_Reverse_Iterator{ cend() };
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
    constexpr auto rend() noexcept
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
    constexpr auto rend() const noexcept
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
    constexpr auto crend() const noexcept
    {
        return Const_Reverse_Iterator{ cbegin() };
    }

    /**
     * \brief Check if the array is empty.
     *
     * \return true if the array is empty.
     * \return false if the array is not empty.
     */
    [[nodiscard]] constexpr auto empty() const noexcept
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
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Fixed_Size_Array
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
auto operator==( Fixed_Size_Array<T, N> const & lhs, Fixed_Size_Array<T, N> const & rhs ) noexcept
{
    return equal( lhs.begin(), lhs.end(), rhs.begin() );
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Fixed_Size_Array
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
auto operator!=( Fixed_Size_Array<T, N> const & lhs, Fixed_Size_Array<T, N> const & rhs ) noexcept
{
    return not( lhs == rhs );
}

} // namespace picolibrary

#endif // PICOLIBRARY_FIXED_SIZE_ARRAY_H
