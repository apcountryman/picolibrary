/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_H
#define PICOLIBRARY_WIZNET_W5500_H

#include <cstdint>
#include <limits>
#include <utility>

#include "picolibrary/array.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/spi.h"
#include "picolibrary/utility.h"

/**
 * \brief WIZnet W5500 facilities.
 */
namespace picolibrary::WIZnet::W5500 {

/**
 * \brief Memory offset.
 */
using Memory_Offset = std::uint16_t;

/**
 * \brief Control byte.
 *
 * The control byte has the following fields:
 * - SPI Operation Mode Bits (OM)
 * - Read/Write Access Mode Bit (RWB)
 * - Block Select Bits (BSB)
 *
 * The BSB field can be split into the following fields when accessing socket memory:
 * - Socket Memory Block (BLOCK)
 * - Socket ID (SOCKET)
 */
struct Control_Byte {
    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto OM  = std::uint_fast8_t{ 2 }; ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ 1 }; ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ 5 }; ///< BSB.

        static constexpr auto BLOCK  = std::uint_fast8_t{ 2 }; ///< BLOCK.
        static constexpr auto SOCKET = std::uint_fast8_t{ 3 }; ///< SOCKET.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto OM  = std::uint_fast8_t{};                  ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ OM + Size::OM };   ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ RWB + Size::RWB }; ///< BSB.

        static constexpr auto BLOCK = std::uint_fast8_t{ RWB + Size::RWB }; ///< BLOCK.
        static constexpr auto SOCKET = std::uint_fast8_t{ BLOCK + Size::BLOCK }; ///< SOCKET.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto OM  = mask<std::uint8_t>( Size::OM, Bit::OM );   ///< OM.
        static constexpr auto RWB = mask<std::uint8_t>( Size::RWB, Bit::RWB ); ///< RWB.
        static constexpr auto BSB = mask<std::uint8_t>( Size::BSB, Bit::BSB ); ///< BSB.

        static constexpr auto BLOCK = mask<std::uint8_t>( Size::BLOCK, Bit::BLOCK ); ///< BLOCK.
        static constexpr auto SOCKET = mask<std::uint8_t>( Size::SOCKET, Bit::SOCKET ); ///< SOCKET.
    };

    /**
     * \brief OM.
     */
    enum OM : std::uint8_t {
        OM_VDM = 0b00 << Bit::OM, ///< Variable Length Data Mode (VDM), N bytes data length (N >= 1).
        OM_FDM_1_BYTE = 0b01 << Bit::OM, ///< Fixed Length Data Mode (FDM), 1 byte data length.
        OM_FDM_2_BYTES = 0b10 << Bit::OM, ///< Fixed Length Data Mode (FDM), 2 bytes data length.
        OM_FDM_4_BYTES = 0b11 << Bit::OM, ///< Fixed Length Data Mode (FDM), 4 bytes data length.
    };

    /**
     * \brief RWB.
     */
    enum RWB : std::uint8_t {
        RWB_READ  = 0 << Bit::RWB, ///< Read.
        RWB_WRITE = 1 << Bit::RWB, ///< Write.
    };

    /**
     * \brief BSB.
     */
    enum BSB : std::uint8_t {
        BSB_COMMON_REGISTERS = 0b000'00 << Bit::BSB, ///< Common registers.

        BSB_SOCKET_0_REGISTERS = 0b000'01 << Bit::BSB, ///< Socket 0 registers.
        BSB_SOCKET_0_TX_BUFFER = 0b000'10 << Bit::BSB, ///< Socket 0 TX buffer.
        BSB_SOCKET_0_RX_BUFFER = 0b000'11 << Bit::BSB, ///< Socket 0 RX buffer.

        BSB_SOCKET_1_REGISTERS = 0b001'01 << Bit::BSB, ///< Socket 1 registers.
        BSB_SOCKET_1_TX_BUFFER = 0b001'10 << Bit::BSB, ///< Socket 1 TX buffer.
        BSB_SOCKET_1_RX_BUFFER = 0b001'11 << Bit::BSB, ///< Socket 1 RX buffer.

        BSB_SOCKET_2_REGISTERS = 0b010'01 << Bit::BSB, ///< Socket 2 registers.
        BSB_SOCKET_2_TX_BUFFER = 0b010'10 << Bit::BSB, ///< Socket 2 TX buffer.
        BSB_SOCKET_2_RX_BUFFER = 0b010'11 << Bit::BSB, ///< Socket 2 RX buffer.

        BSB_SOCKET_3_REGISTERS = 0b011'01 << Bit::BSB, ///< Socket 3 registers.
        BSB_SOCKET_3_TX_BUFFER = 0b011'10 << Bit::BSB, ///< Socket 3 TX buffer.
        BSB_SOCKET_3_RX_BUFFER = 0b011'11 << Bit::BSB, ///< Socket 3 RX buffer.

        BSB_SOCKET_4_REGISTERS = 0b100'01 << Bit::BSB, ///< Socket 4 registers.
        BSB_SOCKET_4_TX_BUFFER = 0b100'10 << Bit::BSB, ///< Socket 4 TX buffer.
        BSB_SOCKET_4_RX_BUFFER = 0b100'11 << Bit::BSB, ///< Socket 4 RX buffer.

        BSB_SOCKET_5_REGISTERS = 0b101'01 << Bit::BSB, ///< Socket 5 registers.
        BSB_SOCKET_5_TX_BUFFER = 0b101'10 << Bit::BSB, ///< Socket 5 TX buffer.
        BSB_SOCKET_5_RX_BUFFER = 0b101'11 << Bit::BSB, ///< Socket 5 RX buffer.

        BSB_SOCKET_6_REGISTERS = 0b110'01 << Bit::BSB, ///< Socket 6 registers.
        BSB_SOCKET_6_TX_BUFFER = 0b110'10 << Bit::BSB, ///< Socket 6 TX buffer.
        BSB_SOCKET_6_RX_BUFFER = 0b110'11 << Bit::BSB, ///< Socket 6 RX buffer.

        BSB_SOCKET_7_REGISTERS = 0b111'01 << Bit::BSB, ///< Socket 7 registers.
        BSB_SOCKET_7_TX_BUFFER = 0b111'10 << Bit::BSB, ///< Socket 7 TX buffer.
        BSB_SOCKET_7_RX_BUFFER = 0b111'11 << Bit::BSB, ///< Socket 7 RX buffer.
    };

    /**
     * \brief BLOCK.
     */
    enum BLOCK : std::uint8_t {
        BLOCK_REGISTERS = 0b01 << Bit::BLOCK, ///< Registers.
        BLOCK_TX_BUFFER = 0b10 << Bit::BLOCK, ///< TX buffer.
        BLOCK_RX_BUFFER = 0b11 << Bit::BLOCK, ///< RX buffer.
    };

    /**
     * \brief SOCKET.
     */
    enum SOCKET : std::uint8_t {
        SOCKET_0 = 0b000 << Bit::SOCKET, ///< Socket 0.
        SOCKET_1 = 0b001 << Bit::SOCKET, ///< Socket 1.
        SOCKET_2 = 0b010 << Bit::SOCKET, ///< Socket 2.
        SOCKET_3 = 0b011 << Bit::SOCKET, ///< Socket 3.
        SOCKET_4 = 0b100 << Bit::SOCKET, ///< Socket 4.
        SOCKET_5 = 0b101 << Bit::SOCKET, ///< Socket 5.
        SOCKET_6 = 0b110 << Bit::SOCKET, ///< Socket 6.
        SOCKET_7 = 0b111 << Bit::SOCKET, ///< Socket 7.
    };
};

/**
 * \brief Sockets.
 */
inline constexpr auto SOCKETS = std::uint_fast8_t{ 8 };

/**
 * \brief Socket ID.
 */
enum class Socket_ID : std::uint8_t {
    _0 = Control_Byte::SOCKET_0, ///< 0.
    _1 = Control_Byte::SOCKET_1, ///< 1.
    _2 = Control_Byte::SOCKET_2, ///< 2.
    _3 = Control_Byte::SOCKET_3, ///< 3.
    _4 = Control_Byte::SOCKET_4, ///< 4.
    _5 = Control_Byte::SOCKET_5, ///< 5.
    _6 = Control_Byte::SOCKET_6, ///< 6.
    _7 = Control_Byte::SOCKET_7, ///< 7.
};

/**
 * \brief Socket memory block.
 */
enum class Socket_Memory_Block : std::uint8_t {
    REGISTERS = Control_Byte::BLOCK_REGISTERS, ///< Registers.
    TX_BUFFER = Control_Byte::BLOCK_TX_BUFFER, ///< TX buffer.
    RX_BUFFER = Control_Byte::BLOCK_RX_BUFFER, ///< RX buffer.
};

/**
 * \brief Communication controller.
 *
 * \tparam Controller The type of controller used to communicate with the W5500.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         W5500.
 * \tparam Device The type of device implementation used by the communication controller.
 *         The default device implementation should be used unless a mock device
 *         implementation is being injected to support automated testing of this
 *         communication controller.
 */
template<typename Controller, typename Device_Selector, typename Device = SPI::Device<Controller, Device_Selector>>
class Communication_Controller : public Device {
  public:
    Communication_Controller( Communication_Controller const & ) = delete;

    auto operator=( Communication_Controller const & ) = delete;

    using Device::initialize;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Communication_Controller() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the W5500.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the W5500's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            W5500.
     */
    constexpr Communication_Controller(
        Controller &                               controller,
        typename Controller::Configuration const & configuration,
        Device_Selector                            device_selector ) noexcept :
        Device{ controller, configuration, std::move( device_selector ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Communication_Controller( Communication_Controller && source ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Communication_Controller() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator           =( Communication_Controller && expression ) noexcept
        -> Communication_Controller & = default;

    /**
     * \brief Read a byte of common register memory.
     *
     * \param[in] memory_offset The offset of the memory to read.
     *
     * \return The data read from memory.
     */
    auto read( Memory_Offset memory_offset ) const noexcept -> std::uint8_t
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( memory_offset, Operation::READ );

        return this->receive();
    }

    /**
     * \brief Read a block of common register memory.
     *
     * \param[in] memory_offset The offset of the memory to read.
     * \param[out] begin The beginning of the data read from the block of memory.
     * \param[out] end The end of the data read from the block of memory.
     */
    void read( Memory_Offset memory_offset, std::uint8_t * begin, std::uint8_t * end ) const noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( memory_offset, Operation::READ );

        this->receive( begin, end );
    }

    /**
     * \brief Write to a byte of common register memory.
     *
     * \param[in] memory_offset The offset of the memory to write to.
     * \param[in] data The data to write to memory.
     */
    void write( Memory_Offset memory_offset, std::uint8_t data ) noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( memory_offset, Operation::WRITE );

        this->transmit( data );
    }

    /**
     * \brief Write to a block of common register memory.
     *
     * \param[in] memory_offset The offset of the memory to write to.
     * \param[in] begin The beginning of the block of data to write to the block of
     *            memory.
     * \param[in] end The end of the block of data to write to the block of memory.
     */
    void write( Memory_Offset memory_offset, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( memory_offset, Operation::WRITE );

        this->transmit( begin, end );
    }

    /**
     * \brief Read a byte of socket register/buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose memory will be read from.
     * \param[in] socket_memory_block The socket memory block to read from.
     * \param[in] memory_offset The offset of the memory to read.
     *
     * \return The data read from memory.
     */
    auto read( Socket_ID socket_id, Socket_Memory_Block socket_memory_block, Memory_Offset memory_offset ) const noexcept
        -> std::uint8_t
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( socket_id, socket_memory_block, memory_offset, Operation::READ );

        return this->receive();
    }

    /**
     * \brief Read a block of socket register/buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose memory will be read from.
     * \param[in] socket_memory_block The socket memory block to read from.
     * \param[in] memory_offset The offset of the memory to read.
     * \param[out] begin The beginning of the data read from the block of memory.
     * \param[out] end The end of the data read from the block of memory.
     */
    void read(
        Socket_ID           socket_id,
        Socket_Memory_Block socket_memory_block,
        Memory_Offset       memory_offset,
        std::uint8_t *      begin,
        std::uint8_t *      end ) const noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( socket_id, socket_memory_block, memory_offset, Operation::READ );

        this->receive( begin, end );
    }

    /**
     * \brief Write to a byte of socket register/buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose memory will be written to.
     * \param[in] socket_memory_block The socket memory block to write to.
     * \param[in] memory_offset The offset of the memory to write to.
     * \param[in] data The data to write to memory.
     */
    void write( Socket_ID socket_id, Socket_Memory_Block socket_memory_block, Memory_Offset memory_offset, std::uint8_t data ) noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( socket_id, socket_memory_block, memory_offset, Operation::WRITE );

        this->transmit( data );
    }

    /**
     * \brief Write to a block of socket register/buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose memory will be written to.
     * \param[in] socket_memory_block The socket memory block to write to.
     * \param[in] memory_offset The offset of the memory to write to.
     * \param[in] begin The beginning of the block of data to write to the block of
     *            memory.
     * \param[in] end The end of the block of data to write to the block of memory.
     */
    void write(
        Socket_ID            socket_id,
        Socket_Memory_Block  socket_memory_block,
        Memory_Offset        memory_offset,
        std::uint8_t const * begin,
        std::uint8_t const * end ) noexcept
    {
        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        transmit_frame_header( socket_id, socket_memory_block, memory_offset, Operation::WRITE );

        this->transmit( begin, end );
    }

  private:
    /**
     * \brief Operation.
     */
    enum class Operation : std::uint8_t {
        READ  = Control_Byte::RWB_READ,  ///< Read.
        WRITE = Control_Byte::RWB_WRITE, ///< Write.
    };

    /**
     * \brief Communication frame header.
     */
    using Frame_Header = Array<std::uint8_t, 3>;

    /**
     * \brief Transmit a common register memory communication frame header.
     *
     * \param[in] memory_offset The offset of the memory that will be accessed.
     * \param[in] operation The operation that will be performed.
     */
    void transmit_frame_header( Memory_Offset memory_offset, Operation operation ) const noexcept
    {
        auto const frame_header = Frame_Header{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            static_cast<std::uint8_t>(
                Control_Byte::BSB_COMMON_REGISTERS | to_underlying( operation ) | Control_Byte::OM_VDM ),
        };

        this->transmit( frame_header.begin(), frame_header.end() );
    }

    /**
     * \brief Transmit a socket register/buffer memory communication frame header.
     *
     * \param[in] socket_id The ID of the socket whose memory will be accessed.
     * \param[in] socket_memory_block The socket memory block that will be accessed.
     * \param[in] memory_offset The offset of the memory that will be accessed.
     * \param[in] operation The operation that will be performed.
     */
    void transmit_frame_header(
        Socket_ID           socket_id,
        Socket_Memory_Block socket_memory_block,
        Memory_Offset       memory_offset,
        Operation           operation ) const noexcept
    {
        auto const frame_header = Frame_Header{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            static_cast<std::uint8_t>(
                to_underlying( socket_id ) | to_underlying( socket_memory_block )
                | to_underlying( operation ) | Control_Byte::OM_VDM ),
        };

        this->transmit( frame_header.begin(), frame_header.end() );
    }
};

/**
 * \brief Mode Register (MR) register.
 *
 * This register has the following fields:
 * - Force ARP Request Enable (FARP)
 * - PPPoE Enable (PPPOE)
 * - Ping Block Enable (PB)
 * - WoL Enable (WOL)
 * - Reset (RST)
 */
struct MR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0000 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto RESERVED0 = std::uint_fast8_t{ 1 }; ///< RESERVED0.
        static constexpr auto FARP      = std::uint_fast8_t{ 1 }; ///< FARP.
        static constexpr auto RESERVED2 = std::uint_fast8_t{ 1 }; ///< RESERVED2.
        static constexpr auto PPPOE     = std::uint_fast8_t{ 1 }; ///< PPPOE.
        static constexpr auto PB        = std::uint_fast8_t{ 1 }; ///< PB.
        static constexpr auto WOL       = std::uint_fast8_t{ 1 }; ///< WOL.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ 1 }; ///< RESERVED6.
        static constexpr auto RST       = std::uint_fast8_t{ 1 }; ///< RST.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto RESERVED0 = std::uint_fast8_t{}; ///< RESERVED0.
        static constexpr auto FARP = std::uint_fast8_t{ RESERVED0 + Size::RESERVED0 }; ///< FARP.
        static constexpr auto RESERVED2 = std::uint_fast8_t{ FARP + Size::FARP }; ///< RESERVED2.
        static constexpr auto PPPOE = std::uint_fast8_t{ RESERVED2 + Size::RESERVED2 }; ///< PPPOE.
        static constexpr auto PB  = std::uint_fast8_t{ PPPOE + Size::PPPOE }; ///< PB.
        static constexpr auto WOL = std::uint_fast8_t{ PB + Size::PB };       ///< WOL.
        static constexpr auto RESERVED6 = std::uint_fast8_t{ WOL + Size::WOL }; ///< RESERVED6.
        static constexpr auto RST = std::uint_fast8_t{ RESERVED6 + Size::RESERVED6 }; ///< RST.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto RESERVED0 = mask<Type>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto FARP = mask<Type>( Size::FARP, Bit::FARP ); ///< FARP.
        static constexpr auto RESERVED2 = mask<Type>( Size::RESERVED2, Bit::RESERVED2 ); ///< RESERVED2.
        static constexpr auto PPPOE = mask<Type>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto PB    = mask<Type>( Size::PB, Bit::PB );       ///< PB.
        static constexpr auto WOL   = mask<Type>( Size::WOL, Bit::WOL );     ///< WOL.
        static constexpr auto RESERVED6 = mask<Type>( Size::RESERVED6, Bit::RESERVED6 ); ///< RESERVED6.
        static constexpr auto RST = mask<Type>( Size::RST, Bit::RST ); ///< RST.
    };
};

/**
 * \brief Gateway IP Address Register (GAR) register.
 */
struct GAR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 4>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0001 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief Subnet Mask Register (SUBR) register.
 */
struct SUBR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 4>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0005 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief Source Hardware Address Register (SHAR) register.
 */
struct SHAR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 6>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0009 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief Source IP Address Register (SIPR) register.
 */
struct SIPR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 4>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x000F };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief Interrupt Low Level Timer Register (INTLEVEL) register.
 */
struct INTLEVEL {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0013 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x0000 };
};

/**
 * \brief Interrupt Register (IR) register.
 *
 * This register has the following fields:
 * - WoL UDP Magic Packet Received (MP)
 * - PPPoE Connection Closed (PPPOE)
 * - Destination Unreachable (UNREACH)
 * - IP Conflict (CONFLICT)
 */
struct IR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0015 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto RESERVED0 = std::uint_fast8_t{ 4 }; ///< RESERVED0.
        static constexpr auto MP        = std::uint_fast8_t{ 1 }; ///< MP.
        static constexpr auto PPPOE     = std::uint_fast8_t{ 1 }; ///< PPPOE.
        static constexpr auto UNREACH   = std::uint_fast8_t{ 1 }; ///< UNREACH.
        static constexpr auto CONFLICT  = std::uint_fast8_t{ 1 }; ///< CONFLICT.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto RESERVED0 = std::uint_fast8_t{}; ///< RESERVED0.
        static constexpr auto MP = std::uint_fast8_t{ RESERVED0 + Size::RESERVED0 }; ///< MP.
        static constexpr auto PPPOE = std::uint_fast8_t{ MP + Size::MP }; ///< PPPOE.
        static constexpr auto UNREACH = std::uint_fast8_t{ PPPOE + Size::PPPOE }; ///< UNREACH.
        static constexpr auto CONFLICT = std::uint_fast8_t{ UNREACH + Size::UNREACH }; ///< CONFLICT.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto RESERVED0 = mask<Type>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto MP    = mask<Type>( Size::MP, Bit::MP );       ///< MP.
        static constexpr auto PPPOE = mask<Type>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto UNREACH = mask<Type>( Size::UNREACH, Bit::UNREACH ); ///< UNREACH.
        static constexpr auto CONFLICT = mask<Type>( Size::CONFLICT, Bit::CONFLICT ); ///< CONFLICT.
    };
};

/**
 * \brief Interrupt Mask Register (IMR) register.
 *
 * This register has the following fields:
 * - WoL UDP Magic Packet Received (MP)
 * - PPPoE Connection Closed (PPPOE)
 * - Destination Unreachable (UNREACH)
 * - IP Conflict (CONFLICT)
 */
struct IMR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0016 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto RESERVED0 = std::uint_fast8_t{ 4 }; ///< RESERVED0.
        static constexpr auto MP        = std::uint_fast8_t{ 1 }; ///< MP.
        static constexpr auto PPPOE     = std::uint_fast8_t{ 1 }; ///< PPPOE.
        static constexpr auto UNREACH   = std::uint_fast8_t{ 1 }; ///< UNREACH.
        static constexpr auto CONFLICT  = std::uint_fast8_t{ 1 }; ///< CONFLICT.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto RESERVED0 = std::uint_fast8_t{}; ///< RESERVED0.
        static constexpr auto MP = std::uint_fast8_t{ RESERVED0 + Size::RESERVED0 }; ///< MP.
        static constexpr auto PPPOE = std::uint_fast8_t{ MP + Size::MP }; ///< PPPOE.
        static constexpr auto UNREACH = std::uint_fast8_t{ PPPOE + Size::PPPOE }; ///< UNREACH.
        static constexpr auto CONFLICT = std::uint_fast8_t{ UNREACH + Size::UNREACH }; ///< CONFLICT.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto RESERVED0 = mask<Type>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto MP    = mask<Type>( Size::MP, Bit::MP );       ///< MP.
        static constexpr auto PPPOE = mask<Type>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto UNREACH = mask<Type>( Size::UNREACH, Bit::UNREACH ); ///< UNREACH.
        static constexpr auto CONFLICT = mask<Type>( Size::CONFLICT, Bit::CONFLICT ); ///< CONFLICT.
    };
};

/**
 * \brief Socket Interrupt Register (SIR) register.
 */
struct SIR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0017 };
};

/**
 * \brief Socket Interrupt Mask Register (SIMR) register.
 */
struct SIMR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0018 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };
};

/**
 * \brief Retry Time Value Register (RTR) register.
 */
struct RTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0019 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x07D0 };
};

/**
 * \brief Retry Count Register (RCR) register.
 */
struct RCR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001B };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x08 };
};

/**
 * \brief PPPoE LCP Request Timer Register (PTIMER) register.
 */
struct PTIMER {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001C };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x28 };
};

/**
 * \brief PPPoE LCP Magic Number Register (PMAGIC) register.
 */
struct PMAGIC {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001D };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };
};

/**
 * \brief PPPoE Server Hardware Address Register (PHAR) register.
 */
struct PHAR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 6>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001E };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief PPPoE Server Session ID Register (PSID) register.
 */
struct PSID {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0024 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x0000 };
};

/**
 * \brief PPPoE Maximum Receive Unit Register (PMRU) register.
 */
struct PMRU {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0026 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0xFFFF };
};

/**
 * \brief Unreachable IP Address Register (UIPR) register.
 */
struct UIPR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 4>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0028 };
};

/**
 * \brief Unreachable Port Register (UPORTR) register.
 */
struct UPORTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002C };
};

/**
 * \brief PHY Configuration Register (PHYCFGR) register.
 *
 * This register has the following fields:
 * - Link Status (LNK)
 * - Speed Status (SPD)
 * - Duplex Status (DPX)
 * - Operating Mode Configuration (OPMDC)
 * - Configure Operating Mode (OPMD)
 * - Reset (RST)
 */
struct PHYCFGR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002E };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0b10111000 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto LNK   = std::uint_fast8_t{ 1 }; ///< LNK.
        static constexpr auto SPD   = std::uint_fast8_t{ 1 }; ///< SPD.
        static constexpr auto DPX   = std::uint_fast8_t{ 1 }; ///< DPX.
        static constexpr auto OPMDC = std::uint_fast8_t{ 3 }; ///< OPMDC.
        static constexpr auto OPMD  = std::uint_fast8_t{ 1 }; ///< OPMD.
        static constexpr auto RST   = std::uint_fast8_t{ 1 }; ///< RST.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto LNK   = std::uint_fast8_t{};                  ///< LNK.
        static constexpr auto SPD   = std::uint_fast8_t{ LNK + Size::LNK }; ///< SPD.
        static constexpr auto DPX   = std::uint_fast8_t{ SPD + Size::SPD }; ///< DPX.
        static constexpr auto OPMDC = std::uint_fast8_t{ DPX + Size::DPX }; ///< OPMDC.
        static constexpr auto OPMD  = std::uint_fast8_t{ OPMDC + Size::OPMDC }; ///< OPMD.
        static constexpr auto RST   = std::uint_fast8_t{ OPMD + Size::OPMD };   ///< RST.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto LNK   = mask<Type>( Size::LNK, Bit::LNK );     ///< LNK.
        static constexpr auto SPD   = mask<Type>( Size::SPD, Bit::SPD );     ///< SPD.
        static constexpr auto DPX   = mask<Type>( Size::DPX, Bit::DPX );     ///< DPX.
        static constexpr auto OPMDC = mask<Type>( Size::OPMDC, Bit::OPMDC ); ///< OPMDC.
        static constexpr auto OPMD  = mask<Type>( Size::OPMD, Bit::OPMD );   ///< OPMD.
        static constexpr auto RST   = mask<Type>( Size::RST, Bit::RST );     ///< RST.
    };

    /**
     * \brief OPMDC.
     */
    enum OPMDC : Type {
        OPMDC_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED  = 0b000 << Bit::OPMDC, ///< 10BT half-duplex, auto-negotiation disabled.
        OPMDC_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED  = 0b001 << Bit::OPMDC, ///< 10BT full-duplex, auto-negotiation disabled.
        OPMDC_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED = 0b010 << Bit::OPMDC, ///< 100BT half-duplex, auto-negotiation disabled.
        OPMDC_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED = 0b011 << Bit::OPMDC, ///< 100BT full-duplex, auto-negotiation disabled.
        OPMDC_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED  = 0b100 << Bit::OPMDC, ///< 100BT half-duplex, auto-negotiation enabled.
        OPMDC_POWER_DOWN_MODE = 0b110 << Bit::OPMDC, ///< Power down mode.
        OPMDC_ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED = 0b111 << Bit::OPMDC, ///< All capable, auto-negotiation enabled.
    };

    /**
     * \brief OPMD.
     */
    enum OPMD : Type {
        OPMD_CONFIGURE_USING_HW_PINS = 0b0 << Bit::OPMD, ///< Configure using HW pins.
        OPMD_CONFIGURE_USING_OPMDC   = 0b1 << Bit::OPMD, ///< Configure using OPMDC.
    };
};

/**
 * \brief Chip Version Register (VERSIONR) register.
 */
struct VERSIONR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0039 };

    /**
     * \brief Version.
     */
    static constexpr auto VERSION = Type{ 0x04 };
};

/**
 * \brief Socket N Mode Register (SN_MR) register.
 *
 * This register has the following fields:
 * - Protocol (P)
 * - UDP Unicast Blocking Enable (UCASTB) / MACRAW IPv6 Packet Blocking Enable (MIP6B)
 * - TCP No Delayed ACK Enable (ND) / UDP Multicast IGMP Version (MC) / MACRAW Multicast
 *   Blocking Enable (MMB)
 * - UDP/MACRAW Broadcast Blocking Enable (BCASTB)
 * - UDP Multicasting Enable (MULTI) / MACRAW MAC Filter Enable (MFEN)
 */
struct SN_MR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0000 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto P      = std::uint_fast8_t{ 4 }; ///< P.
        static constexpr auto UCASTB = std::uint_fast8_t{ 1 }; ///< UCASTB.
        static constexpr auto ND     = std::uint_fast8_t{ 1 }; ///< ND.
        static constexpr auto BCASTB = std::uint_fast8_t{ 1 }; ///< BCASTB.
        static constexpr auto MULTI  = std::uint_fast8_t{ 1 }; ///< MULTI.

        static constexpr auto MIP6B = UCASTB; ///< MIP6B.

        static constexpr auto MC  = ND; ///< MC.
        static constexpr auto MMB = ND; ///< MMB.

        static constexpr auto MFEN = MULTI; ///< MFEN;
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto P      = std::uint_fast8_t{};              ///< P.
        static constexpr auto UCASTB = std::uint_fast8_t{ P + Size::P }; ///< UCASTB.
        static constexpr auto ND = std::uint_fast8_t{ UCASTB + Size::UCASTB }; ///< ND.
        static constexpr auto BCASTB = std::uint_fast8_t{ ND + Size::ND }; ///< BCASTB.
        static constexpr auto MULTI = std::uint_fast8_t{ BCASTB + Size::BCASTB }; ///< MULTI.

        static constexpr auto MIP6B = UCASTB; ///< MIP6B.

        static constexpr auto MC  = ND; ///< MC.
        static constexpr auto MMB = ND; ///< MMB.

        static constexpr auto MFEN = MULTI; ///< MFEN;
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto P      = mask<Type>( Size::P, Bit::P );           ///< P.
        static constexpr auto UCASTB = mask<Type>( Size::UCASTB, Bit::UCASTB ); ///< UCASTB.
        static constexpr auto ND     = mask<Type>( Size::ND, Bit::ND );         ///< ND.
        static constexpr auto BCASTB = mask<Type>( Size::BCASTB, Bit::BCASTB ); ///< BCASTB.
        static constexpr auto MULTI = mask<Type>( Size::MULTI, Bit::MULTI ); ///< MULTI.

        static constexpr auto MIP6B = UCASTB; ///< MIP6B.

        static constexpr auto MC  = ND; ///< MC.
        static constexpr auto MMB = ND; ///< MMB.

        static constexpr auto MFEN = MULTI; ///< MFEN;
    };

    /**
     * \brief P.
     */
    enum P : Type {
        P_CLOSED = 0b0000 << Bit::P, ///< Closed.
        P_TCP    = 0b0001 << Bit::P, ///< TCP.
        P_UDP    = 0b0010 << Bit::P, ///< UDP.
        P_MACRAW = 0b0100 << Bit::P, ///< MACRAW.
    };

    /**
     * \brief MC.
     */
    enum MC : Type {
        MC_IGMP_VERSION_2 = 0 << Bit::MC, ///< IGMP version 2.
        MC_IGMP_VERSION_1 = 1 << Bit::MC, ///< IGMP version 1.
    };
};

/**
 * \brief Socket N Command Register (SN_CR) register.
 */
struct SN_CR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0001 };

    /**
     * \brief Command.
     */
    enum COMMAND : Type {
        COMMAND_OPEN      = 0x01, ///< Open.
        COMMADN_LISTEN    = 0x02, ///< Listen.
        COMMAND_CONNECT   = 0x04, ///< Connect.
        COMMAND_DISCON    = 0x08, ///< Disconnect.
        COMMAND_CLOSE     = 0x10, ///< Close.
        COMMAND_SEND      = 0x20, ///< Send.
        COMMAND_SEND_MAC  = 0x21, ///< Send UDP datagram using a manually configured destination MAC address instead of one obtained by ARP.
        COMMAND_SEND_KEEP = 0x22, ///< Send TCP keepalive packet.
        COMMAND_RECV      = 0x40, ///< Receive.
    };
};

/**
 * \brief Socket N Interrupt Register (SN_IR) register.
 *
 * This register has the following fields:
 * - Peer Connected (CON)
 * - Peer Disconnected (DISCON)
 * - Data Received (RECV)
 * - Timeout (TIMEOUT)
 * - Send Complete (SENDOK)
 */
struct SN_IR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0002 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto CON       = std::uint_fast8_t{ 1 }; ///< CON.
        static constexpr auto DISCON    = std::uint_fast8_t{ 1 }; ///< DISCON.
        static constexpr auto RECV      = std::uint_fast8_t{ 1 }; ///< RECV.
        static constexpr auto TIMEOUT   = std::uint_fast8_t{ 1 }; ///< TIMEOUT.
        static constexpr auto SENDOK    = std::uint_fast8_t{ 1 }; ///< SENDOK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ 3 }; ///< RESERVED5.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto CON    = std::uint_fast8_t{};                  ///< CON.
        static constexpr auto DISCON = std::uint_fast8_t{ CON + Size::CON }; ///< DISCON.
        static constexpr auto RECV = std::uint_fast8_t{ DISCON + Size::DISCON }; ///< RECV.
        static constexpr auto TIMEOUT = std::uint_fast8_t{ RECV + Size::RECV }; ///< TIMEOUT.
        static constexpr auto SENDOK = std::uint_fast8_t{ TIMEOUT + Size::TIMEOUT }; ///< SENDOK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ SENDOK + Size::SENDOK }; ///< RESERVED5.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto CON    = mask<Type>( Size::CON, Bit::CON );       ///< CON.
        static constexpr auto DISCON = mask<Type>( Size::DISCON, Bit::DISCON ); ///< DISCON.
        static constexpr auto RECV   = mask<Type>( Size::RECV, Bit::RECV );     ///< RECV.
        static constexpr auto TIMEOUT = mask<Type>( Size::TIMEOUT, Bit::TIMEOUT ); ///< TIMEOUT.
        static constexpr auto SENDOK = mask<Type>( Size::SENDOK, Bit::SENDOK ); ///< SENDOK.
        static constexpr auto RESERVED5 = mask<Type>( Size::RESERVED5, Bit::RESERVED5 ); ///< RESERVED5.
    };
};

/**
 * \brief Socket N Status Register (SN_SR) register.
 */
struct SN_SR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0003 };

    /**
     * \brief Status.
     */
    enum STATUS : Type {
        STATUS_SOCK_CLOSED = 0x00, ///< Closed.
        STATUS_SOCK_INT    = 0x13, ///< Opened (TCP).
        STATUS_SOCK_LISTEN = 0x14, ///< Waiting for connection request from remote endpoint.
        STATUS_SOCK_ESTABLISHED = 0x17, ///< Established.
        STATUS_SOCK_CLOSE_WAIT = 0x1C, ///< Waiting for connection termination request from local user.
        STATUS_SOCK_UDP       = 0x22, ///< Opened (UDP).
        STATUS_SOCK_MACRAW    = 0x42, ///< Opened (MACRAW).
        STATUS_SOCK_SYNSENT   = 0x15, ///< Connection request sent, waiting for matching connection request.
        STATUS_SOCK_SYNRECV   = 0x16, ///< Connection request sent and received, waiting for request acknowledgement.
        STATUS_SOCK_FIN_WAIT  = 0x18, ///< Waiting for connection termination request, or acknowledgement of previously sent termination request, from remote endpoint.
        STATUS_SOCK_CLOSING   = 0x1A, ///< Waiting for connection termination request acknowledgement from remote endpoint.
        STATUS_SOCK_TIME_WAIT = 0x1B, ///< Waiting for enough time to pass to be sure the remote endpoint received the connection termination request acknowledgement.
        STATUS_SOCK_LAST_ACK  = 0x1D, ///< Waiting for connection termination request acknowledgement from remote endpoint.
    };
};

/**
 * \brief Socket N Source Port Register (SN_PORT) register.
 */
struct SN_PORT {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0004 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x0000 };
};

/**
 * \brief Socket N Destination Hardware Address Register (SN_DHAR) register.
 */
struct SN_DHAR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 6>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0006 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};

/**
 * \brief Socket N Destination IP Address Register (SN_DIPR) register.
 */
struct SN_DIPR {
    /**
     * \brief Register type.
     */
    using Type = Array<std::uint8_t, 4>;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x000C };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00, 0x00, 0x00, 0x00 };
};

/**
 * \brief Socket N Destination Port Register (SN_DPORT) register.
 */
struct SN_DPORT {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0010 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x0000 };
};

/**
 * \brief Socket N Maximum Segment Size Register (SN_MSSR) register.
 */
struct SN_MSSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0012 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x0000 };
};

/**
 * \brief Socket N TOS Register (SN_TOS) register.
 */
struct SN_TOS {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0015 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };
};

/**
 * \brief Socket N TTL Register (SN_TTL) register.
 */
struct SN_TTL {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0016 };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x80 };
};

/**
 * \brief Socket N RX Buffer Size Register (SN_RXBUF_SIZE) register.
 */
struct SN_RXBUF_SIZE {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001E };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x02 };

    /**
     * \brief Size.
     */
    enum SIZE : Type {
        SIZE_0KB  = 0,  ///< 0 KiB.
        SIZE_1KB  = 1,  ///< 1 KiB.
        SIZE_2KB  = 2,  ///< 2 KiB.
        SIZE_4KB  = 4,  ///< 4 KiB.
        SIZE_8KB  = 8,  ///< 8 KiB.
        SIZE_16KB = 16, ///< 16 KiB.
    };
};

/**
 * \brief Socket N TX Buffer Size Register (SN_TXBUF_SIZE) register.
 */
struct SN_TXBUF_SIZE {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x001F };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x02 };

    /**
     * \brief Size.
     */
    enum SIZE : Type {
        SIZE_0KB  = 0,  ///< 0 KiB.
        SIZE_1KB  = 1,  ///< 1 KiB.
        SIZE_2KB  = 2,  ///< 2 KiB.
        SIZE_4KB  = 4,  ///< 4 KiB.
        SIZE_8KB  = 8,  ///< 8 KiB.
        SIZE_16KB = 16, ///< 16 KiB.
    };
};

/**
 * \brief Socket N TX Buffer Free Size Register (SN_TX_FSR) register.
 */
struct SN_TX_FSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0020 };
};

/**
 * \brief Socket N TX Buffer Read Pointer Register (SN_TX_RD) register.
 */
struct SN_TX_RD {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0022 };
};

/**
 * \brief Socket N TX Buffer Write Pointer Register (SN_TX_WR) register.
 */
struct SN_TX_WR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0024 };
};

/**
 * \brief Socket N RX Buffer Received Size Register (SN_RX_RSR) register.
 */
struct SN_RX_RSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0026 };
};

/**
 * \brief Socket N RX Buffer Read Pointer Register (SN_RX_RD) register.
 */
struct SN_RX_RD {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x0028 };
};

/**
 * \brief Socket N RX Buffer Write Pointer Register (SN_RX_WR) register.
 */
struct SN_RX_WR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002A };
};

/**
 * \brief Socket N Interrupt Mask Register (SN_IMR) register.
 *
 * This register has the following fields:
 * - Peer Connected (CON)
 * - Peer Disconnected (DISCON)
 * - Data Received (RECV)
 * - Timeout (TIMEOUT)
 * - Send Complete (SENDOK)
 */
struct SN_IMR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002C };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0xFF };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto CON       = std::uint_fast8_t{ 1 }; ///< CON.
        static constexpr auto DISCON    = std::uint_fast8_t{ 1 }; ///< DISCON.
        static constexpr auto RECV      = std::uint_fast8_t{ 1 }; ///< RECV.
        static constexpr auto TIMEOUT   = std::uint_fast8_t{ 1 }; ///< TIMEOUT.
        static constexpr auto SENDOK    = std::uint_fast8_t{ 1 }; ///< SENDOK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ 3 }; ///< RESERVED5.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto CON    = std::uint_fast8_t{};                  ///< CON.
        static constexpr auto DISCON = std::uint_fast8_t{ CON + Size::CON }; ///< DISCON.
        static constexpr auto RECV = std::uint_fast8_t{ DISCON + Size::DISCON }; ///< RECV.
        static constexpr auto TIMEOUT = std::uint_fast8_t{ RECV + Size::RECV }; ///< TIMEOUT.
        static constexpr auto SENDOK = std::uint_fast8_t{ TIMEOUT + Size::TIMEOUT }; ///< SENDOK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ SENDOK + Size::SENDOK }; ///< RESERVED5.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto CON    = mask<Type>( Size::CON, Bit::CON );       ///< CON.
        static constexpr auto DISCON = mask<Type>( Size::DISCON, Bit::DISCON ); ///< DISCON.
        static constexpr auto RECV   = mask<Type>( Size::RECV, Bit::RECV );     ///< RECV.
        static constexpr auto TIMEOUT = mask<Type>( Size::TIMEOUT, Bit::TIMEOUT ); ///< TIMEOUT.
        static constexpr auto SENDOK = mask<Type>( Size::SENDOK, Bit::SENDOK ); ///< SENDOK.
        static constexpr auto RESERVED5 = mask<Type>( Size::RESERVED5, Bit::RESERVED5 ); ///< RESERVED5.
    };
};

/**
 * \brief Socket N FRAG Register (SN_FRAG) register.
 */
struct SN_FRAG {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002D };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x4000 };
};

/**
 * \brief Socket N Keep Alive Time Register (SN_KPALVTR) register.
 */
struct SN_KPALVTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register memory offset.
     */
    static constexpr auto MEMORY_OFFSET = Memory_Offset{ 0x002F };

    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = Type{ 0x00 };
};

/**
 * \brief Driver.
 *
 * \tparam Controller The type of controller used to communicate with the W5500.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         W5500.
 * \tparam Communication_Controller The type of communication controller implementation
 *         used by the driver. The default communication controller implementation should
 *         be used unless a mock communication controller implementation is being injected
 *         to support automated testing of this driver.
 */
template<typename Controller, typename Device_Selector, typename Communication_Controller = ::picolibrary::WIZnet::W5500::Communication_Controller<Controller, Device_Selector>>
class Driver : public Communication_Controller {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the W5500.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the W5500's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            W5500.
     */
    constexpr Driver( Controller & controller, typename Controller::Configuration const & configuration, Device_Selector device_selector ) noexcept
        :
        Communication_Controller{ controller, configuration, std::move( device_selector ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Driver( Driver && source ) noexcept = default;

    Driver( Driver const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Driver() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Driver && expression ) noexcept -> Driver & = default;

    auto operator=( Driver const & ) = delete;

    /**
     * \brief Read the MR register.
     *
     * \return The data read from the MR register.
     */
    auto read_mr() const noexcept -> MR::Type
    {
        return read<MR::Type>( MR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the MR register.
     *
     * \param[in] data The data to write to the MR register.
     */
    void write_mr( MR::Type data ) noexcept
    {
        write( MR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the GAR register.
     *
     * \return The data read from the GAR register.
     */
    auto read_gar() const noexcept -> GAR::Type
    {
        return read<GAR::Type>( GAR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the GAR register.
     *
     * \param[in] data The data to write to the GAR register.
     */
    void write_gar( GAR::Type const & data ) noexcept
    {
        write( GAR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the SUBR register.
     *
     * \return The data read from the SUBR register.
     */
    auto read_subr() const noexcept -> SUBR::Type
    {
        return read<SUBR::Type>( SUBR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the SUBR register.
     *
     * \param[in] data The data to write to the SUBR register.
     */
    void write_subr( SUBR::Type const & data ) noexcept
    {
        write( SUBR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the SHAR register.
     *
     * \return The data read from the SHAR register.
     */
    auto read_shar() const noexcept -> SHAR::Type
    {
        return read<SHAR::Type>( SHAR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the SHAR register.
     *
     * \param[in] data The data to write to the SHAR register.
     */
    void write_shar( SHAR::Type const & data ) noexcept
    {
        write( SHAR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the SIPR register.
     *
     * \return The data read from the SIPR register.
     */
    auto read_sipr() const noexcept -> SIPR::Type
    {
        return read<SIPR::Type>( SIPR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the SIPR register.
     *
     * \param[in] data The data to write to the SIPR register.
     */
    void write_sipr( SIPR::Type const & data ) noexcept
    {
        write( SIPR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the INTLEVEL register.
     *
     * \return The data read from the INTLEVEL register.
     */
    auto read_intlevel() const noexcept -> INTLEVEL::Type
    {
        return read<INTLEVEL::Type>( INTLEVEL::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the INTLEVEL register.
     *
     * \param[in] data The data to write to the INTLEVEL register.
     */
    void write_intlevel( INTLEVEL::Type data ) noexcept
    {
        write( INTLEVEL::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the IR register.
     *
     * \return The data read from the IR register.
     */
    auto read_ir() const noexcept -> IR::Type
    {
        return read<IR::Type>( IR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the IR register.
     *
     * \param[in] data The data to write to the IR register.
     */
    void write_ir( IR::Type data ) noexcept
    {
        write( IR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the IMR register.
     *
     * \return The data read from the IMR register.
     */
    auto read_imr() const noexcept -> IMR::Type
    {
        return read<IMR::Type>( IMR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the IMR register.
     *
     * \param[in] data The data to write to the IMR register.
     */
    void write_imr( IMR::Type data ) noexcept
    {
        write( IMR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the SIR register.
     *
     * \return The data read from the SIR register.
     */
    auto read_sir() const noexcept -> SIR::Type
    {
        return read<SIR::Type>( SIR::MEMORY_OFFSET );
    }

    /**
     * \brief Read the SIMR register.
     *
     * \return The data read from the SIMR register.
     */
    auto read_simr() const noexcept -> SIMR::Type
    {
        return read<SIMR::Type>( SIMR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the SIMR register.
     *
     * \param[in] data The data to write to the SIMR register.
     */
    void write_simr( SIMR::Type data ) noexcept
    {
        write( SIMR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the RTR register.
     *
     * \return The data read from the RTR register.
     */
    auto read_rtr() const noexcept -> RTR::Type
    {
        return read<RTR::Type>( RTR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the RTR register.
     *
     * \param[in] data The data to write to the RTR register.
     */
    void write_rtr( RTR::Type data ) noexcept
    {
        write( RTR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the RCR register.
     *
     * \return The data read from the RCR register.
     */
    auto read_rcr() const noexcept -> RCR::Type
    {
        return read<RCR::Type>( RCR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the RCR register.
     *
     * \param[in] data The data to write to the RCR register.
     */
    void write_rcr( RCR::Type data ) noexcept
    {
        write( RCR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the PTIMER register.
     *
     * \return The data read from the PTIMER register.
     */
    auto read_ptimer() const noexcept -> PTIMER::Type
    {
        return read<PTIMER::Type>( PTIMER::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PTIMER register.
     *
     * \param[in] data The data to write to the PTIMER register.
     */
    void write_ptimer( PTIMER::Type data ) noexcept
    {
        write( PTIMER::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the PMAGIC register.
     *
     * \return The data read from the PMAGIC register.
     */
    auto read_pmagic() const noexcept -> PMAGIC::Type
    {
        return read<PMAGIC::Type>( PMAGIC::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PMAGIC register.
     *
     * \param[in] data The data to write to the PMAGIC register.
     */
    void write_pmagic( PMAGIC::Type data ) noexcept
    {
        write( PMAGIC::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the PHAR register.
     *
     * \return The data read from the PHAR register.
     */
    auto read_phar() const noexcept -> PHAR::Type
    {
        return read<PHAR::Type>( PHAR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PHAR register.
     *
     * \param[in] data The data to write to the PHAR register.
     */
    void write_phar( PHAR::Type const & data ) noexcept
    {
        write( PHAR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the PSID register.
     *
     * \return The data read from the PSID register.
     */
    auto read_psid() const noexcept -> PSID::Type
    {
        return read<PSID::Type>( PSID::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PSID register.
     *
     * \param[in] data The data to write to the PSID register.
     */
    void write_psid( PSID::Type data ) noexcept
    {
        write( PSID::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the PMRU register.
     *
     * \return The data read from the PMRU register.
     */
    auto read_pmru() const noexcept -> PMRU::Type
    {
        return read<PMRU::Type>( PMRU::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PMRU register.
     *
     * \param[in] data The data to write to the PMRU register.
     */
    void write_pmru( PMRU::Type data ) noexcept
    {
        write( PMRU::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the UIPR register.
     *
     * \return The data read from the UIPR register.
     */
    auto read_uipr() const noexcept -> UIPR::Type
    {
        return read<UIPR::Type>( UIPR::MEMORY_OFFSET );
    }

    /**
     * \brief Read the UPORTR register.
     *
     * \return The data read from the UPORTR register.
     */
    auto read_uportr() const noexcept -> UPORTR::Type
    {
        return read<UPORTR::Type>( UPORTR::MEMORY_OFFSET );
    }

    /**
     * \brief Read the PHYCFGR register.
     *
     * \return The data read from the PHYCFGR register.
     */
    auto read_phycfgr() const noexcept -> PHYCFGR::Type
    {
        return read<PHYCFGR::Type>( PHYCFGR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to the PHYCFGR register.
     *
     * \param[in] data The data to write to the PHYCFGR register.
     */
    void write_phycfgr( PHYCFGR::Type data ) noexcept
    {
        write( PHYCFGR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read the VERSIONR register.
     *
     * \return The data read from the VERSIONR register.
     */
    auto read_versionr() const noexcept -> VERSIONR::Type
    {
        return read<VERSIONR::Type>( VERSIONR::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_MR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MR register will be read.
     *
     * \return The data read from the socket's SN_MR register.
     */
    auto read_sn_mr( Socket_ID socket_id ) const noexcept -> SN_MR::Type
    {
        return read<SN_MR::Type>( socket_id, SN_MR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_MR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MR register will be written to.
     * \param[in] data The data to write to the socket's SN_MR register.
     */
    void write_sn_mr( Socket_ID socket_id, SN_MR::Type data ) noexcept
    {
        write( socket_id, SN_MR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_CR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_CR register will be read.
     *
     * \return The data read from the socket's SN_CR register.
     */
    auto read_sn_cr( Socket_ID socket_id ) const noexcept -> SN_CR::Type
    {
        return read<SN_CR::Type>( socket_id, SN_CR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_CR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_CR register will be written to.
     * \param[in] data The data to write to the socket's SN_CR register.
     */
    void write_sn_cr( Socket_ID socket_id, SN_CR::Type data ) noexcept
    {
        write( socket_id, SN_CR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_IR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IR register will be read.
     *
     * \return The data read from the socket's SN_IR register.
     */
    auto read_sn_ir( Socket_ID socket_id ) const noexcept -> SN_IR::Type
    {
        return read<SN_IR::Type>( socket_id, SN_IR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_IR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IR register will be written to.
     * \param[in] data The data to write to the socket's SN_IR register.
     */
    void write_sn_ir( Socket_ID socket_id, SN_IR::Type data ) noexcept
    {
        write( socket_id, SN_IR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_SR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_SR register will be read.
     *
     * \return The data read from the socket's SN_SR register.
     */
    auto read_sn_sr( Socket_ID socket_id ) const noexcept -> SN_SR::Type
    {
        return read<SN_SR::Type>( socket_id, SN_SR::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_PORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_PORT register will be read.
     *
     * \return The data read from the socket's SN_PORT register.
     */
    auto read_sn_port( Socket_ID socket_id ) const noexcept -> SN_PORT::Type
    {
        return read<SN_PORT::Type>( socket_id, SN_PORT::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_PORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_PORT register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_PORT register.
     */
    void write_sn_port( Socket_ID socket_id, SN_PORT::Type data ) noexcept
    {
        write( socket_id, SN_PORT::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DHAR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DHAR register will be read.
     *
     * \return The data read from the socket's SN_DHAR register.
     */
    auto read_sn_dhar( Socket_ID socket_id ) const noexcept -> SN_DHAR::Type
    {
        return read<SN_DHAR::Type>( socket_id, SN_DHAR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DHAR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DHAR register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DHAR register.
     */
    void write_sn_dhar( Socket_ID socket_id, SN_DHAR::Type const & data ) noexcept
    {
        write( socket_id, SN_DHAR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DIPR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DIPR register will be read.
     *
     * \return The data read from the socket's SN_DIPR register.
     */
    auto read_sn_dipr( Socket_ID socket_id ) const noexcept -> SN_DIPR::Type
    {
        return read<SN_DIPR::Type>( socket_id, SN_DIPR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DIPR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DIPR register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DIPR register.
     */
    void write_sn_dipr( Socket_ID socket_id, SN_DIPR::Type const & data ) noexcept
    {
        write( socket_id, SN_DIPR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DPORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DPORT register will be read.
     *
     * \return The data read from the socket's SN_DPORT register.
     */
    auto read_sn_dport( Socket_ID socket_id ) const noexcept -> SN_DPORT::Type
    {
        return read<SN_DPORT::Type>( socket_id, SN_DPORT::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DPORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DPORT register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DPORT register.
     */
    void write_sn_dport( Socket_ID socket_id, SN_DPORT::Type data ) noexcept
    {
        write( socket_id, SN_DPORT::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_MSSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MSSR register will be read.
     *
     * \return The data read from the socket's SN_MSSR register.
     */
    auto read_sn_mssr( Socket_ID socket_id ) const noexcept -> SN_MSSR::Type
    {
        return read<SN_MSSR::Type>( socket_id, SN_MSSR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_MSSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MSSR register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_MSSR register.
     */
    void write_sn_mssr( Socket_ID socket_id, SN_MSSR::Type data ) noexcept
    {
        write( socket_id, SN_MSSR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TOS register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TOS register will be read.
     *
     * \return The data read from the socket's SN_TOS register.
     */
    auto read_sn_tos( Socket_ID socket_id ) const noexcept -> SN_TOS::Type
    {
        return read<SN_TOS::Type>( socket_id, SN_TOS::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TOS register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TOS register will be written to.
     * \param[in] data The data to write to the socket's SN_TOS register.
     */
    void write_sn_tos( Socket_ID socket_id, SN_TOS::Type data ) noexcept
    {
        write( socket_id, SN_TOS::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TTL register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TTL register will be read.
     *
     * \return The data read from the socket's SN_TTL register.
     */
    auto read_sn_ttl( Socket_ID socket_id ) const noexcept -> SN_TTL::Type
    {
        return read<SN_TTL::Type>( socket_id, SN_TTL::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TTL register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TTL register will be written to.
     * \param[in] data The data to write to the socket's SN_TTL register.
     */
    void write_sn_ttl( Socket_ID socket_id, SN_TTL::Type data ) noexcept
    {
        write( socket_id, SN_TTL::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RXBUF_SIZE register will be
     *            read.
     *
     * \return The data read from the socket's SN_RXBUF_SIZE register.
     */
    auto read_sn_rxbuf_size( Socket_ID socket_id ) const noexcept -> SN_RXBUF_SIZE::Type
    {
        return read<SN_RXBUF_SIZE::Type>( socket_id, SN_RXBUF_SIZE::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_RXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RXBUF_SIZE register will be
     *            written to.
     * \param[in] data The data to write to the socket's SN_RXBUF_SIZE register.
     */
    void write_sn_rxbuf_size( Socket_ID socket_id, SN_RXBUF_SIZE::Type data ) noexcept
    {
        write( socket_id, SN_RXBUF_SIZE::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TXBUF_SIZE register will be
     *            read.
     *
     * \return The data read from the socket's SN_TXBUF_SIZE register.
     */
    auto read_sn_txbuf_size( Socket_ID socket_id ) const noexcept -> SN_TXBUF_SIZE::Type
    {
        return read<SN_TXBUF_SIZE::Type>( socket_id, SN_TXBUF_SIZE::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TXBUF_SIZE register will be
     *            written to.
     * \param[in] data The data to write to the socket's SN_TXBUF_SIZE register.
     */
    void write_sn_txbuf_size( Socket_ID socket_id, SN_TXBUF_SIZE::Type data ) noexcept
    {
        write( socket_id, SN_TXBUF_SIZE::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TX_FSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_FSR register will be read.
     *
     * \return The data read from the socket's SN_TX_FSR register.
     */
    auto read_sn_tx_fsr( Socket_ID socket_id ) const noexcept -> SN_TX_FSR::Type
    {
        return read_non_atomic<SN_TX_FSR::Type>( socket_id, SN_TX_FSR::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_TX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_RD register will be read.
     *
     * \return The data read from the socket's SN_TX_RD register.
     */
    auto read_sn_tx_rd( Socket_ID socket_id ) const noexcept -> SN_TX_RD::Type
    {
        return read<SN_TX_RD::Type>( socket_id, SN_TX_RD::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_TX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_WR register will be read.
     *
     * \return The data read from the socket's SN_TX_WR register.
     */
    auto read_sn_tx_wr( Socket_ID socket_id ) const noexcept -> SN_TX_WR::Type
    {
        return read<SN_TX_WR::Type>( socket_id, SN_TX_WR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_WR register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_TX_WR register.
     */
    void write_sn_tx_wr( Socket_ID socket_id, SN_TX_WR::Type data ) noexcept
    {
        write( socket_id, SN_TX_WR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RX_RSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RSR register will be read.
     *
     * \return The data read from the socket's SN_RX_RSR register.
     */
    auto read_sn_rx_rsr( Socket_ID socket_id ) const noexcept -> SN_RX_RSR::Type
    {
        return read_non_atomic<SN_RX_RSR::Type>( socket_id, SN_RX_RSR::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_RX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RD register will be read.
     *
     * \return The data read from the socket's SN_RX_RD register.
     */
    auto read_sn_rx_rd( Socket_ID socket_id ) const noexcept -> SN_RX_RD::Type
    {
        return read<SN_RX_RD::Type>( socket_id, SN_RX_RD::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_RX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RD register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_RX_RD register.
     */
    void write_sn_rx_rd( Socket_ID socket_id, SN_RX_RD::Type data ) noexcept
    {
        write( socket_id, SN_RX_RD::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_WR register will be read.
     *
     * \return The data read from the socket's SN_RX_WR register.
     */
    auto read_sn_rx_wr( Socket_ID socket_id ) const noexcept -> SN_RX_WR::Type
    {
        return read<SN_RX_WR::Type>( socket_id, SN_RX_WR::MEMORY_OFFSET );
    }

    /**
     * \brief Read a socket's SN_IMR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IMR register will be read.
     *
     * \return The data read from the socket's SN_IMR register.
     */
    auto read_sn_imr( Socket_ID socket_id ) const noexcept -> SN_IMR::Type
    {
        return read<SN_IMR::Type>( socket_id, SN_IMR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_IMR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IMR register will be written to.
     * \param[in] data The data to write to the socket's SN_IMR register.
     */
    void write_sn_imr( Socket_ID socket_id, SN_IMR::Type data ) noexcept
    {
        write( socket_id, SN_IMR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_FRAG register.
     *
     * \param[in] socket_id The ID of the socket whose SN_FRAG register will be read.
     *
     * \return The data read from the socket's SN_FRAG register.
     */
    auto read_sn_frag( Socket_ID socket_id ) const noexcept -> SN_FRAG::Type
    {
        return read<SN_FRAG::Type>( socket_id, SN_FRAG::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_FRAG register.
     *
     * \param[in] socket_id The ID of the socket whose SN_FRAG register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_FRAG register.
     */
    void write_sn_frag( Socket_ID socket_id, SN_FRAG::Type data ) noexcept
    {
        write( socket_id, SN_FRAG::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_KPALVTR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_KPALVTR register will be read.
     *
     * \return The data read from the socket's SN_KPALVTR register.
     */
    auto read_sn_kpalvtr( Socket_ID socket_id ) const noexcept -> SN_KPALVTR::Type
    {
        return read<SN_KPALVTR::Type>( socket_id, SN_KPALVTR::MEMORY_OFFSET );
    }

    /**
     * \brief Write to a socket's SN_KPALVTR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_KPALVTR register will be written
     *            to.
     * \param[in] data The data to write to the socket's SN_KPALVTR register.
     */
    void write_sn_kpalvtr( Socket_ID socket_id, SN_KPALVTR::Type data ) noexcept
    {
        write( socket_id, SN_KPALVTR::MEMORY_OFFSET, data );
    }

    /**
     * \brief Read data from a socket's receive buffer.
     *
     * \param[in] socket_id The ID of the socket whose receive buffer will be read from.
     * \param[in] memory_offset The offset of the buffer memory to read from.
     * \param[in] begin The beginning of the data read from the receive buffer.
     * \param[in] end The end of the data read from the receive buffer.
     */
    void read_rx_buffer( Socket_ID socket_id, Memory_Offset memory_offset, std::uint8_t * begin, std::uint8_t * end ) const noexcept
    {
        Communication_Controller::read(
            socket_id, Socket_Memory_Block::RX_BUFFER, memory_offset, begin, end );
    }

    /**
     * \brief Write data to a socket's transmit buffer.
     *
     * \param[in] socket_id The ID of the socket whose transmit buffer will be written to.
     * \param[in] memory_offset The offset of the buffer memory to write to.
     * \param[in] begin The beginning of the data to write to the transmit buffer.
     * \param[in] end The end of the data to write to the transmit buffer.
     */
    void write_tx_buffer( Socket_ID socket_id, Memory_Offset memory_offset, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        Communication_Controller::write(
            socket_id, Socket_Memory_Block::TX_BUFFER, memory_offset, begin, end );
    }

  private:
    /**
     * \brief Read a common register.
     *
     * \tparam Type The type of register to read.
     *
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    template<typename Type>
    auto read( Memory_Offset memory_offset ) const noexcept -> Type
    {
        return read( memory_offset, Type{} );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Memory_Offset memory_offset, std::uint8_t ) const noexcept -> std::uint8_t
    {
        return Communication_Controller::read( memory_offset );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Memory_Offset memory_offset, std::uint16_t ) const noexcept -> std::uint16_t
    {
        Array<std::uint8_t, 2> buffer;

        Communication_Controller::read( memory_offset, buffer.begin(), buffer.end() );

        return static_cast<std::uint16_t>(
            ( buffer[ 0 ] << std::numeric_limits<std::uint8_t>::digits ) | buffer[ 1 ] );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Memory_Offset memory_offset, Array<std::uint8_t, 4> const & ) const noexcept
        -> Array<std::uint8_t, 4>
    {
        Array<std::uint8_t, 4> buffer;

        Communication_Controller::read( memory_offset, buffer.begin(), buffer.end() );

        return buffer;
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Memory_Offset memory_offset, Array<std::uint8_t, 6> const & ) const noexcept
        -> Array<std::uint8_t, 6>
    {
        Array<std::uint8_t, 6> buffer;

        Communication_Controller::read( memory_offset, buffer.begin(), buffer.end() );

        return buffer;
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Memory_Offset memory_offset, std::uint8_t data ) noexcept
    {
        Communication_Controller::write( memory_offset, data );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Memory_Offset memory_offset, std::uint16_t data ) noexcept
    {
        auto const buffer = Array<std::uint8_t, 2>{
            static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( data ),
        };

        Communication_Controller::write( memory_offset, buffer.begin(), buffer.end() );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Memory_Offset memory_offset, Array<std::uint8_t, 4> const & data ) noexcept
    {
        Communication_Controller::write( memory_offset, data.begin(), data.end() );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Memory_Offset memory_offset, Array<std::uint8_t, 6> const & data ) noexcept
    {
        Communication_Controller::write( memory_offset, data.begin(), data.end() );
    }

    /**
     * \brief Read a socket register.
     *
     * \tparam Type The type of register to read.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    template<typename Type>
    auto read( Socket_ID socket_id, Memory_Offset memory_offset ) const noexcept -> Type
    {
        return read( socket_id, memory_offset, Type{} );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Socket_ID socket_id, Memory_Offset memory_offset, std::uint8_t ) const noexcept
        -> std::uint8_t
    {
        return Communication_Controller::read( socket_id, Socket_Memory_Block::REGISTERS, memory_offset );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Socket_ID socket_id, Memory_Offset memory_offset, std::uint16_t ) const noexcept
        -> std::uint16_t
    {
        Array<std::uint8_t, 2> buffer;

        Communication_Controller::read(
            socket_id,
            Socket_Memory_Block::REGISTERS,
            memory_offset,
            buffer.begin(),
            buffer.end() );

        return static_cast<std::uint16_t>(
            ( buffer[ 0 ] << std::numeric_limits<std::uint8_t>::digits ) | buffer[ 1 ] );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Socket_ID socket_id, Memory_Offset memory_offset, Array<std::uint8_t, 4> const & ) const noexcept
        -> Array<std::uint8_t, 4>
    {
        Array<std::uint8_t, 4> buffer;

        Communication_Controller::read(
            socket_id,
            Socket_Memory_Block::REGISTERS,
            memory_offset,
            buffer.begin(),
            buffer.end() );

        return buffer;
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    auto read( Socket_ID socket_id, Memory_Offset memory_offset, Array<std::uint8_t, 6> const & ) const noexcept
        -> Array<std::uint8_t, 6>
    {
        Array<std::uint8_t, 6> buffer;

        Communication_Controller::read(
            socket_id,
            Socket_Memory_Block::REGISTERS,
            memory_offset,
            buffer.begin(),
            buffer.end() );

        return buffer;
    }

    /**
     * \brief Read a non-atomic socket register.
     *
     * \tparam Type The type of register to read.
     *
     * \param[in] socket_id The ID of the socket whose register will be read.
     * \param[in] memory_offset The offset of the register to read.
     *
     * \return The data read from the register.
     */
    template<typename Type>
    auto read_non_atomic( Socket_ID socket_id, Memory_Offset memory_offset ) const noexcept -> Type
    {
        for ( auto previous_data = read<Type>( socket_id, memory_offset );; ) {
            auto new_data = read<Type>( socket_id, memory_offset );

            if ( new_data == previous_data ) {
                return new_data;
            } // if

            previous_data = new_data;
        } // for
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be written to.
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Socket_ID socket_id, Memory_Offset memory_offset, std::uint8_t data ) noexcept
    {
        Communication_Controller::write(
            socket_id, Socket_Memory_Block::REGISTERS, memory_offset, data );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be written to.
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Socket_ID socket_id, Memory_Offset memory_offset, std::uint16_t data ) noexcept
    {
        auto const buffer = Array<std::uint8_t, 2>{
            static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( data ),
        };

        Communication_Controller::write(
            socket_id,
            Socket_Memory_Block::REGISTERS,
            memory_offset,
            buffer.begin(),
            buffer.end() );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be written to.
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Socket_ID socket_id, Memory_Offset memory_offset, Array<std::uint8_t, 4> const & data ) noexcept
    {
        Communication_Controller::write(
            socket_id, Socket_Memory_Block::REGISTERS, memory_offset, data.begin(), data.end() );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register will be written to.
     * \param[in] memory_offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     */
    void write( Socket_ID socket_id, Memory_Offset memory_offset, Array<std::uint8_t, 6> const & data ) noexcept
    {
        Communication_Controller::write(
            socket_id, Socket_Memory_Block::REGISTERS, memory_offset, data.begin(), data.end() );
    }
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H
