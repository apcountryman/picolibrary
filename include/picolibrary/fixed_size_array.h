/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
     * \brief The underlying array.
     */
    Value m_array[ N ];

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \param[in] position The position of the array element to access.
     *
     * \warning Bounds checking is not performed.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto operator[]( Position position ) noexcept -> Reference
    {
        return m_array[ position ];
    }

    /**
     * \brief Access the element at the specified position in the array.
     *
     * \param[in] position The position of the array element to access.
     *
     * \warning Bounds checking is not performed.
     *
     * \warning Calling this function on an empty array results in undefined behavior.
     *
     * \return The element at the specified position in the array.
     */
    constexpr auto operator[]( Position position ) const noexcept -> Const_Reference
    {
        return m_array[ position ];
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
        return m_array[ 0 ];
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
        return m_array[ 0 ];
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
        return m_array[ N - 1 ];
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
        return m_array[ N - 1 ];
    }

    /**
     * \brief Access the underlying array.
     *
     * \return A null pointer if the array is empty.
     * \return The underlying array if the array is not empty.
     */
    constexpr auto data() noexcept -> Pointer
    {
        if constexpr ( N ) {
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
        if constexpr ( N ) {
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
     * \brief Check if the array is empty.
     *
     * \return true if the array is empty.
     * \return false if the array is not empty.
     */
    constexpr auto empty() const noexcept
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

} // namespace picolibrary

#endif // PICOLIBRARY_FIXED_SIZE_ARRAY_H
