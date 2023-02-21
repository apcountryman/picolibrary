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
 * \brief picolibrary::IP interface.
 */

#ifndef PICOLIBRARY_IP_H
#define PICOLIBRARY_IP_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/format.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/rom.h"
#include "picolibrary/stream.h"

/**
 * \brief IP facilities.
 */
namespace picolibrary::IP {

/**
 * \brief Version.
 */
enum class Version : std::uint_fast8_t {
    UNSPECIFIED, ///< Unspecified.
    _4,          ///< IPv4.
};

/**
 * \brief Version agnostic address.
 */
class Address {
  public:
    static_assert( std::is_trivially_destructible_v<IPv4::Address> );

    /**
     * \brief Constructor.
     */
    // NOLINTNEXTLINE(modernize-use-equals-default)
    constexpr Address() noexcept
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
    constexpr auto operator=( Address && expression ) noexcept -> Address &
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
    constexpr auto operator=( Address const & expression ) noexcept -> Address &
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
    constexpr auto version() const noexcept -> Version
    {
        return m_version;
    }

    /**
     * \brief Check if the version of the stored address is unspecified.
     *
     * \return true if the version of the stored address is unspecified.
     * \return false if the version of the stored address is not unspecified.
     */
    constexpr auto is_unspecified() const noexcept -> bool
    {
        return m_version == Version::UNSPECIFIED;
    }

    /**
     * \brief Check if the stored address is an IPv4 address.
     *
     * \return true if the stored address is an IPv4 address.
     * \return false if the stored address is not an IPv4 address.
     */
    constexpr auto is_ipv4() const noexcept -> bool
    {
        return m_version == Version::_4;
    }

    /**
     * \brief Check if the stored address is an address that is used to represent any
     *        address.
     *
     * \return true if the stored address is an address that is used to represent any
     *         address.
     * \return false if the stored address is not an address that is used to represent any
     *         address.
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
    constexpr auto ipv4() const noexcept -> IPv4::Address const &
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
constexpr auto operator!=( Address const & lhs, Address const & rhs ) noexcept -> bool
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
constexpr auto operator>( Address const & lhs, Address const & rhs ) noexcept -> bool
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
constexpr auto operator<=( Address const & lhs, Address const & rhs ) noexcept -> bool
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
constexpr auto operator>=( Address const & lhs, Address const & rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

/**
 * \brief Protocol port.
 */
class Port {
  public:
    /**
     * \brief Port unsigned integer representation.
     */
    using Unsigned_Integer = std::uint16_t;

    /**
     * \brief Get the minimum valid port.
     *
     * \return The minimum valid port.
     */
    static constexpr auto min() noexcept -> Port
    {
        return Port{ std::numeric_limits<Unsigned_Integer>::min() };
    }

    /**
     * \brief Get the maximum valid port.
     *
     * \return The maximum valid port.
     */
    static constexpr auto max() noexcept -> Port
    {
        return Port{ std::numeric_limits<Unsigned_Integer>::max() };
    }

    /**
     * \brief Get the port that is used to represent any port (0).
     *
     * \return The port that is used to represent any port.
     */
    static constexpr auto any() noexcept -> Port
    {
        return Port{ 0 };
    }

    /**
     * \brief Constructor.
     */
    constexpr Port() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] port The port.
     */
    constexpr Port( Unsigned_Integer port ) noexcept : m_port{ port }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Port( Port && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Port( Port const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Port() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Port && expression ) noexcept -> Port & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Port const & expression ) noexcept -> Port & = default;

    /**
     * \brief Check if the port is the port that is used to represent any port (0).
     *
     * \return true if the port is the port that is used to represent any port.
     * \return false if the port is not the port that is used to represent any port.
     */
    constexpr auto is_any() const noexcept -> bool
    {
        return m_port == any().as_unsigned_integer();
    }

    /**
     * \brief Get the port in its unsigned integer representation.
     *
     * \return The port in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        return m_port;
    }

  private:
    /**
     * \brief The port in its unsigned integer representation.
     */
    Unsigned_Integer m_port{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Port lhs, Port rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Port lhs, Port rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Port lhs, Port rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Port lhs, Port rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Port lhs, Port rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Port
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Port lhs, Port rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

/**
 * \brief Protocol endpoint.
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
     * \param[in] port The endpoint's port.
     */
    constexpr Endpoint( Port port ) noexcept : m_port{ std::move( port ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The endpoint's address.
     * \param[in] port The endpoint's port.
     */
    constexpr Endpoint( Address address, Port port ) noexcept :
        m_address{ std::move( address ) },
        m_port{ std::move( port ) }
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
    constexpr auto address() const noexcept -> Address const &
    {
        return m_address;
    }

    /**
     * \brief Get the endpoint's port.
     *
     * \return The endpoint's port.
     */
    constexpr auto port() const noexcept -> Port
    {
        return m_port;
    }

  private:
    /**
     * \brief The endpoint's address.
     */
    Address m_address{};

    /**
     * \brief The endpoint's port.
     */
    Port m_port{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return lhs.address() == rhs.address() and lhs.port() == rhs.port();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return lhs.address() < rhs.address()
           or ( lhs.address() == rhs.address() and lhs.port() < rhs.port() );
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::IP::Endpoint
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Endpoint const & lhs, Endpoint const & rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

} // namespace picolibrary::IP

namespace picolibrary {

/**
 * \brief picolibrary::IP::Address output formatter.
 */
template<>
class Output_Formatter<IP::Address> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted picolibrary::IP::Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Address to.
     * \param[in] address The picolibrary::IP::Address to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IP::Address const & address ) const noexcept
        -> Result<std::size_t>
    {
        if ( address.is_any() ) {
            return stream.print( PICOLIBRARY_ROM_STRING( "ANY" ) );
        } // if

        switch ( address.version() ) {
            case IP::Version::_4: return stream.print( address.ipv4() );
            default: return std::size_t{ 0 };
        } // switch
    }

    /**
     * \brief Write the formatted picolibrary::IP::Address to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Address to.
     * \param[in] address The picolibrary::IP::Address to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, IP::Address const & address ) const noexcept
        -> std::size_t
    {
        if ( address.is_any() ) {
            return stream.print( PICOLIBRARY_ROM_STRING( "ANY" ) );
        } // if

        switch ( address.version() ) {
            case IP::Version::_4: return stream.print( address.ipv4() );
            default: return std::size_t{ 0 };
        } // switch
    }
};

/**
 * \brief picolibrary::IP::Port output formatter.
 */
template<>
class Output_Formatter<IP::Port> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted picolibrary::IP::Port to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Port to.
     * \param[in] port The picolibrary::IP::Port to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IP::Port port ) const noexcept -> Result<std::size_t>
    {
        return stream.print( Format::Dec{ port.as_unsigned_integer() } );
    }

    /**
     * \brief Write the formatted picolibrary::IP::Port to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Port to.
     * \param[in] port The picolibrary::IP::Port to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, IP::Port port ) const noexcept -> std::size_t
    {
        return stream.print( Format::Dec{ port.as_unsigned_integer() } );
    }
};

/**
 * \brief picolibrary::IP::Endpoint output formatter.
 */
template<>
class Output_Formatter<IP::Endpoint> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted picolibrary::IP::Endpoint to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Endpoint to.
     * \param[in] endpoint The picolibrary::IP::Endpoint to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, IP::Endpoint const & endpoint ) const noexcept
        -> Result<std::size_t>
    {
        return stream.print( endpoint.address(), ':', endpoint.port() );
    }

    /**
     * \brief Write the formatted picolibrary::IP::Endpoint to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::IP::Endpoint to.
     * \param[in] endpoint The picolibrary::IP::Endpoint to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, IP::Endpoint const & endpoint ) const noexcept
        -> std::size_t
    {
        return stream.print( endpoint.address(), ':', endpoint.port() );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_IP_H
