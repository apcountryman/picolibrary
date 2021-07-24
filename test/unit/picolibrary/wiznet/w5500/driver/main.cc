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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Fixed_Size_Array;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::picolibrary::Testing::Unit::WIZnet::W5500::Mock_Communication_Controller;
using ::picolibrary::WIZnet::W5500::Region;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::testing::_;
using ::testing::A;
using ::testing::Return;

using Driver = ::picolibrary::WIZnet::W5500::Driver<Mock_Controller, Mock_Device_Selector::Handle, Mock_Communication_Controller>;

inline auto convert_data_to_vector( std::uint16_t data )
{
    return std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( data ),
    };
}

template<typename T, std::size_t N>
auto random_fixed_size_array()
{
    Fixed_Size_Array<T, N> array;

    std::generate( array.begin(), array.end(), []() { return random<T>(); } );

    return array;
}

template<typename T, std::size_t N>
auto convert_fixed_size_array_to_vector( Fixed_Size_Array<T, N> const & array )
{
    return std::vector<T>( array.begin(), array.end() );
}

} // namespace

namespace picolibrary {

template<typename T, std::size_t N>
auto operator==( Fixed_Size_Array<T, N> const & lhs, std::vector<T> const & rhs )
{
    return std::equal( lhs.begin(), lhs.end(), rhs.begin(), rhs.end() );
}

} // namespace picolibrary

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
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_gar() properly handles a read
 *        error.
 */
TEST( readGAR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_gar();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_gar() works properly.
 */
TEST( readGAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 4 );

    EXPECT_CALL( w5500, read( 0x0001, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_gar();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_gar() properly handles a write
 *        error.
 */
TEST( writeGAR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_gar( random_fixed_size_array<std::uint8_t, 4>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_gar() works properly.
 */
TEST( writeGAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( w5500, write( 0x0001, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_gar( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_subr() properly handles a read
 *        error.
 */
TEST( readSUBR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_subr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_subr() works properly.
 */
TEST( readSUBR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 4 );

    EXPECT_CALL( w5500, read( 0x0005, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_subr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_subr() properly handles a write
 *        error.
 */
TEST( writeSUBR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_subr( random_fixed_size_array<std::uint8_t, 4>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_subr() works properly.
 */
TEST( writeSUBR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( w5500, write( 0x0005, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_subr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_shar() properly handles a read
 *        error.
 */
TEST( readSHAR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_shar();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_shar() works properly.
 */
TEST( readSHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 6 );

    EXPECT_CALL( w5500, read( 0x0009, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_shar();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_shar() properly handles a write
 *        error.
 */
TEST( writeSHAR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_shar( random_fixed_size_array<std::uint8_t, 6>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_shar() works properly.
 */
TEST( writeSHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random_fixed_size_array<std::uint8_t, 6>();

    EXPECT_CALL( w5500, write( 0x0009, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_shar( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sipr() properly handles a read
 *        error.
 */
TEST( readSIPR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sipr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sipr() works properly.
 */
TEST( readSIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 4 );

    EXPECT_CALL( w5500, read( 0x000F, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sipr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sipr() properly handles a write
 *        error.
 */
TEST( writeSIPR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sipr( random_fixed_size_array<std::uint8_t, 4>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sipr() works properly.
 */
TEST( writeSIPR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( w5500, write( 0x000F, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sipr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_intlevel() properly handles a
 *        read error.
 */
TEST( readINTLEVEL, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_intlevel();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_intlevel() works properly.
 */
TEST( readINTLEVEL, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( 0x0013, _ ) ).WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_intlevel();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_intlevel() properly handles a
 *        write error.
 */
TEST( writeINTLEVEL, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_intlevel( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_intlevel() works properly.
 */
TEST( writeINTLEVEL, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, write( 0x0013, convert_data_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_intlevel( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ir() properly handles a read
 *        error.
 */
TEST( readIR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_ir();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ir() works properly.
 */
TEST( readIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0015 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_ir();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ir() properly handles a write
 *        error.
 */
TEST( writeIR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_ir( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ir() works properly.
 */
TEST( writeIR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x0015, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_ir( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_imr() properly handles a read
 *        error.
 */
TEST( readIMR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_imr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_imr() works properly.
 */
TEST( readIMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0016 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_imr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_imr() properly handles a write
 *        error.
 */
TEST( writeIMR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_imr( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_imr() works properly.
 */
TEST( writeIMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x0016, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_imr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sir() properly handles a read
 *        error.
 */
TEST( readSIR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sir();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sir() works properly.
 */
TEST( readSIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0017 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sir();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_simr() properly handles a read
 *        error.
 */
TEST( readSIMR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_simr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_simr() works properly.
 */
TEST( readSIMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0018 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_simr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_simr() properly handles a write
 *        error.
 */
TEST( writeSIMR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_simr( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_simr() works properly.
 */
TEST( writeSIMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x0018, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_simr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rtr() properly handles a read
 *        error.
 */
TEST( readRTR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_rtr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rtr() works properly.
 */
TEST( readRTR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( 0x0019, _ ) ).WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_rtr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rtr() properly handles a write
 *        error.
 */
TEST( writeRTR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_rtr( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rtr() works properly.
 */
TEST( writeRTR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, write( 0x0019, convert_data_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_rtr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rcr() properly handles a read
 *        error.
 */
TEST( readRCR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_rcr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rcr() works properly.
 */
TEST( readRCR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x001B ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_rcr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rcr() properly handles a write
 *        error.
 */
TEST( writeRCR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_rcr( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rcr() works properly.
 */
TEST( writeRCR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x001B, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_rcr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ptimer() properly handles a read
 *        error.
 */
TEST( readPTIMER, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_ptimer();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ptimer() works properly.
 */
TEST( readPTIMER, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x001C ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_ptimer();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ptimer() properly handles a
 *        write error.
 */
TEST( writePTIMER, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_ptimer( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ptimer() works properly.
 */
TEST( writePTIMER, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x001C, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_ptimer( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmagic() properly handles a read
 *        error.
 */
TEST( readPMAGIC, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_pmagic();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmagic() works properly.
 */
TEST( readPMAGIC, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x001D ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_pmagic();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmagic() properly handles a
 *        write error.
 */
TEST( writePMAGIC, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_pmagic( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmagic() works properly.
 */
TEST( writePMAGIC, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x001D, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_pmagic( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phar() properly handles a read
 *        error.
 */
TEST( readPHAR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_phar();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phar() works properly.
 */
TEST( readPHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 6 );

    EXPECT_CALL( w5500, read( 0x001E, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_phar();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phar() properly handles a write
 *        error.
 */
TEST( writePHAR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_phar( random_fixed_size_array<std::uint8_t, 6>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phar() works properly.
 */
TEST( writePHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random_fixed_size_array<std::uint8_t, 6>();

    EXPECT_CALL( w5500, write( 0x001E, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_phar( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_psid() properly handles a read
 *        error.
 */
TEST( readPSID, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_psid();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_psid() works properly.
 */
TEST( readPSID, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( 0x0024, _ ) ).WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_psid();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_psid() properly handles a write
 *        error.
 */
TEST( writePSID, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_psid( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_psid() works properly.
 */
TEST( writePSID, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, write( 0x0024, convert_data_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_psid( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmru() properly handles a read
 *        error.
 */
TEST( readPMRU, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_pmru();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmru() works properly.
 */
TEST( readPMRU, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( 0x0026, _ ) ).WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_pmru();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmru() properly handles a write
 *        error.
 */
TEST( writePMRU, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_pmru( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmru() works properly.
 */
TEST( writePMRU, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, write( 0x0026, convert_data_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_pmru( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uipr() properly handles a read
 *        error.
 */
TEST( readUIPR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_uipr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uipr() works properly.
 */
TEST( readUIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random_container<std::vector<std::uint8_t>>( 4 );

    EXPECT_CALL( w5500, read( 0x0028, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_uipr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uportr() properly handles a read
 *        error.
 */
TEST( readUPORTR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_uportr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uportr() works properly.
 */
TEST( readUPORTR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( 0x002C, _ ) ).WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_uportr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phycfgr() properly handles a
 *        read error.
 */
TEST( readPHYCFGR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_phycfgr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phycfgr() works properly.
 */
TEST( readPHYCFGR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x002E ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_phycfgr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phycfgr() properly handles a
 *        write error.
 */
TEST( writePHYCFGR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_phycfgr( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phycfgr() works properly.
 */
TEST( writePHYCFGR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( 0x002E, data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_phycfgr( data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_versionr() properly handles a
 *        read error.
 */
TEST( readVERSIONR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_versionr();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_versionr() works properly.
 */
TEST( readVERSIONR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( 0x0039 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_versionr();

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_mr() properly handles a read
 *        error.
 */
TEST( readSNMR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_mr( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_mr() works properly.
 */
TEST( readSNMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0000 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_mr( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_mr() properly handles a
 *        write error.
 */
TEST( writeSNMR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_mr( random<Socket_ID>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_mr() works properly.
 */
TEST( writeSNMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x0000, data ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_mr( socket_id, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_cr() properly handles a read
 *        error.
 */
TEST( readSNCR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_cr( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_cr() works properly.
 */
TEST( readSNCR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0001 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_cr( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_cr() properly handles a
 *        write error.
 */
TEST( writeSNCR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_cr( random<Socket_ID>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_cr() works properly.
 */
TEST( writeSNCR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x0001, data ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_cr( socket_id, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_ir() properly handles a read
 *        error.
 */
TEST( readSNIR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_ir( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_ir() works properly.
 */
TEST( readSNIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0002 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_ir( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_ir() properly handles a
 *        write error.
 */
TEST( writeSNIR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_ir( random<Socket_ID>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_ir() works properly.
 */
TEST( writeSNIR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x0002, data ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_ir( socket_id, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_sr() properly handles a read
 *        error.
 */
TEST( readSNSR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_sr( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_sr() works properly.
 */
TEST( readSNSR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint8_t>();

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0003 ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_sr( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_port() properly handles a
 *        read error.
 */
TEST( readSNPORT, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_port( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_port() works properly.
 */
TEST( readSNPORT, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint16_t>();

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0004, _ ) )
        .WillOnce( Return( convert_data_to_vector( data ) ) );

    auto const result = w5500.read_sn_port( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_port() properly handles a
 *        write error.
 */
TEST( writeSNPORT, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_port( random<Socket_ID>(), random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_port() works properly.
 */
TEST( writeSNPORT, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random<std::uint16_t>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x0004, convert_data_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_port( socket_id, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dhar() properly handles a
 *        read error.
 */
TEST( readSNDHAR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_dhar( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dhar() works properly.
 */
TEST( readSNDHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random_container<std::vector<std::uint8_t>>( 6 );

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x0006, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_dhar( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dhar() properly handles a
 *        write error.
 */
TEST( writeSNDHAR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_dhar(
        random<Socket_ID>(), random_fixed_size_array<std::uint8_t, 6>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dhar() works properly.
 */
TEST( writeSNDHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random_fixed_size_array<std::uint8_t, 6>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x0006, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_dhar( socket_id, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dipr() properly handles a
 *        read error.
 */
TEST( readSNDIPR, readError )
{
    auto const w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, read( _, _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = w5500.read_sn_dipr( random<Socket_ID>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dipr() works properly.
 */
TEST( readSNDIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random_container<std::vector<std::uint8_t>>( 4 );

    EXPECT_CALL( w5500, read( socket_id, Region::REGISTERS, 0x000C, _ ) ).WillOnce( Return( data ) );

    auto const result = w5500.read_sn_dipr( socket_id );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dipr() properly handles a
 *        write error.
 */
TEST( writeSNDIPR, writeError )
{
    auto w5500 = Driver{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( w5500, write( _, _, _, A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const result = w5500.write_sn_dipr(
        random<Socket_ID>(), random_fixed_size_array<std::uint8_t, 4>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dipr() works properly.
 */
TEST( writeSNDIPR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = random<Socket_ID>();
    auto const data      = random_fixed_size_array<std::uint8_t, 4>();

    EXPECT_CALL( w5500, write( socket_id, Region::REGISTERS, 0x000C, convert_fixed_size_array_to_vector( data ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( w5500.write_sn_dipr( socket_id, data ).is_error() );
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
