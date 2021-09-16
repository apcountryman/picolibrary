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
 * \brief picolibrary::IP interface.
 */

#ifndef PICOLIBRARY_IP_H
#define PICOLIBRARY_IP_H

#include <cstdint>
#include <new>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

/**
 * \brief Internet Protocol (IP) facilities.
 */
namespace picolibrary::IP {

/**
 * \brief IP version.
 */
enum class Version : std::uint_fast8_t {
    UNSPECIFIED = 0, ///< Unspecified.
    _4          = 4, ///< IPv4.
};

/**
 * \brief Version agnostic IP address.
 */
class Address {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Address() noexcept : m_version{ Version::UNSPECIFIED }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The IPv4 address to store.
     */
    constexpr Address( IPv4::Address const & address ) noexcept :
        m_version{ Version::_4 },
        m_ipv4_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address( Address && source ) noexcept : m_version{ source.m_version }
    {
        switch ( m_version ) {
            case Version::UNSPECIFIED: break;
            case Version::_4:
                new ( &m_ipv4_address ) IPv4::Address{ std::move( source.m_ipv4_address ) };
                break;
        } // switch
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address( Address const & original ) noexcept :
        m_version{ original.m_version }
    {
        switch ( m_version ) {
            case Version::UNSPECIFIED: break;
            case Version::_4:
                new ( &m_ipv4_address ) IPv4::Address{ original.m_ipv4_address };
                break;
        } // switch
    }

    /**
     * \brief Destructor.
     */
    ~Address() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Address && expression ) noexcept
    {
        if ( &expression != this ) {
            if ( m_version == expression.m_version ) {
                switch ( m_version ) {
                    case Version::UNSPECIFIED: break;
                    case Version::_4:
                        m_ipv4_address = std::move( expression.m_ipv4_address );
                        break;
                } // switch
            } else {
                switch ( expression.m_version ) {
                    case Version::UNSPECIFIED: break;
                    case Version::_4:
                        new ( &m_ipv4_address )
                            IPv4::Address{ std::move( expression.m_ipv4_address ) };
                        break;
                } // switch

                m_version = expression.m_version;
            } // else
        }     // if

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Address const & expression ) noexcept
    {
        if ( &expression != this ) {
            if ( m_version == expression.m_version ) {
                switch ( m_version ) {
                    case Version::UNSPECIFIED: break;
                    case Version::_4: m_ipv4_address = expression.m_ipv4_address; break;
                } // switch
            } else {
                switch ( expression.m_version ) {
                    case Version::UNSPECIFIED: break;
                    case Version::_4:
                        new ( &m_ipv4_address ) IPv4::Address{ expression.m_ipv4_address };
                        break;
                } // switch

                m_version = expression.m_version;
            } // else
        }     // if

        return *this;
    }

    /**
     * \brief Get the version of the stored address.
     *
     * \return The version of the stored address.
     */
    constexpr auto version() const noexcept
    {
        return m_version;
    }

    /**
     * \brief Check if the version of the stored address is unspecified.
     *
     * \return true if the version of the stored address is unspecified.
     * \return false if the version of the stored address is not unspecified.
     */
    constexpr auto is_unspecified() const noexcept
    {
        return m_version == Version::UNSPECIFIED;
    }

    /**
     * \brief Check if the stored address is an IPv4 address.
     *
     * \return true if the stored address is an IPv4 address.
     * \return false if the stored address is not an IPv4 address.
     */
    constexpr auto is_ipv4() const noexcept
    {
        return m_version == Version::_4;
    }

    /**
     * \brief Check if the stored address represents any address.
     *
     * \return true if the stored address represents any address.
     * \return false if the stored address does not represent any address.
     */
    constexpr auto is_any() const noexcept -> bool
    {
        switch ( m_version ) {
            case Version::_4: return m_ipv4_address.is_any();
            default: return true;
        } // switch
    }

    /**
     * \brief Check if the stored address is a loopback address.
     *
     * \return true if the stored address is a loopback address.
     * \return false if the stored address is not a loopback address.
     */
    constexpr auto is_loopback() const noexcept -> bool
    {
        switch ( m_version ) {
            case Version::_4: return m_ipv4_address.is_loopback();
            default: return false;
        } // switch
    }

    /**
     * \brief Check if the stored address is a multicast address.
     *
     * \return true if the stored address is a multicast address.
     * \return false if the stored address is not a multicast address.
     */
    constexpr auto is_multicast() const noexcept -> bool
    {
        switch ( m_version ) {
            case Version::_4: return m_ipv4_address.is_multicast();
            default: return false;
        } // switch
    }

    /**
     * \brief Get the stored IPv4 address.
     *
     * \warning Calling this function on an address that does not store an IPv4 address
     *          results in undefined behavior.
     *
     * \return The stored IPv4 address.
     */
    constexpr auto const & ipv4() const noexcept
    {
        return m_ipv4_address;
    }

  private:
    /**
     * \brief The version of the stored address.
     */
    Version m_version{};

    union {
        /**
         * \brief The stored unspecified version address.
         */
        std::uint_fast8_t m_unspecified_address{};

        /**
         * \brief The stored IPv4 address.
         */
        IPv4::Address m_ipv4_address;
    };
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    if ( lhs.version() == rhs.version() ) {
        switch ( lhs.version() ) {
            case Version::UNSPECIFIED: return true;
            case Version::_4: return lhs.ipv4() == rhs.ipv4();
        } // switch
    }     // if

    return false;
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address const & lhs, Address const & rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    if ( lhs.version() == rhs.version() ) {
        switch ( lhs.version() ) {
            case Version::UNSPECIFIED: return false;
            case Version::_4: return lhs.ipv4() < rhs.ipv4();
        } // switch
    }     // if

    return lhs.version() < rhs.version();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address const & lhs, Address const & rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address const & lhs, Address const & rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Address
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address const & lhs, Address const & rhs ) noexcept
{
    return not( lhs < rhs );
}

/**
 * \brief Protocol.
 */
enum class Protocol : std::uint8_t {
    TCP = 6, ///< TCP.
};

} // namespace picolibrary::IP

namespace picolibrary {

/**
 * \brief picolibrary::IP::Address output formatter.
 *
 * picolibrary::IP::Address only supports the default format specification ("{}").
 */
template<>
class Output_Formatter<IP::Address> {
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
     * \brief Parse the format specification for the picolibrary::IP::Address to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::IP::Address to be
     *            formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the picolibrary::IP::Address to the stream.
     *
     * \param[in] stream The stream to write the picolibrary::IP::Address to.
     * \param[in] address The picolibrary::IP::Address to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IP::Address const & address ) noexcept -> Result<Void, Error_Code>
    {
        if ( address.is_any() ) {
            return stream.put( "ANY" );
        } // if

        switch ( address.version() ) {
            case IP::Version::_4: return stream.print( "{}", address.ipv4() );
            default: return {};
        } // switch
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_IP_H
