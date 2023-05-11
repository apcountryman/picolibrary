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
 * \brief picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator automated test
 *        program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/crc.h"
#include "picolibrary/testing/automated/crc.h"

namespace {

using ::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator;
using ::picolibrary::Testing::Automated::CRC::Calculator_Test_Case;
using ::picolibrary::Testing::Automated::CRC::CALCULATOR_TEST_CASES_16;
using ::picolibrary::Testing::Automated::CRC::CALCULATOR_TEST_CASES_32;
using ::picolibrary::Testing::Automated::CRC::CALCULATOR_TEST_CASES_8;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator std::uint8_t
 *        test fixture.
 */
class directNibbleIndexedLookupTableCalculator8 :
    public TestWithParam<Calculator_Test_Case<std::uint8_t>> {
};

/**
 * \brief Verify picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator works
 *        properly.
 */
TEST_P( directNibbleIndexedLookupTableCalculator8, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ test_case.calculation_parameters }.calculate(
            test_case.message.begin(), test_case.message.end() ),
        test_case.remainder );
}

INSTANTIATE_TEST_SUITE_P( testCases, directNibbleIndexedLookupTableCalculator8, ValuesIn( CALCULATOR_TEST_CASES_8 ) );

/**
 * \brief picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator std::uint16_t
 *        test fixture.
 */
class directNibbleIndexedLookupTableCalculator16 :
    public TestWithParam<Calculator_Test_Case<std::uint16_t>> {
};

/**
 * \brief Verify picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator works
 *        properly.
 */
TEST_P( directNibbleIndexedLookupTableCalculator16, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ test_case.calculation_parameters }.calculate(
            test_case.message.begin(), test_case.message.end() ),
        test_case.remainder );
}

INSTANTIATE_TEST_SUITE_P( testCases, directNibbleIndexedLookupTableCalculator16, ValuesIn( CALCULATOR_TEST_CASES_16 ) );

/**
 * \brief picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator std::uint32_t
 *        test fixture.
 */
class directNibbleIndexedLookupTableCalculator32 :
    public TestWithParam<Calculator_Test_Case<std::uint32_t>> {
};

/**
 * \brief Verify picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator works
 *        properly.
 */
TEST_P( directNibbleIndexedLookupTableCalculator32, worksProperly )
{
    auto const test_case = GetParam();

    ASSERT_EQ(
        Direct_Nibble_Indexed_Lookup_Table_Calculator{ test_case.calculation_parameters }.calculate(
            test_case.message.begin(), test_case.message.end() ),
        test_case.remainder );
}

INSTANTIATE_TEST_SUITE_P( testCases, directNibbleIndexedLookupTableCalculator32, ValuesIn( CALCULATOR_TEST_CASES_32 ) );

/**
 * \brief Execute the picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator
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
