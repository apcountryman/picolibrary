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
 * \brief picolibrary::Testing::Automated::CRC interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_CRC_H
#define PICOLIBRARY_TESTING_AUTOMATED_CRC_H

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "gtest/gtest.h"
#include "picolibrary/crc.h"

/**
 * \brief CRC automated testing facilities.
 */
namespace picolibrary::Testing::Automated::CRC {
} // namespace picolibrary::Testing::Automated::CRC

/**
 * \brief Calculator implementation test fixture (std::uint8_t register).
 */
template<typename Calculator>
class calculatorImplementation8 : public ::testing::Test {
  public:
    static_assert( std::is_same_v<typename Calculator::Register, std::uint8_t> );
};

TYPED_TEST_SUITE_P( calculatorImplementation8 );

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly0 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0x00, false, false, 0x00 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xA2 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly1 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0xFF, false, false, 0x00 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x29 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly2 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0x00, true, true, 0x00 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xA1 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly3 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0xFF, true, true, 0x00 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x70 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly4 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0x00, false, false, 0xFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x5D );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly5 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0xFF, false, false, 0xFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xD6 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly6 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0x00, true, true, 0xFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x5E );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation8, worksProperly7 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint8_t>{
        0x31, 0xFF, true, true, 0xFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x8F );
}

REGISTER_TYPED_TEST_SUITE_P(
    calculatorImplementation8,
    worksProperly0,
    worksProperly1,
    worksProperly2,
    worksProperly3,
    worksProperly4,
    worksProperly5,
    worksProperly6,
    worksProperly7 );

/**
 * \brief Calculator implementation test fixture (std::uint16_t register).
 */
template<typename Calculator>
class calculatorImplementation16 : public ::testing::Test {
  public:
    static_assert( std::is_same_v<typename Calculator::Register, std::uint16_t> );
};

TYPED_TEST_SUITE_P( calculatorImplementation16 );

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly0 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0x0000, false, false, 0x0000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x31C3 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly1 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0xFFFF, false, false, 0x0000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xE5CC );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly2 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0x0000, true, true, 0x0000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x2189 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly3 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0xFFFF, true, true, 0x0000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xD1A2 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly4 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0x0000, false, false, 0xFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xCE3C );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly5 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0xFFFF, false, false, 0xFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x1A33 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly6 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0x0000, true, true, 0xFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xDE76 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation16, worksProperly7 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint16_t>{
        0x1021, 0xFFFF, true, true, 0xFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x2E5D );
}

REGISTER_TYPED_TEST_SUITE_P(
    calculatorImplementation16,
    worksProperly0,
    worksProperly1,
    worksProperly2,
    worksProperly3,
    worksProperly4,
    worksProperly5,
    worksProperly6,
    worksProperly7 );

/**
 * \brief Calculator implementation test fixture (std::uint32_t register).
 */
template<typename Calculator>
class calculatorImplementation32 : public ::testing::Test {
  public:
    static_assert( std::is_same_v<typename Calculator::Register, std::uint32_t> );
};

TYPED_TEST_SUITE_P( calculatorImplementation32 );

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly0 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0x00000000, false, false, 0x00000000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x89A1897F );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly1 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0xFFFFFFFF, false, false, 0x00000000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x373C5870 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly2 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0x00000000, true, true, 0x00000000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x2DFD2D88 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly3 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000 } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xDD7694F5 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly4 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0x00000000, false, false, 0xFFFFFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x765E7680 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly5 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0xFFFFFFFF, false, false, 0xFFFFFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xC8C3A78F );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly6 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0x00000000, true, true, 0xFFFFFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0xD202D277 );
}

/**
 * \brief Verify the calculator implementation works properly.
 */
TYPED_TEST_P( calculatorImplementation32, worksProperly7 )
{
    using Calculator = TypeParam;

    auto const calculator = Calculator{ ::picolibrary::CRC::Calculation_Parameters<std::uint32_t>{
        0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF } };

    auto const message = std::string_view{ "123456789" };

    ASSERT_EQ( calculator.calculate( message.begin(), message.end() ), 0x22896B0A );
}

REGISTER_TYPED_TEST_SUITE_P(
    calculatorImplementation32,
    worksProperly0,
    worksProperly1,
    worksProperly2,
    worksProperly3,
    worksProperly4,
    worksProperly5,
    worksProperly6,
    worksProperly7 );

#endif // PICOLIBRARY_TESTING_AUTOMATED_CRC_H
