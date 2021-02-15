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
 * \brief picolibrary::I2C::Address unit test program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::I2C::Address;
using ::picolibrary::I2C::make_address;
using ::picolibrary::Testing::Unit::random;

auto random_numeric_address( std::uint_fast8_t min = Address::Numeric::MIN, std::uint_fast8_t max = Address::Numeric::MAX )
{
    return random<std::uint_fast8_t>( min, max );
}

auto random_unique_numeric_addresses()
{
    auto const a = random_numeric_address();
    auto const b = random_numeric_address();

    return std::pair<std::uint_fast8_t, std::uint_fast8_t>{ a, b != a ? b : b ^ random_numeric_address( 1 ) };
}

} // namespace

/**
 * \brief Verify picolibrary::I2C::Address::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_EQ( address.numeric(), 0 );
    EXPECT_EQ( address.transmitted(), 0 );
}

/**
 * \brief Verify picolibrary::I2C::Address::Address( picolibrary::I2C::Address::Numeric,
 *        std::uint_fast8_t ) works properly.
 */
TEST( constructorNumeric, worksProperly )
{
    auto const numeric_address = random_numeric_address();

    auto const address = Address{ Address::NUMERIC, numeric_address };

    EXPECT_EQ( address.numeric(), numeric_address );
    EXPECT_EQ( address.transmitted(), numeric_address << 1 );
}

/**
 * \brief Verify picolibrary::I2C::Address::Address(
 *        picolibrary::I2C::Address::Transmitted, std::uint8_t ) works properly.
 */
TEST( constructorTransmitted, worksProperly )
{
    auto const transmitted_address = static_cast<std::uint8_t>( random_numeric_address() << 1 );

    auto const address = Address{ Address::TRANSMITTED, transmitted_address };

    EXPECT_EQ( address.numeric(), transmitted_address >> 1 );
    EXPECT_EQ( address.transmitted(), transmitted_address );
}

/**
 * \brief Verify picolibrary::I2C::make_address( picolibrary::I2C::Address::Numeric,
 *        std::uint_fast8_t ) properly handles an invalid address.
 */
TEST( makeAddressNumeric, invalidAddress )
{
    auto const result = make_address(
        Address::NUMERIC, random<std::uint_fast8_t>( Address::Numeric::MAX + 1 ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
}

/**
 * \brief Verify picolibrary::I2C::make_address( picolibrary::I2C::Address::Numeric,
 *        std::uint_fast8_t ) works properly.
 */
TEST( makeAddressNumeric, worksProperly )
{
    auto const numeric_address = random_numeric_address();

    auto const result = make_address( Address::NUMERIC, numeric_address );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().numeric(), numeric_address );
}

/**
 * \brief Verify picolibrary::I2C::make_address( picolibrary::I2C::Address::Transmitted,
 *        std::uint8_t ) properly handles an invalid address.
 */
TEST( makeAddressTransmitted, invalidAddress )
{
    auto const result = make_address( Address::TRANSMITTED, ( random_numeric_address() << 1 ) | 0x01 );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_ARGUMENT );
}

/**
 * \brief Verify picolibrary::I2C::make_address( picolibrary::I2C::Address::Transmitted,
 *        std::uint8_t ) works properly.
 */
TEST( makeAddressTransmitted, worksProperly )
{
    auto const transmitted_address = static_cast<std::uint8_t>( random_numeric_address() << 1 );

    auto const result = make_address( Address::TRANSMITTED, transmitted_address );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value().transmitted(), transmitted_address );
}

/**
 * \brief Verify picolibrary::I2C::operator==( picolibrary::I2C::Address,
 *        picolibrary::I2C::Address ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_numeric_address();
        auto const rhs = lhs;

        EXPECT_TRUE( ( Address{ Address::NUMERIC, lhs } ) == ( Address{ Address::NUMERIC, rhs } ) );
    }

    {
        auto const [ lhs, rhs ] = random_unique_numeric_addresses();

        EXPECT_FALSE( ( Address{ Address::NUMERIC, lhs } ) == ( Address{ Address::NUMERIC, rhs } ) );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator!=( picolibrary::I2C::Address,
 *        picolibrary::I2C::Address ) works properly.
 */
TEST( inqualityOperator, worksProperly )
{
    {
        auto const lhs = random_numeric_address();
        auto const rhs = lhs;

        EXPECT_FALSE( ( Address{ Address::NUMERIC, lhs } ) != ( Address{ Address::NUMERIC, rhs } ) );
    }

    {
        auto const [ lhs, rhs ] = random_unique_numeric_addresses();

        EXPECT_TRUE( ( Address{ Address::NUMERIC, lhs } ) != ( Address{ Address::NUMERIC, rhs } ) );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator<( picolibrary::I2C::Address,
 *        picolibrary::I2C::Address ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_numeric_address( Address::Numeric::MIN + 1 );
        auto const lhs = random_numeric_address( Address::Numeric::MIN, rhs - 1 );

        EXPECT_TRUE( ( Address{ Address::NUMERIC, lhs } ) < ( Address{ Address::NUMERIC, rhs } ) );
    }

    {
        auto const rhs = random_numeric_address();
        auto const lhs = random_numeric_address( rhs );

        EXPECT_FALSE( ( Address{ Address::NUMERIC, lhs } ) < ( Address{ Address::NUMERIC, rhs } ) );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator>( picolibrary::I2C::Address,
 *        picolibrary::I2C::Address ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_numeric_address( Address::Numeric::MIN + 1 );
        auto const rhs = random_numeric_address( Address::Numeric::MIN, lhs - 1 );

        EXPECT_TRUE( ( Address{ Address::NUMERIC, lhs } ) > ( Address{ Address::NUMERIC, rhs } ) );
    }

    {
        auto const lhs = random_numeric_address();
        auto const rhs = random_numeric_address( lhs );

        EXPECT_FALSE( ( Address{ Address::NUMERIC, lhs } ) > ( Address{ Address::NUMERIC, rhs } ) );
    }
}

/**
 * \brief Execute the picolibrary::I2C::Address unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
