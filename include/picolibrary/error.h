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
 * \brief picolibrary error interface.
 */

#ifndef PICOLIBRARY_ERROR_H
#define PICOLIBRARY_ERROR_H

#include <cstdint>
#include <type_traits>

#include "picolibrary/utility.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief Error ID.
 *
 * \attention This type is the expected underlying type for all error code enums.
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
 * \brief Check if an enum is an error code enum.
 *
 * \tparam Enum The enum to check.
 */
template<typename Enum>
inline constexpr auto is_error_code_enum_v = is_error_code_enum<Enum>::value;

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
     * \return The name of the error category.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto name() const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    constexpr auto name() const noexcept -> char const *
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

    /**
     * \brief Get an error ID's description.
     *
     * \param[in] id The error ID whose description is to be got.
     *
     * \return The error ID's description.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto error_description( Error_ID id ) const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    constexpr auto error_description( Error_ID id ) const noexcept -> char const *
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
     * \attention An error code construction helper function with the following name and
     *            signature must be provided:
     * \code
     * auto make_error_code( Error_Code_Enum error ) noexcept -> picolibrary::Error_Code;
     * \endcode
     *
     * \param[in] error The error.
     */
    template<typename Error_Code_Enum, typename = std::enable_if_t<is_error_code_enum_v<Error_Code_Enum>>>
    constexpr Error_Code( Error_Code_Enum error ) noexcept :
        Error_Code{ make_error_code( error ) }
    {
        static_assert( std::is_same_v<std::underlying_type_t<Error_Code_Enum>, Error_ID> );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Error_Code( Error_Code && source ) = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Error_Code( Error_Code const & original ) = default;

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
     * \brief Get the error's category.
     *
     * \return The error's category.
     */
    constexpr auto category() const noexcept -> Error_Category const &
    {
        return *m_category;
    }

    /**
     * \brief Get the error's ID.
     *
     * \return The error's ID.
     */
    constexpr auto id() const noexcept -> Error_ID
    {
        return m_id;
    }

    /**
     * \brief Get the error's description.
     *
     * \return The error's description.
     */
    auto description() const noexcept -> char const *
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
         * \brief Get the name of the error category.
         *
         * \return The name of the error category.
         */
        auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::Default_Error";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
        /**
         * \brief Get an error ID's description.
         *
         * \param[in] id The error ID whose description is to be got.
         *
         * \return The error ID's description.
         */
        auto error_description( Error_ID id ) const noexcept -> char const * override final
        {
            static_cast<void>( id );

            return "UNKNOWN";
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
constexpr auto operator==( Error_Code const & lhs, Error_Code const & rhs ) noexcept -> bool
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
constexpr auto operator!=( Error_Code const & lhs, Error_Code const & rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Generic errors.
 *
 * \relatedalso picolibrary::Generic_Error_Category
 */
enum class Generic_Error : Error_ID {
    ARBITRATION_LOST,                 ///< Arbitration lost.
    BUS_ERROR,                        ///< Bus error.
    INSUFFICIENT_CAPACITY,            ///< Insufficient capacity.
    INVALID_ARGUMENT,                 ///< Invalid argument.
    IO_STREAM_DEGRADED,               ///< I/O stream degraded.
    LOGIC_ERROR,                      ///< Logic error.
    NONRESPONSIVE_DEVICE,             ///< Nonresponsive device.
    NOT_CONNECTED,                    ///< Not connected.
    OPERATION_TIMEOUT,                ///< Operation timeout.
    OUT_OF_RANGE,                     ///< Out of range.
    RUNTIME_ERROR,                    ///< Runtime error.
    UNEXPECTED_EVENT_HANDLING_RESULT, ///< Unexpected event handling result.
    WOULD_BLOCK,                      ///< Would block.
    WOULD_OVERFLOW,                   ///< Would overflow.
    WOULD_UNDERFLOW,                  ///< Would underflow.
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
    static constexpr auto instance() noexcept -> Generic_Error_Category const &
    {
        return INSTANCE;
    }

    Generic_Error_Category( Generic_Error_Category && ) = delete;

    Generic_Error_Category( Generic_Error_Category const & ) = delete;

    auto operator=( Generic_Error_Category && ) = delete;

    auto operator=( Generic_Error_Category const & ) = delete;

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    /**
     * \brief Get the name of the error category.
     *
     * \return The name of the error category.
     */
    auto name() const noexcept -> char const * override final
    {
        return "::picolibrary::Generic_Error";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    /**
     * \brief Get an error ID's description.
     *
     * \param[in] id The error ID whose description is to be got.
     *
     * \return The error ID's description.
     */
    auto error_description( Error_ID id ) const noexcept -> char const * override final
    {
        switch ( static_cast<Generic_Error>( id ) ) {
                // clang-format off

            case Generic_Error::ARBITRATION_LOST: return "ARBITRATION_LOST";
            case Generic_Error::BUS_ERROR: return "BUS_ERROR";
            case Generic_Error::INSUFFICIENT_CAPACITY: return "INSUFFICIENT_CAPACITY";
            case Generic_Error::INVALID_ARGUMENT: return "INVALID_ARGUMENT";
            case Generic_Error::IO_STREAM_DEGRADED: return "IO_STREAM_DEGRADED";
            case Generic_Error::LOGIC_ERROR: return "LOGIC_ERROR";
            case Generic_Error::NONRESPONSIVE_DEVICE: return "NONRESPONSIVE_DEVICE";
            case Generic_Error::NOT_CONNECTED: return "NOT_CONNECTED";
            case Generic_Error::OPERATION_TIMEOUT: return "OPERATION_TIMEOUT";
            case Generic_Error::OUT_OF_RANGE: return "OUT_OF_RANGE";
            case Generic_Error::RUNTIME_ERROR: return "RUNTIME_ERROR";
            case Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT: return "UNEXPECTED_EVENT_HANDLING_RESULT";
            case Generic_Error::WOULD_BLOCK: return "WOULD_BLOCK";
            case Generic_Error::WOULD_OVERFLOW: return "WOULD_OVERFLOW";
            case Generic_Error::WOULD_UNDERFLOW: return "WOULD_UNDERFLOW";

                // clang-format on
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
 * \brief Construct an error code from a generic error.
 *
 * \relatedalso picolibrary::Generic_Error_Category
 *
 * \param[in] error The generic error to construct the error code from.
 *
 * \return The constructed error code.
 */
inline auto make_error_code( Generic_Error error ) noexcept -> Error_Code
{
    return Error_Code{ Generic_Error_Category::instance(), to_underlying( error ) };
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
