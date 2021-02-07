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
 * \brief picolibrary::Result interface.
 */

#ifndef PICOLIBRARY_RESULT_H
#define PICOLIBRARY_RESULT_H

#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/utility.h"

namespace picolibrary {

/**
 * \brief picolibrary::Result error constructor tag.
 */
struct Error_Tag {
};

/**
 * \brief picolibrary::Result error constructor tag.
 */
constexpr auto ERROR = Error_Tag{};

/**
 * \brief picolibrary::Result value constructor tag.
 */
struct Value_Tag {
};

/**
 * \brief picolibrary::Result value constructor tag.
 */
constexpr auto VALUE = Value_Tag{};

/**
 * \brief Operation result wrapper.
 *
 * \tparam Value_Type Operation succeeded result type.
 * \tparam Error_Type Operation failed result type.
 */
template<typename Value_Type, typename Error_Type, bool = std::is_trivially_destructible_v<Value_Type>>
class Result;

/**
 * \brief Operation result wrapper specialized for cases where no information is
 *        generated, and the operation cannot fail.
 */
template<>
class [[nodiscard]] Result<Void, Void, true> final
{
  public:
    /**
     * \brief Operation succeeded result type.
     */
    using Value = Void;

    /**
     * \brief Operation failed result type.
     */
    using Error = Void;

    /**
     * \brief Constructor.
     */
    constexpr Result() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \attention This constructor only exists to provide a consistent interface when
     *            writing generic code. It should never actually be called.
     */
    constexpr Result( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Result() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Result && expression ) noexcept->Result & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Result const & expression ) noexcept->Result & = default;

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return true;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return false (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \attention This function is only provided for interface consistency.
     *
     * \return picolibrary::Void.
     */
    [[nodiscard]] constexpr auto error() const noexcept
    {
        return Error{};
    }
};

/**
 * \brief Operation result wrapper specialized for cases where no information is
 *        generated, and the operation can fail.
 */
template<>
class [[nodiscard]] Result<Void, Error_Code, true> final
{
  public:
    /**
     * \brief Operation succeeded result type.
     */
    using Value = Void;

    /**
     * \brief Operation failed result type.
     */
    using Error = Error_Code;

    /**
     * \brief Constructor.
     *
     * \todo #34
     */
    constexpr Result() noexcept : m_is_value{ true }, m_value{}
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] result The operation result to construct from.
     */
    constexpr Result( Result<Void, Void> result ) noexcept : m_is_value{ true }, m_value{}
    {
        static_cast<void>( result );
    }

    /**
     * \brief Constructor.
     *
     * \tparam E A type implicitly convertible to Error.
     *
     * \param[in] error The object to construct from.
     */
    template<typename E, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<E>, Result> and std::is_convertible_v<E, Error>>>
    constexpr Result( E && error, Error_Tag = {} ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<E>( error ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Error constructor argument types.
     *
     * \param[in] arguments Error constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Error_Tag, Arguments && ... arguments ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept : m_is_value{ source.m_is_value }
    {
        if ( is_error() ) { new ( &m_error ) Error{ std::move( source.m_error ) }; } // if
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept :
        m_is_value{ original.m_is_value }
    {
        if ( is_error() ) { new ( &m_error ) Error{ original.m_error }; } // if
    }

    /**
     * \brief Destructor.
     */
    ~Result() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result && expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_error() ) { m_error = std::move( expression.m_error ); } // if
        } else {
            if ( is_value() ) {
                new ( &m_error ) Error{ std::move( expression.m_error ) };
            } // if

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result const & expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_error() ) { m_error = expression.m_error; } // if
        } else {
            if ( is_value() ) { new ( &m_error ) Error{ expression.m_error }; } // if

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true if the operation result is a value (operation succeeded).
     * \return false if the operation result is not a value (operation failed).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept->bool
    {
        return m_is_value;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return true if the operation result is an error (operation failed).
     * \return false if the operation result is not an error (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept->bool
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto && error() && noexcept
    {
        return static_cast<Error &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const && error() const && noexcept
    {
        return static_cast<Error const &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto & error() & noexcept
    {
        return static_cast<Error &>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const & error() const & noexcept
    {
        return static_cast<Error const &>( m_error );
    }

  private:
    /**
     * \brief Result type flag.
     */
    bool m_is_value{ true };

    /**
     * \cond ANONYMOUS_MEMBER
     *
     * \todo #33
     */
    union {
        /**
         * \todo #33
         *
         * \endcond
         */

        /**
         * \brief Operation succeeded result.
         */
        Value m_value{};

        /**
         * \brief Operation failed result.
         */
        Error m_error;

        /**
         * \cond ANONYMOUS_MEMBER
         *
         * \todo #33
         */
    };
    /**
     * \todo #33
     *
     * \endcond
     */
};

/**
 * \brief Operation result wrapper specialized for cases where trivially destructible
 *        information is generated, and the operation cannot fail.
 *
 * \tparam Value_Type Operation succeeded result type.
 */
template<typename Value_Type>
class [[nodiscard]] Result<Value_Type, Void, true> final
{
  public:
    static_assert( std::is_trivially_destructible_v<Value_Type> );

    /**
     * \brief Operation succeeded result type.
     */
    using Value = Value_Type;

    /**
     * \brief Operation failed result type.
     */
    using Error = Void;

    /**
     * \brief Constructor.
     *
     * \tparam V A type implicitly convertible to Value and not implicitly convertible to
     *           Error.
     *
     * \param[in] value The object to construct from.
     */
    template<typename V, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<V>, Result> and std::is_convertible_v<V, Value> and not std::is_convertible_v<V, Error>>>
    constexpr Result( V && value, Value_Tag = {} ) noexcept :
        m_value{ std::forward<V>( value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Value_Tag, Arguments && ... arguments ) noexcept :
        m_value{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \attention This constructor only exists to provide a consistent interface when
     *            writing generic code. It should never actually be called.
     */
    constexpr Result( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept : m_value{ std::move( source.m_value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept : m_value{ original.m_value }
    {
    }

    /**
     * \brief Destructor.
     */
    ~Result() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result && expression ) noexcept
    {
        m_value = std::move( expression.m_value );

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result const & expression ) noexcept
    {
        m_value = expression.m_value;

        return *this;
    }

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return true;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return false (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto && value() && noexcept
    {
        return static_cast<Value &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const && value() const && noexcept
    {
        return static_cast<Value const &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto & value() & noexcept
    {
        return static_cast<Value &>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const & value() const & noexcept
    {
        return static_cast<Value const &>( m_value );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \attention This function is only provided for interface consistency.
     *
     * \return picolibrary::Void.
     */
    [[nodiscard]] constexpr auto error() const noexcept
    {
        return Error{};
    }

  private:
    /**
     * \cond ANONYMOUS_MEMBER
     *
     * \todo #33
     */
    union {
        /**
         * \todo #33
         *
         * \endcond
         */

        /**
         * \brief Operation succeeded result.
         */
        Value m_value;

        /**
         * \brief Operation failed result.
         */
        Error m_error{};

        /**
         * \cond ANONYMOUS_MEMBER
         *
         * \todo #33
         */
    };
    /**
     * \todo #33
     *
     * \endcond
     */
};

/**
 * \brief Operation result wrapper specialized for cases where non-trivially destructible
 *        information is generated, and the operation cannot fail.
 *
 * \tparam Value_Type Operation succeeded result type.
 */
template<typename Value_Type>
class [[nodiscard]] Result<Value_Type, Void, false> final
{
  public:
    static_assert( not std::is_trivially_destructible_v<Value_Type> );

    /**
     * \brief Operation succeeded result type.
     */
    using Value = Value_Type;

    /**
     * \brief Operation failed result type.
     */
    using Error = Void;

    /**
     * \brief Constructor.
     *
     * \tparam V A type implicitly convertible to Value and not implicitly convertible to
     *           Error.
     *
     * \param[in] value The object to construct from.
     */
    template<typename V, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<V>, Result> and std::is_convertible_v<V, Value> and not std::is_convertible_v<V, Error>>>
    constexpr Result( V && value, Value_Tag = {} ) noexcept :
        m_value{ std::forward<V>( value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Value_Tag, Arguments && ... arguments ) noexcept :
        m_value{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \attention This constructor only exists to provide a consistent interface when
     *            writing generic code. It should never actually be called.
     */
    constexpr Result( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept : m_value{ std::move( source.m_value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept : m_value{ original.m_value }
    {
    }

    /**
     * \brief Destructor.
     */
    ~Result() noexcept
    {
        m_value.~Value();
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result && expression ) noexcept
    {
        m_value = std::move( expression.m_value );

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result const & expression ) noexcept
    {
        m_value = expression.m_value;

        return *this;
    }

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return true;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return false (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto && value() && noexcept
    {
        return static_cast<Value &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const && value() const && noexcept
    {
        return static_cast<Value const &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto & value() & noexcept
    {
        return static_cast<Value &>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const & value() const & noexcept
    {
        return static_cast<Value const &>( m_value );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \attention This function is only provided for interface consistency.
     *
     * \return picolibrary::Void.
     */
    [[nodiscard]] constexpr auto error() const noexcept
    {
        return Error{};
    }

  private:
    /**
     * \cond ANONYMOUS_MEMBER
     *
     * \todo #33
     */
    union {
        /**
         * \todo #33
         *
         * \endcond
         */

        /**
         * \brief Operation succeeded result.
         */
        Value m_value;

        /**
         * \brief Operation failed result.
         */
        Error m_error{};

        /**
         * \cond ANONYMOUS_MEMBER
         *
         * \todo #33
         */
    };
    /**
     * \todo #33
     *
     * \endcond
     */
};

/**
 * \brief Operation result wrapper specialized for cases where trivially destructible
 *        information is generated, and the operation can fail.
 *
 * \tparam Value_Type Operation succeeded result type.
 */
template<typename Value_Type>
class [[nodiscard]] Result<Value_Type, Error_Code, true> final
{
  public:
    static_assert( std::is_trivially_destructible_v<Value_Type> );

    /**
     * \brief Operation succeeded result type.
     */
    using Value = Value_Type;

    /**
     * \brief Operation failed result type.
     */
    using Error = Error_Code;

    /**
     * \brief Constructor.
     *
     * \param[in] result The operation result to construct from.
     */
    constexpr Result( Result<Value, Void> && result ) noexcept :
        m_is_value{ true },
        m_value{ result.value() }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] result The operation result to construct from.
     */
    constexpr Result( Result<Value, Void> const & result ) noexcept :
        m_is_value{ true },
        m_value{ result.value() }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam V A type implicitly convertible to Value and not implicitly convertible to
     *           Error.
     *
     * \param[in] value The object to construct from.
     */
    template<typename V, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<V>, Result> and std::is_convertible_v<V, Value> and not std::is_convertible_v<V, Error>>>
    constexpr Result( V && value, Value_Tag = {} ) noexcept :
        m_is_value{ true },
        m_value{ std::forward<V>( value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Value_Tag, Arguments && ... arguments ) noexcept :
        m_is_value{ true },
        m_value{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam E A type implicitly convertible to Error and not implicitly convertible to
     *           Value.
     *
     * \param[in] error The object to construct from.
     */
    template<typename E, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<E>, Result> and std::is_convertible_v<E, Error> and not std::is_convertible_v<E, Value>>>
    constexpr Result( E && error, Error_Tag = {} ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<E>( error ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Error constructor argument types.
     *
     * \param[in] arguments Error constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Error_Tag, Arguments && ... arguments ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept : m_is_value{ source.m_is_value }
    {
        if ( is_value() ) {
            new ( &m_value ) Value{ std::move( source.m_value ) };
        } else {
            new ( &m_error ) Error{ std::move( source.m_error ) };
        } // else
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept :
        m_is_value{ original.m_is_value }
    {
        if ( is_value() ) {
            new ( &m_value ) Value{ original.m_value };
        } else {
            new ( &m_error ) Error{ original.m_error };
        } // else
    }

    /**
     * \brief Destructor.
     */
    ~Result() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result && expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_value() ) {
                m_value = std::move( expression.m_value );
            } else {
                m_error = std::move( expression.m_error );
            } // else
        } else {
            if ( is_value() ) {
                new ( &m_error ) Error{ std::move( expression.m_error ) };
            } else {
                new ( &m_value ) Error{ std::move( expression.m_value ) };
            } // else

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result const & expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_value() ) {
                m_value = expression.m_value;
            } else {
                m_error = expression.m_error;
            } // else
        } else {
            if ( is_value() ) {
                new ( &m_error ) Error{ expression.m_error };
            } else {
                new ( &m_value ) Error{ expression.m_value };
            } // else

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true if the operation result is a value (operation succeeded).
     * \return false if the operation result is not a value (operation failed).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return m_is_value;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return true if the operation result is an error (operation failed).
     * \return false if the operation result is not an error (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto && value() && noexcept
    {
        return static_cast<Value &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const && value() const && noexcept
    {
        return static_cast<Value const &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto & value() & noexcept
    {
        return static_cast<Value &>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const & value() const & noexcept
    {
        return static_cast<Value const &>( m_value );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto && error() && noexcept
    {
        return static_cast<Error &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const && error() const && noexcept
    {
        return static_cast<Error const &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto & error() & noexcept
    {
        return static_cast<Error &>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const & error() const & noexcept
    {
        return static_cast<Error const &>( m_error );
    }

  private:
    /**
     * \brief Result type flag.
     */
    bool m_is_value;

    /**
     * \cond ANONYMOUS_MEMBER
     *
     * \todo #33
     */
    union {
        /**
         * \todo #33
         *
         * \endcond
         */

        /**
         * \brief Operation succeeded result.
         */
        Value m_value;

        /**
         * \brief Operation failed result.
         */
        Error m_error;

        /**
         * \cond ANONYMOUS_MEMBER
         *
         * \todo #33
         */
    };
    /**
     * \todo #33
     *
     * \endcond
     */
};

/**
 * \brief Operation result wrapper specialized for cases where non-trivially destructible
 *        information is generated, and the operation can fail.
 *
 * \tparam Value_Type Operation succeeded result type.
 */
template<typename Value_Type>
class [[nodiscard]] Result<Value_Type, Error_Code, false> final
{
  public:
    static_assert( not std::is_trivially_destructible_v<Value_Type> );

    /**
     * \brief Operation succeeded result type.
     */
    using Value = Value_Type;

    /**
     * \brief Operation failed result type.
     */
    using Error = Error_Code;

    /**
     * \brief Constructor.
     *
     * \param[in] result The operation result to construct from.
     */
    constexpr Result( Result<Value, Void> && result ) noexcept :
        m_is_value{ true },
        m_value{ result.value() }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] result The operation result to construct from.
     */
    constexpr Result( Result<Value, Void> const & result ) noexcept :
        m_is_value{ true },
        m_value{ result.value() }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam V A type implicitly convertible to Value and not implicitly convertible to
     *           Error.
     *
     * \param[in] value The object to construct from.
     */
    template<typename V, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<V>, Result> and std::is_convertible_v<V, Value> and not std::is_convertible_v<V, Error>>>
    constexpr Result( V && value, Value_Tag = {} ) noexcept :
        m_is_value{ true },
        m_value{ std::forward<V>( value ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Value constructor argument types.
     *
     * \param[in] arguments Value constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Value_Tag, Arguments && ... arguments ) noexcept :
        m_is_value{ true },
        m_value{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam E A type implicitly convertible to Error and not implicitly convertible to
     *           Value.
     *
     * \param[in] error The object to construct from.
     */
    template<typename E, typename = typename std::enable_if_t<not std::is_same_v<std::decay_t<E>, Result> and std::is_convertible_v<E, Error> and not std::is_convertible_v<E, Value>>>
    constexpr Result( E && error, Error_Tag = {} ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<E>( error ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Arguments Error constructor argument types.
     *
     * \param[in] arguments Error constructor arguments.
     */
    template<typename... Arguments>
    constexpr Result( Error_Tag, Arguments && ... arguments ) noexcept :
        m_is_value{ false },
        m_error{ std::forward<Arguments>( arguments )... }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept : m_is_value{ source.m_is_value }
    {
        if ( is_value() ) {
            new ( &m_value ) Value{ std::move( source.m_value ) };
        } else {
            new ( &m_error ) Error{ std::move( source.m_error ) };
        } // else
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept :
        m_is_value{ original.m_is_value }
    {
        if ( is_value() ) {
            new ( &m_value ) Value{ original.m_value };
        } else {
            new ( &m_error ) Error{ original.m_error };
        } // else
    }

    /**
     * \brief Destructor.
     */
    ~Result() noexcept
    {
        if ( is_value() ) { m_value.~Value(); } // if
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result && expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_value() ) {
                m_value = std::move( expression.m_value );
            } else {
                m_error = std::move( expression.m_error );
            } // else
        } else {
            if ( is_value() ) {
                m_value.~Value();
                new ( &m_error ) Error{ std::move( expression.m_error ) };
            } else {
                new ( &m_value ) Error{ std::move( expression.m_value ) };
            } // else

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Result const & expression ) noexcept
    {
        if ( is_value() == expression.is_value() ) {
            if ( is_value() ) {
                m_value = expression.m_value;
            } else {
                m_error = expression.m_error;
            } // else
        } else {
            if ( is_value() ) {
                m_value.~Value();
                new ( &m_error ) Error{ expression.m_error };
            } else {
                new ( &m_value ) Error{ expression.m_value };
            } // else

            m_is_value = expression.m_is_value;
        } // else

        return *this;
    }

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true if the operation result is a value (operation succeeded).
     * \return false if the operation result is not a value (operation failed).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return m_is_value;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return true if the operation result is an error (operation failed).
     * \return false if the operation result is not an error (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return not is_value();
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto && value() && noexcept
    {
        return static_cast<Value &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const && value() const && noexcept
    {
        return static_cast<Value const &&>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto & value() & noexcept
    {
        return static_cast<Value &>( m_value );
    }

    /**
     * \brief Access the result of a successful operation.
     *
     * \pre The operation succeeded.
     *
     * \warning Calling this function on the result of a failed operation results in
     *          undefined behavior.
     *
     * \return The generated information.
     */
    [[nodiscard]] constexpr auto const & value() const & noexcept
    {
        return static_cast<Value const &>( m_value );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto && error() && noexcept
    {
        return static_cast<Error &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const && error() const && noexcept
    {
        return static_cast<Error const &&>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto & error() & noexcept
    {
        return static_cast<Error &>( m_error );
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \pre The operation failed.
     *
     * \warning Calling this function on the result of a successful operation results in
     *          undefined behavior.
     *
     * \return The result error.
     */
    [[nodiscard]] constexpr auto const & error() const & noexcept
    {
        return static_cast<Error const &>( m_error );
    }

  private:
    /**
     * \brief Result type flag.
     */
    bool m_is_value;

    /**
     * \cond ANONYMOUS_MEMBER
     *
     * \todo #33
     */
    union {
        /**
         * \todo #33
         *
         * \endcond
         */

        /**
         * \brief Operation succeeded result.
         */
        Value m_value;

        /**
         * \brief Operation failed result.
         */
        Error m_error;

        /**
         * \cond ANONYMOUS_MEMBER
         *
         * \todo #33
         */
    };
    /**
     * \todo #33
     *
     * \endcond
     */
};

} // namespace picolibrary

#endif // PICOLIBRARY_RESULT_H
