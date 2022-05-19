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
     * \param[in] memory_offset The offset of the memory to access.
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
     * \param[in] socket_memory_block The socket memory block to access.
     * \param[in] memory_offset The offset of the memory to access.
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

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H
