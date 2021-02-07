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
 * \brief picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator unit test
 *        program.
 */

#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/crc.h"
#include "picolibrary/testing/unit/crc.h"

namespace {

using ::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator;

} // namespace

/**
 * \brief Verify
 *        picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint8_t>::calculate()
 *        works properly.
 */
INSTANTIATE_TYPED_TEST_SUITE_P(
    augmentedByteLookupTable,
    CalculatorUint8Register,
    Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint8_t> );

/**
 * \brief Verify
 *        picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint16_t>::calculate()
 *        works properly.
 */
INSTANTIATE_TYPED_TEST_SUITE_P(
    augmentedByteLookupTable,
    CalculatorUint16Register,
    Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint16_t> );

/**
 * \brief Verify
 *        picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint32_t>::calculate()
 *        works properly.
 */
INSTANTIATE_TYPED_TEST_SUITE_P(
    augmentedByteLookupTable,
    CalculatorUint32Register,
    Augmented_Byte_Indexed_Lookup_Table_Calculator<std::uint32_t> );

/**
 * \brief Execute the picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator
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
