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
 * \brief picolibrary::Circular_Buffer interface.
 */

#ifndef PICOLIBRARY_CIRCULAR_BUFFER_H
#define PICOLIBRARY_CIRCULAR_BUFFER_H

#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"

namespace picolibrary {

/**
 * \brief picolibrary::Circular_Buffer single reader/writer access pattern.
 *
 * This access pattern supports the following use cases (assumes there is a main thread of
 * execution and interrupts, and interrupts cannot interrupt one another):
 * - The main thread of execution both writes to and reads from the circular buffer while
 *   interrupts neither write to nor read from the circular buffer
 * - An interrupt both writes to and reads from the circular buffer while the main thread
 *   of execution and other interrupts neither write to nor read from the circular buffer
 */
struct Single_Reader_Writer {
};

/**
 * \brief picolibrary::Circular_Buffer single reader, single writer access pattern.
 *
 * This access pattern supports the following use cases (assumes there is a main thread of
 * execution and interrupts, and interrupts cannot interrupt one another):
 * - The main thread of execution only writes to the circular buffer while interrupts only
 *   read from the circular buffer
 * - Interrupts only write to the circular buffer while the main thread of execution only
 *   reads from the circular buffer
 * - Interrupts both write to and read from the circular buffer while the main thread of
 *   execution neither writes to nor reads from the circular buffer
 */
struct Single_Reader_Single_Writer {
};

/**
 * \brief picolibrary::Circular_Buffer single reader, multiple writers access pattern.
 *
 * This access pattern supports the following use cases (assumes there is a main thread of
 * execution and interrupts, and interrupts cannot interrupt one another):
 * - The main thread of execution both write to and reads from the circular buffer while
 *   interrupts only write to the circular buffer
 * - Interrupts both write to and read from the circular buffer while the main thread of
 *   execution only writes to the circular buffer
 */
struct Single_Reader_Multiple_Writers {
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 * \tparam Access_Pattern The circular buffer access pattern (must be
 *         picolibrary::Single_Reader_Writer, picolibrary::Single_Reader_Single_Writer, or
 *         picolibrary::Single_Reader_Multiple_Writers).
 * \tparam Interrupt_Controller The type of interrupt controller used to manipulate the
 *         interrupt enable state of any interrupts that interact with the circular buffer
 *         (must be void if Access_Pattern is picolibrary::Single_Reader_Writer).
 */
template<typename T, typename Size_Type, Size_Type N, typename Access_Pattern, typename Interrupt_Controller = void>
class Circular_Buffer;

/**
 * \brief Single reader/writer circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 */
template<typename T, typename Size_Type, Size_Type N>
class Circular_Buffer<T, Size_Type, N, Single_Reader_Writer, void> {
  public:
    static_assert( std::is_unsigned_v<Size_Type> );

    static_assert( is_power_of_two( N ) );

    /**
     * \brief The circular buffer element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the circular buffer.
     */
    using Size = Size_Type;

    /**
     * \brief Constructor.
     */
    Circular_Buffer() noexcept
    {
    }

    Circular_Buffer( Circular_Buffer && ) = delete;

    Circular_Buffer( Circular_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Circular_Buffer() noexcept
    {
        clear();
    }

    auto operator=( Circular_Buffer && ) = delete;

    auto operator=( Circular_Buffer const & ) = delete;

    /**
     * \brief Access the first element of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element of the circular buffer.
     */
    auto front() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the first element of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element of the circular buffer.
     */
    auto front() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the last element of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element of the circular buffer.
     */
    auto back() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>(
            &m_storage[ ( m_read - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Access the last element of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element of the circular buffer.
     */
    auto back() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>(
            &m_storage[ ( m_read - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    auto empty() const noexcept -> bool
    {
        return not size();
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    auto full() const noexcept -> bool
    {
        return size() == max_size();
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \return The number of elements in the circular buffer.
     */
    auto size() const noexcept -> Size
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the circular buffer is able to hold.
     *
     * \return The maximum number of elements the circular buffer is able to hold.
     */
    auto max_size() const noexcept -> Size
    {
        return N;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full()
     *
     * \param[in] value The value to insert.
     */
    void push( Value && value ) noexcept
    {
        expect( not full(), Generic_Error::WOULD_OVERFLOW );

        push( BYPASS_PRECONDITION_EXPECTATION_CHECKS, std::move( value ) );
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to insert.
     */
    void push( Bypass_Precondition_Expectation_Checks, Value && value ) noexcept
    {
        new ( &m_storage[ m_write ] ) Value{ std::move( value ) };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full()
     *
     * \param[in] value The value to insert.
     */
    void push( Value const & value ) noexcept
    {
        expect( not full(), Generic_Error::WOULD_OVERFLOW );

        push( BYPASS_PRECONDITION_EXPECTATION_CHECKS, value );
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to insert.
     */
    void push( Bypass_Precondition_Expectation_Checks, Value const & value ) noexcept
    {
        new ( &m_storage[ m_write ] ) Value{ value };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace( Arguments &&... arguments ) noexcept
    {
        expect( not full(), Generic_Error::WOULD_OVERFLOW );

        emplace( BYPASS_PRECONDITION_EXPECTATION_CHECKS, std::forward<Arguments>( arguments )... );
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace( Bypass_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        new ( &m_storage[ m_write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::empty()
     */
    void pop() noexcept
    {
        expect( not empty(), Generic_Error::WOULD_UNDERFLOW );

        pop( BYPASS_PRECONDITION_EXPECTATION_CHECKS );
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     */
    void pop( Bypass_Precondition_Expectation_Checks ) noexcept
    {
        front().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear() noexcept
    {
        while ( not empty() ) { pop( BYPASS_PRECONDITION_EXPECTATION_CHECKS ); } // while
    }

  private:
    /**
     * \brief Circular buffer element storage.
     */
    using Storage = std::aligned_storage_t<sizeof( Value ), alignof( Value )>;

    /**
     * \brief Circular buffer element storage index.
     */
    using Index = Size;

    /**
     * \brief The circular buffer element read index.
     */
    Index m_read{ 0 };

    /**
     * \brief The circular buffer element write index.
     */
    Index m_write{ 0 };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size m_size{ 0 };

    /**
     * \brief The circular buffer element storage.
     */
    Storage m_storage[ N ];

    /**
     * \brief Get the circular buffer element index wrap around mask.
     *
     * \return The circular buffer element index wrap around mask.
     */
    static constexpr auto index_wrap_around_mask() noexcept -> Index
    {
        return N - 1;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
