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
 * \brief picolibrary::WIZnet::W5500::Driver unit test program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Communication_Controller;
using ::testing::_;
using ::testing::A;
using ::testing::Return;

using Driver = ::picolibrary::WIZnet::W5500::Driver<Mock_Controller, Mock_Device_Selector::Handle, Mock_Communication_Controller>;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_mr() properly handles a read
 *        error.
 */
TEST( readMR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_mr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_mr() works properly.
 */
TEST( readMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0000 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_mr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_mr() properly handles a write
 *        error.
 */
TEST( writeMR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_mr( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_mr() works properly.
 */
TEST( writeMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x0000, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_mr( data ).is_error() );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Driver unit tests.
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
