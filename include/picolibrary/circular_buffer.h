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
 * \brief Circular buffer use case.
 *
 * \attention These use cases assume that interrupts that interact with the circular
 *            buffer cannot be interrupted by other interrupts that interact with the same
 *            circular buffer.
 */
enum class Circular_Buffer_Use_Case {
    /**
     * The main thread of execution reads from and writes to the circular buffer.
     * Interrupts do not interact with the circular buffer.
     */
    MAIN_READS_WRITES,

    /**
     * An interrupt reads from and writes to the circular buffer. Other interrupts do not
     * interact with the circular buffer. The main thread of execution does not interact
     * with the circular buffer.
     */
    INTERRUPT_READS_WRITES,

    /**
     * The main thread of execution reads from the circular buffer. One or more interrupts
     * write to the circular buffer.
     */
    MAIN_READS_INTERRUPT_WRITES,

    /**
     * An interrupt reads from the circular buffer. Other interrupts do not interact with
     * the circular buffer. The main thread of execution writes to the circular buffer.
     */
    INTERRUPT_READS_MAIN_WRITES,

    /**
     * An interrupt reads from the circular buffer. One or more other interrupts write to
     * the circular buffer. The main thread of execution does not interact with the
     * circular buffer.
     */
    INTERRUPT_READS_INTERRUPT_WRITES,

    /**
     * The main thread of execution reads from and writes to the circular buffer. One or
     * more interrupts write to the circular buffer.
     */
    MAIN_READS_WRITES_INTERRUPT_WRITES,

    /**
     * An interrupt reads from and writes to the circular buffer. Other interrupts may
     * write to the circular buffer. The main thread of execution writes to the circular
     * buffer.
     */
    INTERRUPT_READS_WRITES_MAIN_WRITES,

    /**
     * An interrupt reads from and writes to the circular buffer. Other interrupts write
     * to the circular buffer. The main thread of execution does not interact with the
     * circular buffer.
     */
    INTERRUPT_READS_WRITES_INTERRUPT_WRITES = INTERRUPT_READS_INTERRUPT_WRITES,
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 * \tparam USE_CASE The circular buffer use case.
 * \tparam Interrupt_Controller The type of interrupt controller used by the main thread
 *         of execution to manipulate the interrupt enable state of any interrupts that
 *         also interact with the circular buffer (must be void if either only the main
 *         thread of execution interacts with the circular buffer or only interrupts
 *         interact with the circular buffer).
 */
template<typename T, typename Size_Type, Size_Type N, Circular_Buffer_Use_Case USE_CASE, typename Interrupt_Controller = void>
class Circular_Buffer;

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 */
template<typename T, typename Size_Type, Size_Type N>
class Circular_Buffer<T, Size_Type, N, Circular_Buffer_Use_Case::MAIN_READS_WRITES, void> {
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
     * \brief Access the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the front of the circular buffer.
     */
    auto front() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the front of the circular buffer.
     */
    auto front() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>(
            &m_storage[ ( m_read + size() - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>(
            &m_storage[ ( m_read + size() - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return size() == 0;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    [[nodiscard]] auto full() const noexcept -> bool
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

        new ( &m_storage[ m_write ] ) Value{ std::move( value ) };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
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

        new ( &m_storage[ m_write ] ) Value{ value };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
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

        new ( &m_storage[ m_write ] ) Value{ std::forward<Arguments>( arguments )... };

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
    void emplace( Run_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        expect( not full(), Generic_Error::WOULD_OVERFLOW );

        new ( &m_storage[ m_write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        ++m_size;
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

        front().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
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
     * \brief Get the circular buffer element storage index wrap around mask.
     *
     * \return The circular buffer element storage index wrap around mask.
     */
    static constexpr auto index_wrap_around_mask() noexcept -> Index
    {
        return N - 1;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
