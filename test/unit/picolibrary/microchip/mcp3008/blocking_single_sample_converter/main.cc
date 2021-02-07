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
 * \brief picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter unit test
 *        program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/microchip/mcp3008.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter;
using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Microchip::MCP3008::Sample;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::Microchip::MCP3008::Mock_Driver;
using ::testing::_;
using ::testing::Return;

} // namespace

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::initialize()
 *        works properly.
 */
TEST( initialize, worksProperly )
{
    auto mcp3008 = Mock_Driver{};

    auto adc = Blocking_Single_Sample_Converter{ mcp3008, random<Input>() };

    EXPECT_FALSE( adc.initialize().is_error() );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::sample()
 *        properly handles a sampling error.
 */
TEST( sample, samplingError )
{
    auto mcp3008 = Mock_Driver{};

    auto adc = Blocking_Single_Sample_Converter{ mcp3008, random<Input>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( mcp3008, sample( _ ) ).WillOnce( Return( error ) );

    auto const result = adc.sample();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::sample()
 *        works properly.
 */
TEST( sample, worksProperly )
{
    auto mcp3008 = Mock_Driver{};

    auto const input = random<Input>();

    auto adc = Blocking_Single_Sample_Converter{ mcp3008, input };

    auto const sample = random<Sample::Value>( Sample::MIN, Sample::MAX );

    EXPECT_CALL( mcp3008, sample( input ) ).WillOnce( Return( sample ) );

    auto const result = adc.sample();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), sample );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter
 *        unit tests.
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
