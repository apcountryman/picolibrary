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
 * \brief picolibrary::I2C interface.
 */

#ifndef PICOLIBRARY_I2C_H
#define PICOLIBRARY_I2C_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/precondition.h"

/**
 * \brief Inter-Integrated Circuit (I2C) facilities.
 */
namespace picolibrary::I2C {

/**
 * \brief Device address, numeric format.
 */
class Address_Numeric;

/**
 * \brief Device address, transmitted format.
 */
class Address_Transmitted;

class Address_Numeric {
  public:
    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint_fast8_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept
    {
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b0000000 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept
    {
        return Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b1111111 };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address_Numeric() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre address <= picolibrary::I2C::Numeric_Address::max().as_unsigned_integer()
     */
    constexpr Address_Numeric( Unsigned_Integer address ) noexcept : m_address{ address }
    {
        expect( address <= max().as_unsigned_integer(), Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Numeric( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Numeric( Address_Transmitted address ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address_Numeric( Address_Numeric && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address_Numeric( Address_Numeric const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address_Numeric() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Address_Numeric && expression ) noexcept -> Address_Numeric & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator  =( Address_Numeric const & expression ) noexcept
        -> Address_Numeric & = default;

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        return m_address;
    }

  private:
    /**
     * \brief The address.
     */
    Unsigned_Integer m_address{};
};

class Address_Transmitted {
  public:
    /**
     * \brief Address unsigned integer representation.
     */
    using Unsigned_Integer = std::uint8_t;

    /**
     * \brief Get the minimum valid address.
     *
     * \return The minimum valid address.
     */
    static constexpr auto min() noexcept
    {
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b0000000'0 };
    }

    /**
     * \brief Get the maximum valid address.
     *
     * \return The maximum valid address.
     */
    static constexpr auto max() noexcept
    {
        return Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b1111111'0 };
    }

    /**
     * \brief Constructor.
     */
    constexpr Address_Transmitted() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     *
     * \pre not ( address & 0b1 )
     */
    constexpr Address_Transmitted( Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
        expect( not( address & 0b1 ), Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Transmitted( Bypass_Precondition_Expectation_Checks, Unsigned_Integer address ) noexcept :
        m_address{ address }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] address The address.
     */
    constexpr Address_Transmitted( Address_Numeric address ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Address_Transmitted( Address_Transmitted && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Address_Transmitted( Address_Transmitted const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Address_Transmitted() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator      =( Address_Transmitted && expression ) noexcept
        -> Address_Transmitted & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator      =( Address_Transmitted const & expression ) noexcept
        -> Address_Transmitted & = default;

    /**
     * \brief Get the address in its unsigned integer representation.
     *
     * \return The address in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept
    {
        return m_address;
    }

  private:
    /**
     * \brief The address.
     */
    Unsigned_Integer m_address{};
};

constexpr Address_Numeric::Address_Numeric( Address_Transmitted address ) noexcept :
    m_address{ static_cast<Unsigned_Integer>( address.as_unsigned_integer() >> 1 ) }
{
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address_Numeric
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address_Numeric lhs, Address_Numeric rhs ) noexcept
{
    return not( lhs < rhs );
}

constexpr Address_Transmitted::Address_Transmitted( Address_Numeric address ) noexcept :
    m_address{ static_cast<Unsigned_Integer>( address.as_unsigned_integer() << 1 ) }
{
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
constexpr auto operator==( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
constexpr auto operator!=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
constexpr auto operator<( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
constexpr auto operator>( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
constexpr auto operator<=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::I2C::Address_Transmitted
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
constexpr auto operator>=( Address_Transmitted lhs, Address_Transmitted rhs ) noexcept
{
    return not( lhs < rhs );
}

/**
 * \brief Operation.
 */
enum class Operation : std::uint8_t {
    READ  = 0b1, ///< Read.
    WRITE = 0b0, ///< Write.
};

/**
 * \brief Response.
 */
enum class Response : std::uint_fast8_t {
    ACK  = 0b0, ///< ACK.
    NACK = 0b1, ///< NACK.
};

} // namespace picolibrary::I2C

#endif // PICOLIBRARY_I2C_H
