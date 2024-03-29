/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include "picolibrary/interrupt.h"
#include "picolibrary/precondition.h"

namespace picolibrary {

/**
 * \brief Circular buffer use case.
 *
 * \attention These use cases assume that interrupts that interact with the circular
 *            buffer cannot be interrupted by other interrupts that also interact with the
 *            circular buffer.
 */
enum class Circular_Buffer_Use_Case {
    /**
     * The main thread of execution reads from and writes to the circular buffer.
     * Interrupts do not interact with the circular buffer.
     */
    MAIN_READS_MAIN_WRITES,

    /**
     * The main thread of execution reads from the circular buffer. One or more interrupts
     * write to the circular buffer.
     */
    MAIN_READS_INTERRUPTS_WRITE,

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
    INTERRUPT_READS_INTERRUPTS_WRITE,
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type (must be trivially destructible if USE_CASE
 *         is either picolibrary::Circular_Buffer_Use_Case::INTERRUPT_READS_MAIN_WRITES or
 *         picolibrary::Circular_Buffer::INTERRUPT_READS_INTERRUPTS_WRITE).
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 * \tparam USE_CASE The circular buffer use case.
 * \tparam Interrupt_Controller The type of interrupt controller used by the main thread
 *         of execution to manipulate the interrupt enable state of any interrupts that
 *         also interact with the circular buffer (must be void if USE_CASE is either
 *         picolibrary::Circular_Buffer_Use_Case::MAIN_READS_MAIN_WRITES or
 *         picolibrary::Circular_Buffer_Use_Case::INTERRUPT_READS_INTERRUPTS_WRITE).
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
class Circular_Buffer<T, Size_Type, N, Circular_Buffer_Use_Case::MAIN_READS_MAIN_WRITES, void> {
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
    // NOLINTNEXTLINE(modernize-use-equals-default)
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
     * \brief The number of elements in the circular buffer.
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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

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
        PICOLIBRARY_EXPECT( not empty(), Generic_Error::WOULD_UNDERFLOW );

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
     * \brief The circular buffer element storage read index.
     */
    Index m_read{ 0 };

    /**
     * \brief The circular buffer element storage write index.
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

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 * \tparam Interrupt_Controller The type of interrupt controller used by the main thread
 *         of execution to manipulate the interrupt enable state of interrupts that
 *         interact with the circular buffer.
 */
template<typename T, typename Size_Type, Size_Type N, typename Interrupt_Controller>
class Circular_Buffer<T, Size_Type, N, Circular_Buffer_Use_Case::MAIN_READS_INTERRUPTS_WRITE, Interrupt_Controller> {
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

    Circular_Buffer() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] interrupt_controller The interrupt controller used by the main thread of
     *            execution to manipulate the interrupt enable state of interrupts that
     *            interact with the circular buffer.
     */
    Circular_Buffer( Interrupt_Controller interrupt_controller ) noexcept :
        m_interrupt_controller{ std::move( interrupt_controller ) }
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
    auto front() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the front of the circular buffer.
     */
    auto front() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>(
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
    auto back() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>(
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
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    [[nodiscard]] auto empty_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == 0;
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
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    [[nodiscard]] auto full_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == max_size_from_interrupt();
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \brief The number of elements in the circular buffer.
     */
    auto size() const noexcept -> Size
    {
        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

        return m_size;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \brief The number of elements in the circular buffer.
     */
    auto size_from_interrupt() const noexcept -> Size
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
     * \brief Get the maximum number of elements the circular buffer is able to hold.
     *
     * \return The maximum number of elements the circular buffer is able to hold.
     */
    auto max_size_from_interrupt() const noexcept -> Size
    {
        return N;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \param[in] value The value to insert.
     */
    void push_from_interrupt( Value && value ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::move( value ) };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void push_from_interrupt( Bypass_Precondition_Expectation_Checks, Value && value ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::move( value ) };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \param[in] value The value to insert.
     */
    void push_from_interrupt( Value const & value ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ value };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void push_from_interrupt( Bypass_Precondition_Expectation_Checks, Value const & value ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ value };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace_from_interrupt( Arguments &&... arguments ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace_from_interrupt( Run_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void emplace_from_interrupt( Bypass_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::empty()
     */
    void pop() noexcept
    {
        PICOLIBRARY_EXPECT( not empty(), Generic_Error::WOULD_UNDERFLOW );

        front().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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
     * \brief The circular buffer element storage read index.
     */
    Index m_read{ 0 };

    /**
     * \brief The circular buffer element storage write index.
     */
    Index volatile m_write{ 0 };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size volatile m_size{ 0 };

    /**
     * \brief The circular buffer element storage.
     */
    Storage volatile m_storage[ N ];

    /**
     * \brief The interrupt controller used by the main thread of execution to manipulate
     *        the interrupt enable state of interrupts that interact with the circular
     *        buffer.
     */
    Interrupt_Controller m_interrupt_controller{};

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

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type (must be trivially destructible).
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 * \tparam Interrupt_Controller The type of interrupt controller used by the main thread
 *         of execution to manipulate the interrupt enable state of interrupts that
 *         interact with the circular buffer.
 */
template<typename T, typename Size_Type, Size_Type N, typename Interrupt_Controller>
class Circular_Buffer<T, Size_Type, N, Circular_Buffer_Use_Case::INTERRUPT_READS_MAIN_WRITES, Interrupt_Controller> {
  public:
    static_assert( std::is_trivially_destructible_v<T> );

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

    Circular_Buffer() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] interrupt_controller The interrupt controller used by the main thread of
     *            execution to manipulate the interrupt enable state of interrupts that
     *            interact with the circular buffer.
     */
    Circular_Buffer( Interrupt_Controller interrupt_controller ) noexcept :
        m_interrupt_controller{ std::move( interrupt_controller ) }
    {
    }

    Circular_Buffer( Circular_Buffer && ) = delete;

    Circular_Buffer( Circular_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Circular_Buffer() noexcept = default;

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
    auto front_from_interrupt() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the front of the circular buffer.
     */
    auto front_from_interrupt() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back_from_interrupt() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>(
            &m_storage[ ( m_read + size_from_interrupt() - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back_from_interrupt() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>(
            &m_storage[ ( m_read + size_from_interrupt() - 1 ) & index_wrap_around_mask() ] ) );
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
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    [[nodiscard]] auto empty_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == 0;
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
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    [[nodiscard]] auto full_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == max_size_from_interrupt();
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \brief The number of elements in the circular buffer.
     */
    auto size() const noexcept -> Size
    {
        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

        return m_size;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \brief The number of elements in the circular buffer.
     */
    auto size_from_interrupt() const noexcept -> Size
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
     * \brief Get the maximum number of elements the circular buffer is able to hold.
     *
     * \return The maximum number of elements the circular buffer is able to hold.
     */
    auto max_size_from_interrupt() const noexcept -> Size
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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

        new ( &m_storage[ m_write ] ) Value{ std::move( value ) };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

        new ( &m_storage[ m_write ] ) Value{ value };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };

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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

        new ( &m_storage[ m_write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };
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
        PICOLIBRARY_EXPECT( not full(), Generic_Error::WOULD_OVERFLOW );

        new ( &m_storage[ m_write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( m_write + 1 ) & index_wrap_around_mask();

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };
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

        auto const guard = Interrupt::Critical_Section_Guard{ m_interrupt_controller,
                                                              Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };
        ++m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::empty_from_interrupt()
     */
    void pop_from_interrupt() noexcept
    {
        PICOLIBRARY_EXPECT( not empty_from_interrupt(), Generic_Error::WOULD_UNDERFLOW );

        front_from_interrupt().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     */
    void pop_from_interrupt( Bypass_Precondition_Expectation_Checks ) noexcept
    {
        front_from_interrupt().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear_from_interrupt() noexcept
    {
        while ( not empty_from_interrupt() ) {
            pop_from_interrupt( BYPASS_PRECONDITION_EXPECTATION_CHECKS );
        } // while
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
     * \brief The circular buffer element storage read index.
     */
    Index m_read{ 0 };

    /**
     * \brief The circular buffer element storage write index.
     */
    Index m_write{ 0 };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size volatile m_size{ 0 };

    /**
     * \brief The circular buffer element storage.
     */
    Storage volatile m_storage[ N ];

    /**
     * \brief The interrupt controller used by the main thread of execution to manipulate
     *        the interrupt enable state of interrupts that interact with the circular
     *        buffer.
     */
    Interrupt_Controller m_interrupt_controller{};

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

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type (must be trivially destructible).
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer (must be a power of
 *         two).
 */
template<typename T, typename Size_Type, Size_Type N>
class Circular_Buffer<T, Size_Type, N, Circular_Buffer_Use_Case::INTERRUPT_READS_INTERRUPTS_WRITE, void> {
  public:
    static_assert( std::is_trivially_destructible_v<T> );

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
    // NOLINTNEXTLINE(modernize-use-equals-default)
    Circular_Buffer() noexcept
    {
    }

    Circular_Buffer( Circular_Buffer && ) = delete;

    Circular_Buffer( Circular_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Circular_Buffer() noexcept = default;

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
    auto front_from_interrupt() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the front of the circular buffer.
     */
    auto front_from_interrupt() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>( &m_storage[ m_read ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back_from_interrupt() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>(
            &m_storage[ ( m_read + size_from_interrupt() - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Access the element at the back of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The element at the back of the circular buffer.
     */
    auto back_from_interrupt() const noexcept -> Value volatile const &
    {
        return *std::launder( reinterpret_cast<Value volatile const *>(
            &m_storage[ ( m_read + size_from_interrupt() - 1 ) & index_wrap_around_mask() ] ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    [[nodiscard]] auto empty_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == 0;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    [[nodiscard]] auto full_from_interrupt() const noexcept -> bool
    {
        return size_from_interrupt() == max_size_from_interrupt();
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \brief The number of elements in the circular buffer.
     */
    auto size_from_interrupt() const noexcept -> Size
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the circular buffer is able to hold.
     *
     * \return The maximum number of elements the circular buffer is able to hold.
     */
    auto max_size_from_interrupt() const noexcept -> Size
    {
        return N;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \param[in] value The value to insert.
     */
    void push_from_interrupt( Value && value ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::move( value ) };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void push_from_interrupt( Bypass_Precondition_Expectation_Checks, Value && value ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::move( value ) };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Insert a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \param[in] value The value to insert.
     */
    void push_from_interrupt( Value const & value ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ value };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void push_from_interrupt( Bypass_Precondition_Expectation_Checks, Value const & value ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ value };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace_from_interrupt( Arguments &&... arguments ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Emplace a value at the back of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::full_from_interrupt()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     */
    template<typename... Arguments>
    void emplace_from_interrupt( Run_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        PICOLIBRARY_EXPECT( not full_from_interrupt(), Generic_Error::WOULD_OVERFLOW );

        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

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
    void emplace_from_interrupt( Bypass_Precondition_Expectation_Checks, Arguments &&... arguments ) noexcept
    {
        auto const write = Index{ m_write };

        new ( &m_storage[ write ] ) Value{ std::forward<Arguments>( arguments )... };

        m_write = ( write + 1 ) & index_wrap_around_mask();

        ++m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \pre not picolibrary::Circular_Buffer::empty_from_interrupt()
     */
    void pop_from_interrupt() noexcept
    {
        PICOLIBRARY_EXPECT( not empty_from_interrupt(), Generic_Error::WOULD_UNDERFLOW );

        front_from_interrupt().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
    }

    /**
     * \brief Remove the element at the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     */
    void pop_from_interrupt( Bypass_Precondition_Expectation_Checks ) noexcept
    {
        front_from_interrupt().~Value();

        m_read = ( m_read + 1 ) & index_wrap_around_mask();

        --m_size;
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear_from_interrupt() noexcept
    {
        while ( not empty_from_interrupt() ) {
            pop_from_interrupt( BYPASS_PRECONDITION_EXPECTATION_CHECKS );
        } // while
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
     * \brief The circular buffer element storage read index.
     */
    Index m_read{ 0 };

    /**
     * \brief The circular buffer element storage write index.
     */
    Index volatile m_write{ 0 };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size volatile m_size{ 0 };

    /**
     * \brief The circular buffer element storage.
     */
    Storage volatile m_storage[ N ];

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
