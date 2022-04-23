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
 * \brief picolibrary::I2C::Address_Numeric unit test program.
 */

#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::I2C::Address_Numeric;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::Testing::Unit::random;

auto random_address( Address_Numeric::Unsigned_Integer min = 0b0000000, Address_Numeric::Unsigned_Integer max = 0b1111111 )
    -> Address_Numeric::Unsigned_Integer
{
    return random<Address_Numeric::Unsigned_Integer>( min, max );
}

auto random_unique_address_pair()
    -> std::pair<Address_Numeric::Unsigned_Integer, Address_Numeric::Unsigned_Integer>
{
    auto const a = random_address();
    auto const b = random_address();

    return std::pair<Address_Numeric::Unsigned_Integer, Address_Numeric::Unsigned_Integer>{
        a, b != a ? b : b ^ random_address( 1 )
    };
}

} // namespace

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_numeric = Address_Numeric{};

    EXPECT_EQ( address_numeric.as_unsigned_integer(), 0b0000000 );
}

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    auto const address = random_address();

    auto const address_numeric = Address_Numeric{ address };

    EXPECT_EQ( address_numeric.as_unsigned_integer(), address );
}

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::I2C::Address_Numeric::Unsigned_Integer ) works properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    auto const address = random_address();

    auto const address_numeric = Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address };

    EXPECT_EQ( address_numeric.as_unsigned_integer(), address );
}

/**
 * \brief Verify picolibrary::I2C::Address_Numeric::Address_Numeric(
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( constructorAddressTransmitted, worksProperly )
{
    auto const address = random<Address_Transmitted>();

    auto const address_numeric = Address_Numeric{ address };

    EXPECT_EQ( address_numeric.as_unsigned_integer(), address.as_unsigned_integer() >> 1 );
}

/**
 * \brief Verify picolibrary::I2C::operator==( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_TRUE( Address_Numeric{ lhs } == Address_Numeric{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_FALSE( Address_Numeric{ lhs } == Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator!=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_FALSE( Address_Numeric{ lhs } != Address_Numeric{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_TRUE( Address_Numeric{ lhs } != Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator<( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_address( 0b0000000 + 1 );
        auto const lhs = random_address( 0b0000000, rhs - 1 );

        EXPECT_TRUE( Address_Numeric{ lhs } < Address_Numeric{ rhs } );
    }

    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_FALSE( Address_Numeric{ lhs } < Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator>( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_address( 0b0000000 + 1 );
        auto const rhs = random_address( 0b0000000, lhs - 1 );

        EXPECT_TRUE( Address_Numeric{ lhs } > Address_Numeric{ rhs } );
    }

    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_FALSE( Address_Numeric{ lhs } > Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator<=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_TRUE( Address_Numeric{ lhs } <= Address_Numeric{ rhs } );
    }

    {
        auto const lhs = random_address( 0b0000000 + 1 );
        auto const rhs = random_address( 0b0000000, lhs - 1 );

        EXPECT_FALSE( Address_Numeric{ lhs } <= Address_Numeric{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator>=( picolibrary::I2C::Address_Numeric,
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_TRUE( Address_Numeric{ lhs } >= Address_Numeric{ rhs } );
    }

    {
        auto const rhs = random_address( 0b0000000 + 1 );
        auto const lhs = random_address( 0b0000000, rhs - 1 );

        EXPECT_FALSE( Address_Numeric{ lhs } >= Address_Numeric{ rhs } );
    }
}

/**
 * \brief Execute the picolibrary::I2C::Address_Numeric unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
