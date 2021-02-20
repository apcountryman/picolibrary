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
 * \brief picolibrary::I2C::Device<std::uint8_t> unit test program.
 */

#include <cstdint>
#include <functional>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;

class Device :
    public ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>> {
  public:
    Device(
        std::function<Result<Void, Error_Code>()> bus_multiplexer_aligner,
        Mock_Controller &                         controller,
        Address                                   address,
        Error_Code const &                        nonresponsive ) noexcept :
        ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>{
            std::move( bus_multiplexer_aligner ),
            controller,
            address,
            nonresponsive
        }
    {
    }

    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::controller;
};

} // namespace

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t>::Device( Bus_Multiplexer_Aligner,
 *        Controller &, picolibrary::I2C::Address, picolibrary::Error_Code const & ) works
 *        properly.
 */
TEST( constructor, worksProperly )
{
    auto controller = Mock_Controller{};

    auto const address       = random<Address>();
    auto const nonresponsive = random<Mock_Error>();

    auto const device = Device{ {}, controller, address, nonresponsive };

    EXPECT_EQ( &device.controller(), &controller );
    EXPECT_EQ( device.address(), address );
    EXPECT_EQ( device.nonresponsive(), nonresponsive );
}

/**
 * \brief Execute the picolibrary::I2C::Device<std::uint8_t> unit tests.
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
