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
 * \brief picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter automated test
 *        program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/automated/adc.h"
#include "picolibrary/testing/automated/microchip/mcp3008.h"

namespace {

using ::picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter;
using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Microchip::MCP3008::Sample;
using ::picolibrary::Testing::Automated::Microchip::MCP3008::Mock_Driver;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::sample() test
 *        fixture.
 */
class sample : public TestWithParam<Input> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::sample()
 *        works properly.
 */
TEST_P( sample, worksProperly )
{
    auto       mcp3008 = Mock_Driver{};
    auto const input   = GetParam();

    auto adc = Blocking_Single_Sample_Converter{ mcp3008, input };

    auto const s = Sample{ 485 };

    EXPECT_CALL( mcp3008, sample( input ) ).WillOnce( Return( s ) );

    ASSERT_EQ( adc.sample(), s );
}

/**
 * \brief picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter::sample() test
 *        cases.
 */
Input const sample_TEST_CASES[]{
    // clang-format off

    Input::CH0,
    Input::CH1,
    Input::CH2,
    Input::CH3,
    Input::CH4,
    Input::CH5,
    Input::CH6,
    Input::CH7,

    Input::CH0_RELATIVE_TO_CH1,
    Input::CH1_RELATIVE_TO_CH0,
    Input::CH2_RELATIVE_TO_CH3,
    Input::CH3_RELATIVE_TO_CH2,
    Input::CH4_RELATIVE_TO_CH5,
    Input::CH5_RELATIVE_TO_CH4,
    Input::CH6_RELATIVE_TO_CH7,
    Input::CH7_RELATIVE_TO_CH6,

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, sample, ValuesIn( sample_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter
 *        automated tests.
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
