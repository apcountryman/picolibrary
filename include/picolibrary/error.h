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
 * \brief picolibrary::Error interface.
 */

#ifndef PICOLIBRARY_ERROR_H
#define PICOLIBRARY_ERROR_H

#include <cstdint>
#include <type_traits>

#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief Error ID number.
 *
 * \attention This type is the underlying integer type for all error code enums.
 */
using Error_ID = std::uint_fast8_t;

/**
 * \brief Check if an enum is an error code enum.
 *
 * \tparam Enum The enum to check.
 */
template<typename Enum>
struct is_error_code_enum : std::false_type {
};

/**
 * \copydoc picolibrary::is_error_code_enum
 */
template<typename Enum>
constexpr auto is_error_code_enum_v = is_error_code_enum<Enum>::value;

/**
 * \brief Error category.
 */
class Error_Category {
  public:
    Error_Category( Error_Category && ) = delete;

    Error_Category( Error_Category const & ) = delete;

    auto operator=( Error_Category && ) = delete;

    auto operator=( Error_Category const & ) = delete;

    /**
     * \brief Get the name of the error category.
     *
     * \return The name of the error category if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION is not defined.
     * \return An empty string if PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION is
     *         defined.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto name() const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    auto const * name() const noexcept
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

    /**
     * \brief Get an error ID's description.
     *
     * \param[in] id The error ID whose description is to be got.
     *
     * \return The error ID's description if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION is not defined.
     * \return An empty string if PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION is
     *         defined.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto error_description( Error_ID id ) const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    auto const * error_description( Error_ID id ) const noexcept
    {
        static_cast<void>( id );

        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Error_Category() noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Error_Category() noexcept = default;
};

/**
 * \brief Error code.
 */
class Error_Code final {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Error_Code() noexcept = default;

    /**
     * \brief Constructor.
     */
    constexpr Error_Code( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] category The error's category.
     * \param[in] id The error's ID.
     */
    constexpr Error_Code( Error_Category const & category, Error_ID id ) noexcept :
        m_category{ &category },
        m_id{ id }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Error_Code_Enum The type of error code enum to construct from.
     *
     * \attention Error_Code_Enum must have been registered as an error code enum by
     *            specializing picolibrary::is_error_code_enum for this constructor to be
     *            considered during overload resolution.
     * \attention Error_Code_Enum's underlying integer type must be picolibrary::Error_ID.
     * \attention An error code builder with the following name and signature must be
     *            provided:
     * \code
     * picolibrary::Error_Code make_error_code( Error_Code_Enum );
     * \endcode
     *
     * \param[in] error The error.
     */
    template<typename Error_Code_Enum, typename = std::enable_if_t<is_error_code_enum_v<Error_Code_Enum>>>
    Error_Code( Error_Code_Enum error ) noexcept
    {
        static_assert( std::is_same_v<std::underlying_type_t<Error_Code_Enum>, Error_ID> );

        *this = make_error_code( error );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Error_Code( Error_Code && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Error_Code( Error_Code const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Error_Code() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Error_Code && expression ) noexcept -> Error_Code & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Error_Code const & expression ) noexcept -> Error_Code & = default;

    /**
     * \brief Check if the picolibrary::Error_Code identifies as an error (i.e. was not
     *        default constructed, and was not constructed from picolibrary::Void).
     *
     * \return true if the picolibrary::Error_Code identifies as an error.
     * \return false if the picolibrary::Error_Code does not identify as an error.
     */
    constexpr explicit operator bool() const noexcept
    {
        return m_category != &Default_Error_Category::instance();
    }

    /**
     * \brief Get the error's category.
     *
     * \return The error's category.
     */
    constexpr auto const & category() const noexcept
    {
        return *m_category;
    }

    /**
     * \brief Get the error's ID.
     *
     * \return The error's ID.
     */
    constexpr auto id() const noexcept
    {
        return m_id;
    }

    /**
     * \brief Get the error's description.
     *
     * \return The error's description.
     */
    auto description() const noexcept
    {
        return category().error_description( id() );
    }

  private:
    /**
     * \brief Default error category.
     */
    class Default_Error_Category final : public Error_Category {
      public:
        /**
         * \brief Get a reference to the default error category instance.
         *
         * \return A reference to the default error category instance.
         */
        static constexpr auto instance() noexcept -> Default_Error_Category const &
        {
            return INSTANCE;
        }

        Default_Error_Category( Default_Error_Category && ) = delete;

        Default_Error_Category( Default_Error_Category const & ) = delete;

        auto operator=( Default_Error_Category && ) = delete;

        auto operator=( Default_Error_Category const & ) = delete;

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
        /**
         * \copydoc picolibrary::Error_Category::name()
         */
        virtual auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::Default_Error";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
        /**
         * \copydoc picolibrary::Error_Category::error_description()
         */
        virtual auto error_description( Error_ID id ) const noexcept -> char const * override final
        {
            static_cast<void>( id );

            return "NONE";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

      private:
        /**
         * \brief The default error category instance.
         */
        static Default_Error_Category const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Default_Error_Category() noexcept = default;

        /**
         * \brief Destructor.
         */
        ~Default_Error_Category() noexcept = default;
    };

    /**
     * \brief The error's category.
     */
    Error_Category const * m_category{ &Default_Error_Category::instance() };

    /**
     * \brief The error's ID.
     */
    Error_ID m_id{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Error_Code
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Error_Code const & lhs, Error_Code const & rhs ) noexcept
{
    return &lhs.category() == &rhs.category() and lhs.id() == rhs.id();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Error_Code
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Error_Code const & lhs, Error_Code const & rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Generic errors.
 *
 * \relatedalso picolibrary::Generic_Error_Category
 */
enum class Generic_Error : Error_ID {
    INVALID_ARGUMENT,                ///< Invalid argument.
    UNSUPPORTED_OPERATION,           ///< Unsupported operation.
    OPERATION_TIMEOUT,               ///< Operation timeout.
    IO_STREAM_DEGRADED,              ///< I/O stream degraded.
    INVALID_FORMAT,                  ///< Invalid format string.
    NONRESPONSIVE_DEVICE,            ///< Nonresponsive device.
    ARBITRATION_LOST,                ///< Arbitration lost.
    LOGIC_ERROR,                     ///< Logic error.
    BUS_ERROR,                       ///< Bus error.
    CIRCULAR_BUFFER_WOULD_OVERFLOW,  ///< Circular buffer would overflow.
    CIRCULAR_BUFFER_WOULD_UNDERFLOW, ///< Circular buffer would underflow.
};

/**
 * \brief Generic error category.
 */
class Generic_Error_Category final : public Error_Category {
  public:
    /**
     * \brief Get a reference to the generic error category instance.
     *
     * \return A reference to the generic error category instance.
     */
    static constexpr auto const & instance() noexcept
    {
        return INSTANCE;
    }

    Generic_Error_Category( Generic_Error_Category && ) = delete;

    Generic_Error_Category( Generic_Error_Category const & ) = delete;

    auto operator=( Generic_Error_Category && ) = delete;

    auto operator=( Generic_Error_Category const & ) = delete;

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    /**
     * \copydoc picolibrary::Error_Category::name()
     */
    virtual auto name() const noexcept -> char const * override final
    {
        return "::picolibrary::Generic_Error";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    /**
     * \copydoc picolibrary::Error_Category::error_description()
     */
    virtual auto error_description( Error_ID id ) const noexcept -> char const * override final
    {
        switch ( static_cast<Generic_Error>( id ) ) {
            case Generic_Error::INVALID_ARGUMENT: return "INVALID_ARGUMENT";
            case Generic_Error::UNSUPPORTED_OPERATION: return "UNSUPPORTED_OPERATION";
            case Generic_Error::OPERATION_TIMEOUT: return "OPERATION_TIMEOUT";
            case Generic_Error::IO_STREAM_DEGRADED: return "IO_STREAM_DEGRADED";
            case Generic_Error::INVALID_FORMAT: return "INVALID_FORMAT";
            case Generic_Error::NONRESPONSIVE_DEVICE: return "NONRESPONSIVE_DEVICE";
            case Generic_Error::ARBITRATION_LOST: return "ARBITRATION_LOST";
            case Generic_Error::LOGIC_ERROR: return "LOGIC_ERROR";
            case Generic_Error::BUS_ERROR: return "BUS_ERROR";
            case Generic_Error::CIRCULAR_BUFFER_WOULD_OVERFLOW:
                return "CIRCULAR_BUFFER_WOULD_OVERFLOW";
            case Generic_Error::CIRCULAR_BUFFER_WOULD_UNDERFLOW:
                return "CIRCULAR_BUFFER_WOULD_UNDERFLOW";
        } // switch

        return "UNKNOWN";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

  private:
    /**
     * \brief The generic error category instance.
     */
    static Generic_Error_Category const INSTANCE;

    /**
     * \brief Constructor.
     */
    constexpr Generic_Error_Category() noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Generic_Error_Category() noexcept = default;
};

/**
 * \brief Build an error code from a generic error.
 *
 * \relatedalso picolibrary::Generic_Error_Category
 *
 * \param[in] error The generic error to build the error code from.
 *
 * \return The built error code.
 */
inline auto make_error_code( Generic_Error error ) noexcept
{
    return Error_Code{ Generic_Error_Category::instance(), static_cast<Error_ID>( error ) };
}

/**
 * \brief picolibrary::Generic_Error error code enum registration.
 *
 * \relatedalso picolibrary::Generic_Error_Category
 */
template<>
struct is_error_code_enum<Generic_Error> : std::true_type {
};

} // namespace picolibrary

#endif // PICOLIBRARY_ERROR_H
