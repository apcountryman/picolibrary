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
 * \brief picolibrary::WIZnet::W5500::Driver automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/testing/automated/spi.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Communication_Controller;
using ::picolibrary::WIZnet::W5500::Memory_Offset;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::Socket_Memory_Block;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

using Driver = ::picolibrary::WIZnet::W5500::Driver<Mock_Controller, Mock_Device_Selector::Handle, Mock_Communication_Controller>;

auto to_vector( std::uint16_t data ) -> std::vector<std::uint8_t>
{
    return { static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
             static_cast<std::uint8_t>( data ) };
}

template<std::size_t N>
auto to_vector( Array<std::uint8_t, N> const & data ) -> std::vector<std::uint8_t>
{
    return { data.begin(), data.end() };
}

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_mr() works properly.
 */
TEST( readMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0xAD };

    EXPECT_CALL( w5500, read( 0x0000 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_mr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_mr() works properly.
 */
TEST( writeMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0xA8 };

    EXPECT_CALL( w5500, write( 0x0000, data ) );

    w5500.write_mr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_gar() works properly.
 */
TEST( readGAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0xCE, 0xA9, 0xBF, 0x1D };

    EXPECT_CALL( w5500, read( 0x0001, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_gar(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_gar() works properly.
 */
TEST( writeGAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0x4A, 0x95, 0x16, 0xE0 };

    EXPECT_CALL( w5500, write( 0x0001, to_vector( data ) ) );

    w5500.write_gar( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_subr() works properly.
 */
TEST( readSUBR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0x14, 0x2A, 0x48, 0x06 };

    EXPECT_CALL( w5500, read( 0x0005, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_subr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_subr() works properly.
 */
TEST( writeSUBR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0xB5, 0x48, 0x39, 0x79 };

    EXPECT_CALL( w5500, write( 0x0005, to_vector( data ) ) );

    w5500.write_subr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_shar() works properly.
 */
TEST( readSHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 6>{ 0x49, 0xDA, 0x02, 0x99, 0x23, 0xCA };

    EXPECT_CALL( w5500, read( 0x0009, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_shar(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_shar() works properly.
 */
TEST( writeSHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = Array<std::uint8_t, 6>{ 0xC5, 0x96, 0x24, 0x2E, 0x37, 0x69 };

    EXPECT_CALL( w5500, write( 0x0009, to_vector( data ) ) );

    w5500.write_shar( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sipr() works properly.
 */
TEST( readSIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0x3C, 0xF7, 0x6B, 0xD3 };

    EXPECT_CALL( w5500, read( 0x000F, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sipr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sipr() works properly.
 */
TEST( writeSIPR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0x7B, 0x75, 0xB7, 0x72 };

    EXPECT_CALL( w5500, write( 0x000F, to_vector( data ) ) );

    w5500.write_sipr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_intlevel() works properly.
 */
TEST( readINTLEVEL, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint16_t{ 0x504A };

    EXPECT_CALL( w5500, read( 0x0013, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_intlevel(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_intlevel() works properly.
 */
TEST( writeINTLEVEL, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint16_t{ 0xB859 };

    EXPECT_CALL( w5500, write( 0x0013, to_vector( data ) ) );

    w5500.write_intlevel( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ir() works properly.
 */
TEST( readIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x38 };

    EXPECT_CALL( w5500, read( 0x0015 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_ir(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ir() works properly.
 */
TEST( writeIR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0x1C };

    EXPECT_CALL( w5500, write( 0x0015, data ) );

    w5500.write_ir( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_imr() works properly.
 */
TEST( readIMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x2D };

    EXPECT_CALL( w5500, read( 0x0016 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_imr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_imr() works properly.
 */
TEST( writeIMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0xFB };

    EXPECT_CALL( w5500, write( 0x0016, data ) );

    w5500.write_imr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sir() works properly.
 */
TEST( readSIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x4C };

    EXPECT_CALL( w5500, read( 0x0017 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sir(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_simr() works properly.
 */
TEST( readSIMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x9F };

    EXPECT_CALL( w5500, read( 0x0018 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_simr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_simr() works properly.
 */
TEST( writeSIMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0x4F };

    EXPECT_CALL( w5500, write( 0x0018, data ) );

    w5500.write_simr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rtr() works properly.
 */
TEST( readRTR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint16_t{ 0x67A7 };

    EXPECT_CALL( w5500, read( 0x0019, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_rtr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rtr() works properly.
 */
TEST( writeRTR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint16_t{ 0x2BD7 };

    EXPECT_CALL( w5500, write( 0x0019, to_vector( data ) ) );

    w5500.write_rtr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rcr() works properly.
 */
TEST( readRCR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x73 };

    EXPECT_CALL( w5500, read( 0x001B ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_rcr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_rcr() works properly.
 */
TEST( writeRCR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0xA7 };

    EXPECT_CALL( w5500, write( 0x001B, data ) );

    w5500.write_rcr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_ptimer() works properly.
 */
TEST( readPTIMER, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x84 };

    EXPECT_CALL( w5500, read( 0x001C ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_ptimer(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_ptimer() works properly.
 */
TEST( writePTIMER, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0x69 };

    EXPECT_CALL( w5500, write( 0x001C, data ) );

    w5500.write_ptimer( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmagic() works properly.
 */
TEST( readPMAGIC, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x3D };

    EXPECT_CALL( w5500, read( 0x001D ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_pmagic(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmagic() works properly.
 */
TEST( writePMAGIC, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0xE0 };

    EXPECT_CALL( w5500, write( 0x001D, data ) );

    w5500.write_pmagic( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phar() works properly.
 */
TEST( readPHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 6>{ 0x7E, 0xF9, 0x91, 0x42, 0xBD, 0x8F };

    EXPECT_CALL( w5500, read( 0x001E, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_phar(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phar() works properly.
 */
TEST( writePHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = Array<std::uint8_t, 6>{ 0x00, 0xAD, 0x6B, 0x17, 0x2D, 0x28 };

    EXPECT_CALL( w5500, write( 0x001E, to_vector( data ) ) );

    w5500.write_phar( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_psid() works properly.
 */
TEST( readPSID, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint16_t{ 0x3E96 };

    EXPECT_CALL( w5500, read( 0x0024, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_psid(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_psid() works properly.
 */
TEST( writePSID, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint16_t{ 0x9C80 };

    EXPECT_CALL( w5500, write( 0x0024, to_vector( data ) ) );

    w5500.write_psid( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_pmru() works properly.
 */
TEST( readPMRU, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint16_t{ 0x1F85 };

    EXPECT_CALL( w5500, read( 0x0026, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_pmru(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_pmru() works properly.
 */
TEST( writePMRU, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint16_t{ 0x2256 };

    EXPECT_CALL( w5500, write( 0x0026, to_vector( data ) ) );

    w5500.write_pmru( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uipr() works properly.
 */
TEST( readUIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = Array<std::uint8_t, 4>{ 0xC3, 0xCE, 0x18, 0x9B };

    EXPECT_CALL( w5500, read( 0x0028, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_uipr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_uportr() works properly.
 */
TEST( readUPORTR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint16_t{ 0x085E };

    EXPECT_CALL( w5500, read( 0x002C, _ ) ).WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_uportr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_phycfgr() works properly.
 */
TEST( readPHYCFGR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0x25 };

    EXPECT_CALL( w5500, read( 0x002E ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_phycfgr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_phycfgr() works properly.
 */
TEST( writePHYCFGR, worksProperly )
{
    auto w5500 = Driver{};

    auto const data = std::uint8_t{ 0x40 };

    EXPECT_CALL( w5500, write( 0x002E, data ) );

    w5500.write_phycfgr( data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_versionr() works properly.
 */
TEST( readVERSIONR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const data = std::uint8_t{ 0xD1 };

    EXPECT_CALL( w5500, read( 0x0039 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_versionr(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_mr() works properly.
 */
TEST( readSNMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint8_t{ 0xD1 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0000 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_mr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_mr() works properly.
 */
TEST( writeSNMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_6;
    auto const data      = std::uint8_t{ 0x0D };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0000, data ) );

    w5500.write_sn_mr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_cr() works properly.
 */
TEST( readSNCR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_7;
    auto const data      = std::uint8_t{ 0xBE };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0001 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_cr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_cr() works properly.
 */
TEST( writeSNCR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = std::uint8_t{ 0xE1 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0001, data ) );

    w5500.write_sn_cr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_ir() works properly.
 */
TEST( readSNIR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_6;
    auto const data      = std::uint8_t{ 0xDB };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0002 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_ir( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_ir() works properly.
 */
TEST( writeSNIR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint8_t{ 0x63 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0002, data ) );

    w5500.write_sn_ir( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_sr() works properly.
 */
TEST( readSNSR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_0;
    auto const data      = std::uint8_t{ 0x1D };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0003 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_sr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_port() works properly.
 */
TEST( readSNPORT, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = std::uint16_t{ 0x5FE6 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0004, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_port( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_port() works properly.
 */
TEST( writeSNPORT, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint16_t{ 0xF46A };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0004, to_vector( data ) ) );

    w5500.write_sn_port( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dhar() works properly.
 */
TEST( readSNDHAR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = Array<std::uint8_t, 6>{ 0xDD, 0x05, 0x58, 0x2C, 0xF8, 0x96 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0006, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_dhar( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dhar() works properly.
 */
TEST( writeSNDHAR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = Array<std::uint8_t, 6>{ 0x91, 0x68, 0x1C, 0xC0, 0xEC, 0xA5 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0006, to_vector( data ) ) );

    w5500.write_sn_dhar( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dipr() works properly.
 */
TEST( readSNDIPR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = Array<std::uint8_t, 4>{ 0x91, 0x9D, 0x05, 0x9C };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x000C, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_dipr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dipr() works properly.
 */
TEST( writeSNDIPR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = Array<std::uint8_t, 4>{ 0x7B, 0x2F, 0xC0, 0x89 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x000C, to_vector( data ) ) );

    w5500.write_sn_dipr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_dport() works properly.
 */
TEST( readSNDPORT, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_6;
    auto const data      = std::uint16_t{ 0x09F9 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0010, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_dport( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_dport() works properly.
 */
TEST( writeSNDPORT, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_3;
    auto const data      = std::uint16_t{ 0x4085 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0010, to_vector( data ) ) );

    w5500.write_sn_dport( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_mssr() works properly.
 */
TEST( readSNMSSR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_7;
    auto const data      = std::uint16_t{ 0x9BBE };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0012, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_mssr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_mssr() works properly.
 */
TEST( writeSNMSSR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_1;
    auto const data      = std::uint16_t{ 0xCF4E };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0012, to_vector( data ) ) );

    w5500.write_sn_mssr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_tos() works properly.
 */
TEST( readSNTOS, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint8_t{ 0x3B };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0015 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_tos( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_tos() works properly.
 */
TEST( writeSNTOS, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_1;
    auto const data      = std::uint8_t{ 0x95 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0015, data ) );

    w5500.write_sn_tos( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_ttl() works properly.
 */
TEST( readSNTTL, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint8_t{ 0x33 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0016 ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_ttl( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_ttl() works properly.
 */
TEST( writeSNTTL, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint8_t{ 0x83 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0016, data ) );

    w5500.write_sn_ttl( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_rxbuf_size() works properly.
 */
TEST( readSNRXBUFSIZE, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint8_t{ 0x32 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x001E ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_rxbuf_size( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_rxbuf_size() works properly.
 */
TEST( writeSNRXBUFSIZE, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_7;
    auto const data      = std::uint8_t{ 0xCB };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x001E, data ) );

    w5500.write_sn_rxbuf_size( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_txbuf_size() works properly.
 */
TEST( readSNTXBUFSIZE, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = std::uint8_t{ 0x62 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x001F ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_txbuf_size( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_txbuf_size() works properly.
 */
TEST( writeSNTXBUFSIZE, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint8_t{ 0xEC };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x001F, data ) );

    w5500.write_sn_txbuf_size( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_tx_fsr() works properly when
 *        the register is not stable.
 */
TEST( readSNTXFSR, worksProperlyNotStable )
{
    auto const in_sequence = InSequence{};

    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_4;
    auto const data      = std::uint16_t{ 0x782D };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( 0x8DF7 ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( data ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_tx_fsr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_tx_fsr() works properly when
 *        the register is stable.
 */
TEST( readSNTXFSR, worksProperlyStable )
{
    auto const in_sequence = InSequence{};

    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_0;
    auto const data      = std::uint16_t{ 0x3056 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( 0x7927 ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( data ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0020, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_tx_fsr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_tx_rd() works properly.
 */
TEST( readSNTXRD, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_7;
    auto const data      = std::uint16_t{ 0x088B };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0022, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_tx_rd( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_tx_wr() works properly.
 */
TEST( readSNTXWR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint16_t{ 0xC946 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0024, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_tx_wr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_tx_wr() works properly.
 */
TEST( writeSNTXWR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_7;
    auto const data      = std::uint16_t{ 0x1860 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0024, to_vector( data ) ) );

    w5500.write_sn_tx_wr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_rx_rsr() works properly when
 *        the register is not stable.
 */
TEST( readSNRXRSR, worksProperlyNotStable )
{
    auto const in_sequence = InSequence{};

    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_0;
    auto const data      = std::uint16_t{ 0x6443 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( 0xBF8B ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( data ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_rx_rsr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_rx_rsr() works properly when
 *        the register is stable.
 */
TEST( readSNRXRSR, worksProperlyStable )
{
    auto const in_sequence = InSequence{};

    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_6;
    auto const data      = std::uint16_t{ 0x924A };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( 0x14A1 ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( data ) ) );
    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0026, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_rx_rsr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_rx_rd() works properly.
 */
TEST( readSNRXRD, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_0;
    auto const data      = std::uint16_t{ 0xACBD };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x0028, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_rx_rd( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_rx_rd() works properly.
 */
TEST( writeSNRXRD, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = std::uint16_t{ 0x0065 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x0028, to_vector( data ) ) );

    w5500.write_sn_rx_rd( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_rx_wr() works properly.
 */
TEST( readSNRXWR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint16_t{ 0x78EE };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x002A, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_rx_wr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_imr() works properly.
 */
TEST( readSNIMR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_1;
    auto const data      = std::uint8_t{ 0xA5 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x002C ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_imr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_imr() works properly.
 */
TEST( writeSNIMR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = std::uint8_t{ 0x0E };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x002C, data ) );

    w5500.write_sn_imr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_frag() works properly.
 */
TEST( readSNFRAG, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_1;
    auto const data      = std::uint16_t{ 0xD646 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x002D, _ ) )
        .WillOnce( Return( to_vector( data ) ) );

    ASSERT_EQ( w5500.read_sn_frag( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_frag() works properly.
 */
TEST( writeSNFRAG, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_5;
    auto const data      = std::uint16_t{ 0xE7E0 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x002D, to_vector( data ) ) );

    w5500.write_sn_frag( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_sn_kpalvtr() works properly.
 */
TEST( readSNKPALVTR, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = std::uint8_t{ 0x53 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::REGISTERS, 0x002F ) ).WillOnce( Return( data ) );

    ASSERT_EQ( w5500.read_sn_kpalvtr( socket_id ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_sn_kpalvtr() works properly.
 */
TEST( writeSNKPALVTR, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id = Socket_ID::_2;
    auto const data      = std::uint8_t{ 0x26 };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::REGISTERS, 0x002F, data ) );

    w5500.write_sn_kpalvtr( socket_id, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::read_rx_buffer() works properly.
 */
TEST( readRXBuffer, worksProperly )
{
    auto const w5500 = Driver{};

    auto const socket_id     = Socket_ID::_7;
    auto const memory_offset = Memory_Offset{ 0x6A5A };
    auto       data          = std::vector<std::uint8_t>( 3 );
    auto const data_expected = std::vector<std::uint8_t>{ 0x30, 0xD1, 0xD4 };

    EXPECT_CALL( w5500, read( socket_id, Socket_Memory_Block::RX_BUFFER, memory_offset, _ ) )
        .WillOnce( Return( data_expected ) );

    w5500.read_rx_buffer( socket_id, memory_offset, &*data.begin(), &*data.end() );

    ASSERT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Driver::write_tx_buffer() works properly.
 */
TEST( writeTXBuffer, worksProperly )
{
    auto w5500 = Driver{};

    auto const socket_id     = Socket_ID::_5;
    auto const memory_offset = Memory_Offset{ 0xB08E };
    auto const data          = std::vector<std::uint8_t>{ 0xC7, 0xDB, 0x8D, 0xF2, 0xCD };

    EXPECT_CALL( w5500, write( socket_id, Socket_Memory_Block::TX_BUFFER, memory_offset, data ) );

    w5500.write_tx_buffer( socket_id, memory_offset, &*data.begin(), &*data.end() );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Driver automated tests.
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
