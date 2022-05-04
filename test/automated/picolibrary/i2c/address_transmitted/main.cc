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
 * \brief picolibrary::I2C::Address_Transmitted automated test program.
 */

#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/i2c.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/testing/automated/random.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::I2C::Address_Numeric;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::Testing::Automated::random;

auto random_address(
    Address_Transmitted::Unsigned_Integer min = 0b0000000'0,
    Address_Transmitted::Unsigned_Integer max = 0b1111111'0 ) -> Address_Transmitted::Unsigned_Integer
{
    return static_cast<Address_Transmitted::Unsigned_Integer>(
        random<Address_Transmitted::Unsigned_Integer>( min, max ) & 0b1111111'0 );
}

auto random_unique_address_pair()
    -> std::pair<Address_Transmitted::Unsigned_Integer, Address_Transmitted::Unsigned_Integer>
{
    auto const a = random_address();
    auto const b = random_address();

    return std::pair<Address_Transmitted::Unsigned_Integer, Address_Transmitted::Unsigned_Integer>{
        a, b != a ? b : b ^ random_address( 2 )
    };
}

} // namespace

/**
 * \brief Verify picolibrary::I2C::Address_Transmitted::Address_Transmitted() works
 *        properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address_transmitted = Address_Transmitted{};

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), 0b0000000'0 );
}

/**
 * \brief Verify picolibrary::I2C::Address_Transmitted::Address_Transmitted(
 *        picolibrary::I2C::Address_Transmitted::Unsigned_Integer ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    auto const address = random_address();

    auto const address_transmitted = Address_Transmitted{ address };

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), address );
}

/**
 * \brief Verify picolibrary::I2C::Address_Transmitted::Address_Transmitted(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::I2C::Address_Transmitted::Unsigned_Integer ) works properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    auto const address = random_address();

    auto const address_transmitted = Address_Transmitted{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, address };

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), address );
}

/**
 * \brief Verify picolibrary::I2C::Address_Transmitted::Address_Transmitted(
 *        picolibrary::I2C::Address_Numeric ) works properly.
 */
TEST( constructorAddressNumeric, worksProperly )
{
    auto const address = random<Address_Numeric>();

    auto const address_transmitted = Address_Transmitted{ address };

    EXPECT_EQ( address_transmitted.as_unsigned_integer(), address.as_unsigned_integer() << 1 );
}

/**
 * \brief Verify picolibrary::I2C::operator==( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_TRUE( Address_Transmitted{ lhs } == Address_Transmitted{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_FALSE( Address_Transmitted{ lhs } == Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator!=( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = lhs;

        EXPECT_FALSE( Address_Transmitted{ lhs } != Address_Transmitted{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_address_pair();

        EXPECT_TRUE( Address_Transmitted{ lhs } != Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator<( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_address( 0b0000000 + 2 );
        auto const lhs = random_address( 0b0000000, rhs - 2 );

        EXPECT_TRUE( Address_Transmitted{ lhs } < Address_Transmitted{ rhs } );
    }

    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_FALSE( Address_Transmitted{ lhs } < Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator>( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_address( 0b0000000 + 2 );
        auto const rhs = random_address( 0b0000000, lhs - 2 );

        EXPECT_TRUE( Address_Transmitted{ lhs } > Address_Transmitted{ rhs } );
    }

    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_FALSE( Address_Transmitted{ lhs } > Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator<=( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random_address();
        auto const rhs = random_address( lhs );

        EXPECT_TRUE( Address_Transmitted{ lhs } <= Address_Transmitted{ rhs } );
    }

    {
        auto const lhs = random_address( 0b0000000 + 2 );
        auto const rhs = random_address( 0b0000000, lhs - 2 );

        EXPECT_FALSE( Address_Transmitted{ lhs } <= Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::I2C::operator>=( picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random_address();
        auto const lhs = random_address( rhs );

        EXPECT_TRUE( Address_Transmitted{ lhs } >= Address_Transmitted{ rhs } );
    }

    {
        auto const rhs = random_address( 0b0000000 + 2 );
        auto const lhs = random_address( 0b0000000, rhs - 2 );

        EXPECT_FALSE( Address_Transmitted{ lhs } >= Address_Transmitted{ rhs } );
    }
}

/**
 * \brief Execute the picolibrary::I2C::Address_Transmitted automated tests.
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
