/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Automated error interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_ERROR_H
#define PICOLIBRARY_TESTING_AUTOMATED_ERROR_H

#include <cstdint>
#include <ios>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/utility.h"

namespace picolibrary {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Generic_Error to.
 * \param[in] generic_error The picolibrary::Generic_Error to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Generic_Error generic_error ) -> std::ostream &
{
    // #lizard forgives the length

    switch ( generic_error ) {
            // clang-format off

        case Generic_Error::ARBITRATION_LOST:                 return stream << "::picolibrary::Generic_Error::ARBITRATION_LOST";
        case Generic_Error::BUS_ERROR:                        return stream << "::picolibrary::Generic_Error::BUS_ERROR";
        case Generic_Error::ENDPOINT_IN_USE:                  return stream << "::picolibrary::Generic_Error::ENDPOINT_IN_USE";
        case Generic_Error::EPHEMERAL_PORTS_EXHAUSTED:        return stream << "::picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED";
        case Generic_Error::EXCESSIVE_MESSAGE_SIZE:           return stream << "::picolibrary::Generic_Error::EXCESSIVE_MESSAGE_SIZE";
        case Generic_Error::INSUFFICIENT_CAPACITY:            return stream << "::picolibrary::Generic_Error::INSUFFICIENT_CAPACITY";
        case Generic_Error::INSUFFICIENT_SOCKETS_AVAILABLE:   return stream << "::picolibrary::Generic_Error::INSUFFICIENT_SOCKETS_AVAILABLE";
        case Generic_Error::INVALID_ARGUMENT:                 return stream << "::picolibrary::Generic_Error::INVALID_ARGUMENT";
        case Generic_Error::IO_STREAM_DEGRADED:               return stream << "::picolibrary::Generic_Error::IO_STREAM_DEGRADED";
        case Generic_Error::LOGIC_ERROR:                      return stream << "::picolibrary::Generic_Error::LOGIC_ERROR";
        case Generic_Error::NONRESPONSIVE_DEVICE:             return stream << "::picolibrary::Generic_Error::NONRESPONSIVE_DEVICE";
        case Generic_Error::NOT_CONNECTED:                    return stream << "::picolibrary::Generic_Error::NOT_CONNECTED";
        case Generic_Error::NO_SOCKETS_AVAILABLE:             return stream << "::picolibrary::Generic_Error::NO_SOCKETS_AVAILABLE";
        case Generic_Error::OPERATION_TIMEOUT:                return stream << "::picolibrary::Generic_Error::OPERATION_TIMEOUT";
        case Generic_Error::OUT_OF_RANGE:                     return stream << "::picolibrary::Generic_Error::OUT_OF_RANGE";
        case Generic_Error::RUNTIME_ERROR:                    return stream << "::picolibrary::Generic_Error::RUNTIME_ERROR";
        case Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT: return stream << "::picolibrary::Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT";
        case Generic_Error::WOULD_BLOCK:                      return stream << "::picolibrary::Generic_Error::WOULD_BLOCK";
        case Generic_Error::WOULD_OVERFLOW:                   return stream << "::picolibrary::Generic_Error::WOULD_OVERFLOW";
        case Generic_Error::WOULD_UNDERFLOW:                  return stream << "::picolibrary::Generic_Error::WOULD_UNDERFLOW";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "generic_error is not a valid ::picolibrary::Generic_Error"
    };
}

} // namespace picolibrary

namespace picolibrary::Testing::Automated {

/**
 * \brief Mock errors.
 *
 * \relatedalso picolibrary::Testing::Automated::Mock_Error_Category
 */
enum class Mock_Error : Error_ID {};

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Testing::Automated::Mock_Error
 *            to.
 * \param[in] error The picolibrary::Testing::Automated::Mock_Error to write to the
 *            stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Mock_Error error ) -> std::ostream &
{
    return stream << "::picolibrary::Testing::Automated::Mock_Error::" << std::dec
                  << static_cast<std::uint_fast16_t>( error );
}

/**
 * \brief Mock error category.
 */
class Mock_Error_Category : public Error_Category {
  public:
    static auto instance() -> Mock_Error_Category const &
    {
        static auto category = Mock_Error_Category{};

        return category;
    }

    Mock_Error_Category() = default;

    Mock_Error_Category( Mock_Error_Category && ) = delete;

    Mock_Error_Category( Mock_Error_Category const & ) = delete;

    ~Mock_Error_Category() noexcept = default;

    auto operator=( Mock_Error_Category && ) = delete;

    auto operator=( Mock_Error_Category const & ) = delete;

    MOCK_METHOD( char const *, name, (), ( const, noexcept, override ) );

    MOCK_METHOD( char const *, error_description, ( Error_ID ), ( const, noexcept, override ) );
};

/**
 * \brief Construct an error code from a mock error.
 *
 * \relatedalso picolibrary::Testing::Automated::Mock_Error_Category
 *
 * \param[in] error The mock error to construct the error code from.
 *
 * \return The constructed error code.
 */
inline auto make_error_code( Mock_Error error ) noexcept -> Error_Code
{
    return Error_Code{ Mock_Error_Category::instance(), to_underlying( error ) };
}

} // namespace picolibrary::Testing::Automated

namespace picolibrary {

/**
 * \brief picolibrary::Testing::Automated::Mock_Error error code enum registration.
 *
 * \relatedalso picolibrary::Testing::Automated::Mock_Error_Category
 */
template<>
struct is_error_code_enum<Testing::Automated::Mock_Error> : std::true_type {
};

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Error_Code to.
 * \param[in] error The picolibrary::Error_Code to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Error_Code const & error ) -> std::ostream &
{
    if ( &error.category() == &Testing::Automated::Mock_Error_Category::instance() ) {
        return stream << "::picolibrary::Testing::Automated::Mock_Error::" << std::dec
                      << static_cast<std::uint_fast16_t>( error.id() );
    } // if

    if ( typeid( error.category() ) == typeid( Testing::Automated::Mock_Error_Category ) ) {
        return stream << "::picolibrary::Testing::Automated::Mock_Error( " << &error.category()
                      << " )::" << std::dec << static_cast<std::uint_fast16_t>( error.id() );
    } // if

    return stream << error.category().name() << "::" << error.description();
}

} // namespace picolibrary

#endif // PICOLIBRARY_TESTING_AUTOMATED_ERROR_H
