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
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner automated test
 *        program.
 */

#include <bitset>
#include <cstdint>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/automated/texas_instruments/tca9548a.h"
#include "picolibrary/texas_instruments/tca9548a.h"

namespace {

using ::picolibrary::Testing::Automated::Texas_Instruments::TCA9548A::Mock_Caching_Driver;
using ::picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        TCA9548A has already been configured to enable communication with the device
 *        test fixture.
 */
class callOperatorTCA9548AAlreadyConfigured : public TestWithParam<std::uint8_t> {
};

/**
 * \brief Verify
 *        picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        works properly when the TCA9548A has already been configured to enable
 *        communication with the device.
 */
TEST_P( callOperatorTCA9548AAlreadyConfigured, worksProperly )
{
    auto tca9548a = Mock_Caching_Driver{};

    auto const align_bus_multiplexer = Bus_Multiplexer_Aligner{ tca9548a, 0b1100'0000, 0b0011'0000 };

    auto const control = GetParam();

    EXPECT_CALL( tca9548a, control() ).WillOnce( Return( control ) );
    EXPECT_CALL( tca9548a, write_control( _ ) ).Times( 0 );

    align_bus_multiplexer();
}

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        TCA9548A has already been configured to enable communication with the device
 *        test cases.
 */
std::uint8_t callOperatorTCA9548AAlreadyConfigured_TEST_CASES[]{
    // clang-format off

    0b1100'0000,
    0b1100'0001,
    0b1100'0010,
    0b1100'0011,
    0b1100'0100,
    0b1100'0101,
    0b1100'0110,
    0b1100'0111,
    0b1100'1000,
    0b1100'1001,
    0b1100'1010,
    0b1100'1011,
    0b1100'1100,
    0b1100'1101,
    0b1100'1110,
    0b1100'1111,

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, callOperatorTCA9548AAlreadyConfigured, ValuesIn( callOperatorTCA9548AAlreadyConfigured_TEST_CASES ) );

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        TCA9548A has not already been configured to enable communication with the device
 *        test fixture.
 */
struct callOperatorTCA9548ANotAlreadyConfigured_Test_Case {
    /**
     * \brief The cached Control register value.
     */
    std::uint8_t control;

    /**
     * \brief The data to write to the Control register.
     */
    std::uint8_t data;
};

auto operator<<( std::ostream & stream, callOperatorTCA9548ANotAlreadyConfigured_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".control = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.control }
                  << ", "
                  << ".data = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.data }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        TCA9548A has not already been configured to enable communication with the device
 *        test fixture.
 */
class callOperatorTCA9548ANotAlreadyConfigured :
    public TestWithParam<callOperatorTCA9548ANotAlreadyConfigured_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        works properly when the TCA9548A has not already been configured to enable
 *        communication with the device.
 */
TEST_P( callOperatorTCA9548ANotAlreadyConfigured, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto tca9548a = Mock_Caching_Driver{};

    auto const align_bus_multiplexer = Bus_Multiplexer_Aligner{ tca9548a, 0b11'00'0000, 0b00'11'0000 };

    EXPECT_CALL( tca9548a, control() ).WillOnce( Return( test_case.control ) );
    EXPECT_CALL( tca9548a, write_control( test_case.data ) );

    align_bus_multiplexer();
}

/**
 * \brief picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner::operator()()
 *        TCA9548A has not already been configured to enable communication with the device
 *        test cases.
 */
callOperatorTCA9548ANotAlreadyConfigured_Test_Case callOperatorTCA9548ANotAlreadyConfigured_TEST_CASES[]{
    // clang-format off

    { 0b0000'1101, 0b1100'1101 },
    { 0b0001'0000, 0b1100'0000 },
    { 0b0010'1111, 0b1100'1111 },
    { 0b0011'0100, 0b1100'0100 },
    { 0b0100'0110, 0b1100'0110 },
    { 0b0101'0101, 0b1100'0101 },
    { 0b0110'1111, 0b1100'1111 },
    { 0b0111'0001, 0b1100'0001 },
    { 0b1000'1000, 0b1100'1000 },
    { 0b1001'1110, 0b1100'1110 },
    { 0b1010'0111, 0b1100'0111 },
    { 0b1011'1001, 0b1100'1001 },
    { 0b1101'1001, 0b1100'1001 },
    { 0b1110'1111, 0b1100'1111 },
    { 0b1111'1011, 0b1100'1011 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, callOperatorTCA9548ANotAlreadyConfigured, ValuesIn( callOperatorTCA9548ANotAlreadyConfigured_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Texas_Instruments::TCA9548A::Bus_Multiplexer_Aligner
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
