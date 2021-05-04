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
 * \brief picolibrary::Circular_Buffer interface.
 */

#ifndef PICOLIBRARY_CIRCULAR_BUFFER_H
#define PICOLIBRARY_CIRCULAR_BUFFER_H

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/interrupt_guard.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief picolibrary::Circular_Buffer interrupts supported interrupt support policy.
 */
struct With_Interrupt_Support {
};

/**
 * \brief picolibrary::Circular_Buffer interrupts not supported interrupt support policy.
 */
struct Without_Interrupt_Support {
};

/**
 * \brief picolibrary::Circular_Buffer overflow/underflow protection enabled policy.
 */
struct With_Overflow_Underflow_Protection {
};

/**
 * \brief picolibrary::Circular_Buffer overflow/underflow protection disabled policy.
 */
struct Without_Overflow_Underflow_Protection {
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam N The maximum number of elements in the circular buffer.
 * \tparam Interrupt_Support The circular buffer interrupt support policy
 *         (picolibrary::With_Interrupt_Support or
 *         picolibrary::Without_Interrupt_Support).
 * \tparam Overflow_Underflow_Protection The circular buffer overflow/underflow protection
 *         policy (picolibrary::With_Overflow_Underflow_Protection or
 *         picolibrary::Without_Overflow_Underflow_Protection).
 */
template<typename T, std::size_t N, typename Interrupt_Support, typename Overflow_Underflow_Protection>
class Circular_Buffer;

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam N The maximum number of elements in the circular buffer.
 */
template<typename T, std::size_t N>
class Circular_Buffer<T, N, Without_Interrupt_Support, Without_Overflow_Underflow_Protection> {
  public:
    static_assert( N > 0 );

    /**
     * \brief The circular buffer element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the circular buffer.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Circular_Buffer() noexcept = default;

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
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>( m_read ) );
    }

    /**
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>( m_read ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    auto empty() const noexcept
    {
        return not m_size;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    auto full() const noexcept
    {
        return m_size == N;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \return The number of elements in the circular buffer.
     */
    auto size() const noexcept
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the circular buffer can hold.
     *
     * \return The maximum number of elements the circular buffer can hold.
     */
    auto max_size() const noexcept
    {
        return N;
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push( Value && value ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ std::move( value ) };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push( Value const & value ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ value };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer, constructing the element
     *        in-place.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     *
     * \return Success.
     */
    template<typename... Arguments>
    auto emplace( Arguments &&... arguments ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ std::forward<Arguments>( arguments )... };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Remove an element from the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return Success.
     */
    auto pop() noexcept -> Result<Void, Void>
    {
        std::launder( reinterpret_cast<Value *>( m_read ) )->~Value();

        m_read = m_read + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_read + 1;

        --m_size;

        return {};
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear() noexcept
    {
        while ( not empty() ) { static_cast<void>( pop() ); } // while
    }

  private:
    /**
     * \brief Circular buffer element storage.
     */
    using Storage = std::aligned_storage_t<sizeof( Value ), alignof( Value )>;

    /**
     * \brief The circular buffer read pointer.
     */
    Storage * m_read{ &m_storage[ 0 ] };

    /**
     * \brief The circular buffer write pointer.
     */
    Storage * m_write{ &m_storage[ 0 ] };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size m_size{ 0 };

    /**
     * \brief The circular buffer storage.
     */
    Storage m_storage[ N ];
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam N The maximum number of elements in the circular buffer.
 */
template<typename T, std::size_t N>
class Circular_Buffer<T, N, Without_Interrupt_Support, With_Overflow_Underflow_Protection> {
  public:
    static_assert( N > 0 );

    /**
     * \brief The circular buffer element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the circular buffer.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Circular_Buffer() noexcept = default;

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
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>( m_read ) );
    }

    /**
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>( m_read ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() noexcept -> Value &
    {
        return *std::launder( reinterpret_cast<Value *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() const noexcept -> Value const &
    {
        return *std::launder( reinterpret_cast<Value const *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    auto empty() const noexcept
    {
        return not m_size;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    auto full() const noexcept
    {
        return m_size == N;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \return The number of elements in the circular buffer.
     */
    auto size() const noexcept
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the circular buffer can hold.
     *
     * \return The maximum number of elements the circular buffer can hold.
     */
    auto max_size() const noexcept
    {
        return N;
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Nothing if the circular buffer is not full.
     * \return picolibrary::Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW if the circular
     *         buffer is full.
     */
    auto push( Value && value ) noexcept -> Result<Void, Error_Code>
    {
        if ( full() ) {
            return Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW;
        } // if

        new ( m_write ) Value{ std::move( value ) };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Nothing if the circular buffer is not full.
     * \return picolibrary::Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW if the circular
     *         buffer is full.
     */
    auto push( Value const & value ) noexcept -> Result<Void, Error_Code>
    {
        if ( full() ) {
            return Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW;
        } // if

        new ( m_write ) Value{ value };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer, constructing the element
     *        in-place.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     *
     * \return Nothing if the circular buffer is not full.
     * \return picolibrary::Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW if the circular
     *         buffer is full.
     */
    template<typename... Arguments>
    auto emplace( Arguments &&... arguments ) noexcept -> Result<Void, Error_Code>
    {
        if ( full() ) {
            return Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW;
        } // if

        new ( m_write ) Value{ std::forward<Arguments>( arguments )... };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Remove an element from the front of the circular buffer.
     *
     * \return Nothing if the circular buffer is not empty.
     * \return picolibrary::Generic_Error::CIRCULAR_BUFFER_WOULD_UNDERFLOW if the circular
     *         buffer is empty.
     */
    auto pop() noexcept -> Result<Void, Error_Code>
    {
        if ( empty() ) {
            return Generic_Error::CIRCULAR_BUFFER_WOULD_UNDERFLOW;
        } // if

        std::launder( reinterpret_cast<Value *>( m_read ) )->~Value();

        m_read = m_read + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_read + 1;

        --m_size;

        return {};
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear() noexcept
    {
        while ( not empty() ) { static_cast<void>( pop() ); } // while
    }

  private:
    /**
     * \brief Circular buffer element storage.
     */
    using Storage = std::aligned_storage_t<sizeof( Value ), alignof( Value )>;

    /**
     * \brief The circular buffer read pointer.
     */
    Storage * m_read{ &m_storage[ 0 ] };

    /**
     * \brief The circular buffer write pointer.
     */
    Storage * m_write{ &m_storage[ 0 ] };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size m_size{ 0 };

    /**
     * \brief The circular buffer storage.
     */
    Storage m_storage[ N ];
};

/**
 * \brief Circular buffer.
 *
 * \attention This circular buffer implementation assumes that either only the main thread
 *            of execution reads data from the circular buffer, or only the interrupt
 *            reads data from the circular buffer. Both the main thread of execution and
 *            the interrupt may write data to the circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam N The maximum number of elements in the circular buffer.
 */
template<typename T, std::size_t N>
class Circular_Buffer<T, N, With_Interrupt_Support, Without_Overflow_Underflow_Protection> {
  public:
    static_assert( N > 0 );

    /**
     * \brief The circular buffer element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the circular buffer.
     */
    using Size = std::size_t;

    /**
     * \brief Constructor.
     */
    Circular_Buffer() noexcept = default;

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
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>( m_read ) );
    }

    /**
     * \brief Access the first element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The first element in the circular buffer.
     */
    auto front() const noexcept -> Value const volatile &
    {
        return *std::launder( reinterpret_cast<Value const volatile *>( m_read ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() noexcept -> Value volatile &
    {
        Storage volatile * write;
        {
            auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

            write = m_write;
        }

        return *std::launder( reinterpret_cast<Value volatile *>(
            write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : write - 1 ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back_from_interrupt() noexcept -> Value volatile &
    {
        return *std::launder( reinterpret_cast<Value volatile *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back() const noexcept -> Value const volatile &
    {
        Storage const volatile * write;
        {
            auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

            write = m_write;
        }

        return *std::launder( reinterpret_cast<Value const volatile *>(
            write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : write - 1 ) );
    }

    /**
     * \brief Access the last element in the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return The last element in the circular buffer.
     */
    auto back_from_interrupt() const noexcept -> Value const volatile &
    {
        return *std::launder( reinterpret_cast<Value const volatile *>(
            m_write == &m_storage[ 0 ] ? &m_storage[ N - 1 ] : m_write - 1 ) );
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    auto empty() const noexcept
    {
        Size size;
        {
            auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

            size = m_size;
        }

        return not size;
    }

    /**
     * \brief Check if the circular buffer is empty.
     *
     * \return true if the circular buffer is empty.
     * \return false if the circular buffer is not empty.
     */
    auto empty_from_interrupt() const noexcept
    {
        return not m_size;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    auto full() const noexcept
    {
        Size size;
        {
            auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

            size = m_size;
        }

        return size == N;
    }

    /**
     * \brief Check if the circular buffer is full.
     *
     * \return true if the circular buffer is full.
     * \return false if the circular buffer is not full.
     */
    auto full_from_interrupt() const noexcept
    {
        return m_size == N;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \return The number of elements in the circular buffer.
     */
    auto size() const noexcept
    {
        auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

        return m_size;
    }

    /**
     * \brief Get the number of elements in the circular buffer.
     *
     * \return The number of elements in the circular buffer.
     */
    auto size_from_interrupt() const noexcept
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the circular buffer can hold.
     *
     * \return The maximum number of elements the circular buffer can hold.
     */
    auto max_size() const noexcept
    {
        return N;
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push( Value && value ) noexcept -> Result<Void, Void>
    {
        auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

        new ( m_write ) Value{ std::move( value ) };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push_from_interrupt( Value && value ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ std::move( value ) };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push( Value const & value ) noexcept -> Result<Void, Void>
    {
        auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

        new ( m_write ) Value{ value };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \param[in] value The value to push to the end of the circular buffer.
     *
     * \return Success.
     */
    auto push_from_interrupt( Value const & value ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ value };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer, constructing the element
     *        in-place.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     *
     * \return Success.
     */
    template<typename... Arguments>
    auto emplace( Arguments &&... arguments ) noexcept -> Result<Void, Void>
    {
        auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

        new ( m_write ) Value{ std::forward<Arguments>( arguments )... };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Push a value to the end of the circular buffer, constructing the element
     *        in-place.
     *
     * \warning Calling this function on a full circular buffer results in undefined
     *          behavior.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     *
     * \return Success.
     */
    template<typename... Arguments>
    auto emplace_from_interrupt( Arguments &&... arguments ) noexcept -> Result<Void, Void>
    {
        new ( m_write ) Value{ std::forward<Arguments>( arguments )... };

        m_write = m_write + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_write + 1;

        ++m_size;

        return {};
    }

    /**
     * \brief Remove an element from the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return Success.
     */
    auto pop() noexcept -> Result<Void, Void>
    {
        std::launder( reinterpret_cast<Value volatile *>( m_read ) )->~Value();

        m_read = m_read + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_read + 1;

        auto guard = Interrupt_Guard<Restore_Interrupt_Enable_State>{};

        --m_size;

        return {};
    }

    /**
     * \brief Remove an element from the front of the circular buffer.
     *
     * \warning Calling this function on an empty circular buffer results in undefined
     *          behavior.
     *
     * \return Success.
     */
    auto pop_from_interrupt() noexcept -> Result<Void, Void>
    {
        std::launder( reinterpret_cast<Value volatile *>( m_read ) )->~Value();

        m_read = m_read + 1 == &m_storage[ N ] ? &m_storage[ 0 ] : m_read + 1;

        --m_size;

        return {};
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear() noexcept
    {
        while ( not empty() ) { static_cast<void>( pop() ); } // while
    }

    /**
     * \brief Remove all circular buffer elements.
     */
    void clear_from_interrupt() noexcept
    {
        while ( not empty_from_interrupt() ) {
            static_cast<void>( pop_from_interrupt() );
        } // while
    }

  private:
    /**
     * \brief Circular buffer element storage.
     */
    using Storage = std::aligned_storage_t<sizeof( Value ), alignof( Value )>;

    /**
     * \brief The circular buffer read pointer.
     */
    Storage volatile * m_read{ &m_storage[ 0 ] };

    /**
     * \brief The circular buffer write pointer.
     */
    Storage volatile * volatile m_write{ &m_storage[ 0 ] };

    /**
     * \brief The number of elements in the circular buffer.
     */
    Size volatile m_size{ 0 };

    /**
     * \brief The circular buffer storage.
     */
    Storage volatile m_storage[ N ];
};

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
