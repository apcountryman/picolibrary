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
 * \brief picolibrary::IP::TCP interface.
 */

#ifndef PICOLIBRARY_IP_TCP_H
#define PICOLIBRARY_IP_TCP_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/format.h"
#include "picolibrary/ip.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP facilities.
 */
namespace picolibrary::IP::TCP {

/**
 * \brief Port number.
 */
using Port = std::uint16_t;

/**
 * \brief Endpoint.
 */
class Endpoint {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Endpoint() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] port The endpoint's port number.
     */
    constexpr Endpoint( Port port ) noexcept : m_address{}, m_port{ port }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The endpoint's address.
     * \param[in] port The endpoint's port number.
     */
    constexpr Endpoint( Address const & address, Port port ) noexcept :
        m_address{ address },
        m_port{ port }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Endpoint( Endpoint && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Endpoint( Endpoint const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Endpoint() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Endpoint && expression ) noexcept -> Endpoint & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Endpoint const & expression ) noexcept -> Endpoint & = default;

    /**
     * \brief Get the endpoint's address.
     *
     * \return The endpoint's address.
     */
    constexpr auto const & address() const noexcept
    {
        return m_address;
    }

    /**
     * \brief Get the endpoint's port number.
     *
     * \return The endpoint's port number.
     */
    constexpr auto port() const noexcept
    {
        return m_port;
    }

  private:
    /**
     * \brief The endpoint's address.
     */
    Address m_address{};

    /**
     * \brief The endpoint's port number.
     */
    Port m_port{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return lhs.address() == rhs.address() and lhs.port() == rhs.port();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return lhs.address() < rhs.address()
           or ( lhs.address() == rhs.address() and lhs.port() < rhs.port() );
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::IP::TCP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Endpoint const & lhs, Endpoint const & rhs ) noexcept
{
    return not( lhs < rhs );
}

} // namespace picolibrary::IP::TCP

namespace picolibrary {

/**
 * \brief picolibrary::IP::TCP::Endpoint output formatter.
 *
 * picolibrary::IP::TCP::Endpoint only supports the default format specification ("{}").
 */
template<>
class Output_Formatter<IP::TCP::Endpoint> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the picolibrary::IP::TCP::Endpoint to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::IP::TCP::Endpoint
     *            to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the picolibrary::IP::TCP::Endpoint to the stream.
     *
     * \param[in] stream The stream to write the picolibrary::IP::TCP::Endpoint to.
     * \param[in] endpoint The picolibrary::IP::TCP::Endpoint to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IP::TCP::Endpoint const & endpoint ) noexcept
        -> Result<Void, Error_Code>
    {
        return stream.print( "{}:{}", endpoint.address(), Format::Decimal{ endpoint.port() } );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_IP_TCP_H
