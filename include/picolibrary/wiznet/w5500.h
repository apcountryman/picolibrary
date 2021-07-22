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
 * \brief picolibrary::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_H
#define PICOLIBRARY_WIZNET_W5500_H

#include <cstdint>
#include <limits>
#include <utility>

#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/spi.h"
#include "picolibrary/void.h"

/**
 * \brief WIZnet W5500 facilities.
 */
namespace picolibrary::WIZnet::W5500 {

/**
 * \brief Control byte definitions.
 *
 * The control byte has the following fields:
 * - SPI Operation Mode Bits (OM)
 * - Read/Write Access Mode Bit (RWB)
 * - Block Select Bits (BSB)
 * - Region (REGION)
 * - Socket (SOCKET)
 */
struct Control_Byte {
    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto OM  = std::uint_fast8_t{ 2 }; ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ 1 }; ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ 5 }; ///< BSB.

        static constexpr auto REGION = std::uint_fast8_t{ 2 }; ///< REGION.
        static constexpr auto SOCKET = std::uint_fast8_t{ 3 }; ///< SOCKET.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto OM  = std::uint_fast8_t{};                  ///< OM.
        static constexpr auto RWB = std::uint_fast8_t{ OM + Size::OM };   ///< RWB.
        static constexpr auto BSB = std::uint_fast8_t{ RWB + Size::RWB }; ///< BSB.

        static constexpr auto REGION = std::uint_fast8_t{ RWB + Size::RWB }; ///< REGION.
        static constexpr auto SOCKET = std::uint_fast8_t{ REGION + Size::REGION }; ///< SOCKET.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto OM  = mask<std::uint8_t>( Size::OM, Bit::OM );   ///< OM.
        static constexpr auto RWB = mask<std::uint8_t>( Size::RWB, Bit::RWB ); ///< RWB.
        static constexpr auto BSB = mask<std::uint8_t>( Size::BSB, Bit::BSB ); ///< BSB.

        static constexpr auto REGION = mask<std::uint8_t>( Size::REGION, Bit::REGION ); ///< REGION.
        static constexpr auto SOCKET = mask<std::uint8_t>( Size::SOCKET, Bit::SOCKET ); ///< SOCKET.
    };
};

/**
 * \brief Socket ID.
 */
enum class Socket_ID : std::uint8_t {
    _0 = 0 << Control_Byte::Bit::SOCKET, ///< 0.
    _1 = 1 << Control_Byte::Bit::SOCKET, ///< 1.
    _2 = 2 << Control_Byte::Bit::SOCKET, ///< 2.
    _3 = 3 << Control_Byte::Bit::SOCKET, ///< 3.
    _4 = 4 << Control_Byte::Bit::SOCKET, ///< 4.
    _5 = 5 << Control_Byte::Bit::SOCKET, ///< 5.
    _6 = 6 << Control_Byte::Bit::SOCKET, ///< 6.
    _7 = 7 << Control_Byte::Bit::SOCKET, ///< 7.
};

/**
 * \brief Region.
 */
enum class Region : std::uint8_t {
    REGISTERS = 0b01 << Control_Byte::Bit::REGION, ///< Registers.
    TX_BUFFER = 0b10 << Control_Byte::Bit::REGION, ///< TX buffer.
    RX_BUFFER = 0b11 << Control_Byte::Bit::REGION, ///< RX buffer.
};

/**
 * \brief Operation.
 */
enum class Operation : std::uint8_t {
    READ  = 0b0 << Control_Byte::Bit::RWB, ///< Read.
    WRITE = 0b1 << Control_Byte::Bit::RWB, ///< Write.
};

/**
 * \brief SPI mode.
 */
enum class SPI_Mode : std::uint8_t {
    VARIABLE_LENGTH_DATA = 0b00 << Control_Byte::Bit::OM,     ///< Variable length data.
    FIXED_LENGTH_DATA_1_BYTE = 0b01 << Control_Byte::Bit::OM, ///< Fixed length data, 1 byte.
    FIXED_LENGTH_DATA_2_BYTE = 0b10 << Control_Byte::Bit::OM, ///< Fixed length data, 2 bytes.
    FIXED_LENGTH_DATA_4_BYTE = 0b11 << Control_Byte::Bit::OM, ///< Fixed length data, 4 bytes.
};

/**
 * \brief WIZnet W5500 communication controller.
 *
 * \tparam Controller_Type The type of SPI controller used to communicate with the W5500.
 * \tparam Device_Selector_Type The type of SPI device selector used to select and
 *         deselect the W5500.
 * \tparam Device The type of SPI device implementation used by the communication
 *         controller. The default SPI device implementation should be used unless a mock
 *         SPI device implementation is being injected to support unit testing of this
 *         communication controller.
 */
template<typename Controller_Type, typename Device_Selector_Type, typename Device = SPI::Device<Controller_Type, Device_Selector_Type>>
class Communication_Controller : public Device {
  public:
    /**
     * \brief The type of SPI controller used to communicate with the W5500.
     */
    using Controller = Controller_Type;

    /**
     * \brief The type of SPI device selector used to select and deselect the W5500.
     */
    using Device_Selector = Device_Selector_Type;

    Communication_Controller( Communication_Controller const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Communication_Controller() noexcept = default;

    auto operator=( Communication_Controller const & ) = delete;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Communication_Controller() = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the W5500.
     * \param[in] configuration The controller clock, and data exchange bit order
     *            configuration that meets the W5500's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            W5500.
     */
    constexpr Communication_Controller(
        Controller &                       controller,
        typename Controller::Configuration configuration,
        Device_Selector                    device_selector ) noexcept :
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
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator                     =( Communication_Controller && expression ) noexcept
        -> Communication_Controller & = default;

    /**
     * \brief Read a byte of common register memory.
     *
     * \param[in] offset The offset of the register memory to read.
     *
     * \return The data read from register memory if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset ) const noexcept -> Result<std::uint8_t, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( offset, Operation::READ );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->receive();
    }

    /**
     * \brief Read a block of common register memory.
     *
     * \param[in] offset The offset of the block of register memory to read.
     * \param[in] begin The beginning of the data read from the block of register memory.
     * \param[in] end The end of the data read from the block of register memory.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( offset, Operation::READ );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->receive( begin, end );
    }

    /**
     * \brief Write to a byte of common register memory.
     *
     * \param[in] offset The offset of the register memory to write to.
     * \param[in] data The data to write to register memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, std::uint8_t data ) noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( offset, Operation::WRITE );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->transmit( data );
    }

    /**
     * \brief Write to a block of common register memory.
     *
     * \param[in] offset The offset of the block of register memory to write to.
     * \param[in] begin The beginning of the data to write to the block of register
     *            memory.
     * \param[in] end The end of the data to write to the block of register memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( offset, Operation::WRITE );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->transmit( begin, end );
    }

    /**
     * \brief Read a byte of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            read.
     * \param[in] region The memory region to read.
     * \param[in] offset The offset of the register or buffer memory to read.
     *
     * \return The data read from register or buffer memory if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, Region region, std::uint16_t offset ) const noexcept
        -> Result<std::uint8_t, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( socket_id, region, offset, Operation::READ );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->receive();
    }

    /**
     * \brief Read a block of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            read.
     * \param[in] region The memory region to read.
     * \param[in] offset The offset of the register or or buffer memory to read.
     * \param[in] begin The beginning of the data read from the block of register or
     *            buffer memory.
     * \param[in] end The end of the data read from the block of register or buffer
     *            memory.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( socket_id, region, offset, Operation::READ );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->receive( begin, end );
    }

    /**
     * \brief Write to a byte of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            written to.
     * \param[in] region The memory region to write to.
     * \param[in] offset The offset of the register or buffer memory to write to.
     * \param[in] data The data to write to register or buffer memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t data ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( socket_id, region, offset, Operation::WRITE );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->transmit( data );
    }

    /**
     * \brief Write to a block of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            written to.
     * \param[in] region The memory region to write to.
     * \param[in] offset The offset of the block of register or buffer memory to write to.
     * \param[in] begin The beginning of the data to write to the block of register or
     *            buffer memory.
     * \param[in] end The end of the data to write to the block of register or buffer
     *            memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        auto const frame = make_frame( socket_id, region, offset, Operation::WRITE );

        auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
        {
            auto result = SPI::make_device_selection_guard( this->device_selector() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            guard = std::move( result ).value();
        }

        {
            auto result = this->transmit( frame.begin(), frame.end() );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return this->transmit( begin, end );
    }

  private:
    /**
     * \brief SPI communication frame.
     */
    using Frame = Fixed_Size_Array<std::uint8_t, 3>;

    /**
     * \brief Construct an SPI communication frame for accessing common register memory.
     *
     * \param[in] offset The offset of the register memory to be accessed.
     * \param[in] operation The operation to be performed.
     *
     * \return The SPI communication frame for accessing the register memory.
     */
    auto make_frame( std::uint16_t offset, Operation operation ) const noexcept
    {
        return Frame{
            static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( offset ),
            static_cast<std::uint8_t>(
                static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
                | static_cast<std::uint8_t>( operation ) ),
        };
    }

    /**
     * \brief Construct an SPI communication frame for accessing socket register or buffer
     *        memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            accessed.
     * \param[in] region The memory region to be accessed.
     * \param[in] offset The offset of the register or buffer memory to be accessed.
     * \param[in] operation The operation to be performed.
     *
     * \return The SPI communication frame for accessing the register or buffer memory.
     */
    auto make_frame( Socket_ID socket_id, Region region, std::uint16_t offset, Operation operation ) const noexcept
    {
        return Frame{
            static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( offset ),
            static_cast<std::uint8_t>(
                static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
                | static_cast<std::uint8_t>( socket_id ) | static_cast<std::uint8_t>( region )
                | static_cast<std::uint8_t>( operation ) ),
        };
    }
};

/**
 * \brief WIZnet W5500 Mode Register (MR).
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
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0000 };

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
        static constexpr auto RESERVED0 = mask<std::uint8_t>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto FARP = mask<std::uint8_t>( Size::FARP, Bit::FARP ); ///< FARP.
        static constexpr auto RESERVED2 = mask<std::uint8_t>( Size::RESERVED2, Bit::RESERVED2 ); ///< RESERVED2.
        static constexpr auto PPPOE = mask<std::uint8_t>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto PB  = mask<std::uint8_t>( Size::PB, Bit::PB );   ///< PB.
        static constexpr auto WOL = mask<std::uint8_t>( Size::WOL, Bit::WOL ); ///< WOL.
        static constexpr auto RESERVED6 = mask<std::uint8_t>( Size::RESERVED6, Bit::RESERVED6 ); ///< RESERVED6.
        static constexpr auto RST = mask<std::uint8_t>( Size::RST, Bit::RST ); ///< RST.
    };
};

/**
 * \brief WIZnet W5500 Gateway IP Address Register (GAR).
 */
struct GAR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 4>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0001 };
};

/**
 * \brief WIZnet W5500 Subnet Mask Register (SUBR).
 */
struct SUBR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 4>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0005 };
};

/**
 * \brief WIZnet W5500 Source Hardware Address Register (SHAR).
 */
struct SHAR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 6>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0009 };
};

/**
 * \brief WIZnet W5500 Source IP Address Register (SIPR).
 */
struct SIPR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 4>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x000F };
};

/**
 * \brief WIZnet W5500 Interrupt Low Level Timer Register (INTLEVEL).
 */
struct INTLEVEL {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0013 };
};

/**
 * \brief WIZnet W5500 Interrupt Register (IR).
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
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0015 };

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
        static constexpr auto RESERVED0 = mask<std::uint8_t>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto MP = mask<std::uint8_t>( Size::MP, Bit::MP ); ///< MP.
        static constexpr auto PPPOE = mask<std::uint8_t>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto UNREACH = mask<std::uint8_t>( Size::UNREACH, Bit::UNREACH ); ///< UNREACH.
        static constexpr auto CONFLICT = mask<std::uint8_t>( Size::CONFLICT, Bit::CONFLICT ); ///< CONFLICT.
    };
};

/**
 * \brief WIZnet W5500 Interrupt Mask Register (IMR).
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
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0016 };

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
        static constexpr auto RESERVED0 = mask<std::uint8_t>( Size::RESERVED0, Bit::RESERVED0 ); ///< RESERVED0.
        static constexpr auto MP = mask<std::uint8_t>( Size::MP, Bit::MP ); ///< MP.
        static constexpr auto PPPOE = mask<std::uint8_t>( Size::PPPOE, Bit::PPPOE ); ///< PPPOE.
        static constexpr auto UNREACH = mask<std::uint8_t>( Size::UNREACH, Bit::UNREACH ); ///< UNREACH.
        static constexpr auto CONFLICT = mask<std::uint8_t>( Size::CONFLICT, Bit::CONFLICT ); ///< CONFLICT.
    };
};

/**
 * \brief WIZnet W5500 Socket Interrupt Register (SIR).
 */
struct SIR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0017 };
};

/**
 * \brief WIZnet W5500 Socket Interrupt Mask Register (SIMR).
 */
struct SIMR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0018 };
};

/**
 * \brief WIZnet W5500 Retry Time Value Register (RTR).
 */
struct RTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0019 };
};

/**
 * \brief WIZnet W5500 Retry Count Register (RCR).
 */
struct RCR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001B };
};

/**
 * \brief WIZnet W5500 PPPoE LCP Request Timer Register (PTIMER).
 */
struct PTIMER {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001C };
};

/**
 * \brief WIZnet W5500 PPPoE LCP Magic Number Register (PMAGIC).
 */
struct PMAGIC {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001D };
};

/**
 * \brief WIZnet W5500 PPPoE Server Hardware Address Register (PHAR).
 */
struct PHAR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 6>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001E };
};

/**
 * \brief WIZnet W5500 PPPoE Server Session ID Register (PSID).
 */
struct PSID {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0024 };
};

/**
 * \brief WIZnet W5500 PPPoE Maximum Receive Unit Register (PMRU).
 */
struct PMRU {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0026 };
};

/**
 * \brief WIZnet W5500 Unreachable IP Address Register (UIPR).
 */
struct UIPR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 4>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0028 };
};

/**
 * \brief WIZnet W5500 Unreachable Port Register (UPORTR).
 */
struct UPORTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002C };
};

/**
 * \brief WIZnet W5500 PHY Configuration Register (PHYCFGR).
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
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002E };

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
        static constexpr auto LNK   = std::uint_fast8_t{};                  /// LNK
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
        static constexpr auto LNK = mask<std::uint8_t>( Size::LNK, Bit::LNK ); ///< LNK.
        static constexpr auto SPD = mask<std::uint8_t>( Size::SPD, Bit::SPD ); ///< SPD.
        static constexpr auto DPX = mask<std::uint8_t>( Size::DPX, Bit::DPX ); ///< DPX.
        static constexpr auto OPMDC = mask<std::uint8_t>( Size::OPMDC, Bit::OPMDC ); ///< OPMDC.
        static constexpr auto OPMD = mask<std::uint8_t>( Size::OPMD, Bit::OPMD ); ///< OPMD.
        static constexpr auto RST = mask<std::uint8_t>( Size::RST, Bit::RST ); ///< RST.
    };
};

/**
 * \brief WIZnet W5500 Chip Version Register (VERSIONR).
 */
struct VERSIONR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0039 };
};

/**
 * \brief WIZnet W5500 Socket N Mode Register (SN_MR).
 *
 * This register has the following fields:
 * - Protocol (P)
 * - UDP Unicast Blocking Enable (UCASTB)
 * - MACRAW IPv6 Packet Blocking Enable (MIP6B)
 * - TCP No Delayed ACK Enable (ND)
 * - UDP Multicast IGMP Version (MC)
 * - MACRAW Multicast Blocking Enable (MMB)
 * - UDP/MACRAW Broadcast Blocking Enable (BCASTB)
 * - UDP Multicasting Enable (MULTI)
 * - MACRAW MAC Filter Enable (MFEN)
 */
struct SN_MR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0000 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto P      = std::uint_fast8_t{ 4 }; ///< P.
        static constexpr auto UCASTB = std::uint_fast8_t{ 1 }; ///< UCASTB.
        static constexpr auto MIP6B  = std::uint_fast8_t{ 1 }; ///< MIP6B.
        static constexpr auto ND     = std::uint_fast8_t{ 1 }; ///< ND.
        static constexpr auto MC     = std::uint_fast8_t{ 1 }; ///< MC.
        static constexpr auto MMB    = std::uint_fast8_t{ 1 }; ///< MMB.
        static constexpr auto BCASTB = std::uint_fast8_t{ 1 }; ///< BCASTB.
        static constexpr auto MULTI  = std::uint_fast8_t{ 1 }; ///< MULTI.
        static constexpr auto MFEN   = std::uint_fast8_t{ 1 }; ///< MFEN.
    };

    /**
     * \brief Field bit positions.
     */
    struct Bit {
        static constexpr auto P      = std::uint_fast8_t{};              ///< P.
        static constexpr auto UCASTB = std::uint_fast8_t{ P + Size::P }; ///< UCASTB.
        static constexpr auto MIP6B  = std::uint_fast8_t{ P + Size::P }; ///< MIP6B.
        static constexpr auto ND  = std::uint_fast8_t{ UCASTB + Size::UCASTB }; ///< ND.
        static constexpr auto MC  = std::uint_fast8_t{ UCASTB + Size::UCASTB }; ///< MC.
        static constexpr auto MMB = std::uint_fast8_t{ UCASTB + Size::UCASTB }; ///< MMB.
        static constexpr auto BCASTB = std::uint_fast8_t{ ND + Size::ND }; ///< BCASTB.
        static constexpr auto MULTI = std::uint_fast8_t{ BCASTB + Size::BCASTB }; ///< MULTI.
        static constexpr auto MFEN = std::uint_fast8_t{ BCASTB + Size::BCASTB }; ///< MFEN.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto P = mask<std::uint8_t>( Size::P, Bit::P ); ///< P.
        static constexpr auto UCASTB = mask<std::uint8_t>( Size::UCASTB, Bit::UCASTB ); ///< UCASTB.
        static constexpr auto MIP6B = mask<std::uint8_t>( Size::MIP6B, Bit::MIP6B ); ///< MIP6B.
        static constexpr auto ND  = mask<std::uint8_t>( Size::ND, Bit::ND );   ///< ND.
        static constexpr auto MC  = mask<std::uint8_t>( Size::MC, Bit::MC );   ///< MC.
        static constexpr auto MMB = mask<std::uint8_t>( Size::MMB, Bit::MMB ); ///< MMB.
        static constexpr auto BCASTB = mask<std::uint8_t>( Size::BCASTB, Bit::BCASTB ); ///< BCASTB.
        static constexpr auto MULTI = mask<std::uint8_t>( Size::MULTI, Bit::MULTI ); ///< MULTI.
        static constexpr auto MFEN = mask<std::uint8_t>( Size::MFEN, Bit::MFEN ); ///< MFEN.
    };
};

/**
 * \brief WIZnet W5500 Socket N Command Register (SN_CR).
 */
struct SN_CR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0001 };
};

/**
 * \brief WIZnet W5500 Socket N Interrupt Register (SN_IR).
 *
 * This register has the following fields:
 * - Peer Connected (CON)
 * - Peer Disconnected (DISCON)
 * - Data Received (RECV)
 * - Timeout (TIMEOUT)
 * - Transmission Complete (SEND_OK)
 */
struct SN_IR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0002 };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto CON       = std::uint_fast8_t{ 1 }; ///< CON.
        static constexpr auto DISCON    = std::uint_fast8_t{ 1 }; ///< DISCON.
        static constexpr auto RECV      = std::uint_fast8_t{ 1 }; ///< RECV.
        static constexpr auto TIMEOUT   = std::uint_fast8_t{ 1 }; ///< TIMEOUT.
        static constexpr auto SEND_OK   = std::uint_fast8_t{ 1 }; ///< SEND_OK.
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
        static constexpr auto SEND_OK = std::uint_fast8_t{ TIMEOUT + Size::TIMEOUT }; ///< SEND_OK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ SEND_OK + Size::SEND_OK }; ///< RESERVED5.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto CON = mask<std::uint8_t>( Size::CON, Bit::CON ); ///< CON.
        static constexpr auto DISCON = mask<std::uint8_t>( Size::DISCON, Bit::DISCON ); ///< DISCON.
        static constexpr auto RECV = mask<std::uint8_t>( Size::RECV, Bit::RECV ); ///< RECV.
        static constexpr auto TIMEOUT = mask<std::uint8_t>( Size::TIMEOUT, Bit::TIMEOUT ); ///< TIMEOUT.
        static constexpr auto SEND_OK = mask<std::uint8_t>( Size::SEND_OK, Bit::SEND_OK ); ///< SEND_OK.
        static constexpr auto RESERVED5 = mask<std::uint8_t>( Size::RESERVED5, Bit::RESERVED5 ); ///< RESERVED5.
    };
};

/**
 * \brief WIZnet W5500 Socket N Status Register (SN_SR).
 */
struct SN_SR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0003 };
};

/**
 * \brief WIZnet W5500 Socket N Source Port Register (SN_PORT).
 */
struct SN_PORT {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0004 };
};

/**
 * \brief WIZnet W5500 Socket N Destination Hardware Address Register (SN_DHAR).
 */
struct SN_DHAR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 6>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0006 };
};

/**
 * \brief WIZnet W5500 Socket N Destination IP Address Register (SN_DIPR).
 */
struct SN_DIPR {
    /**
     * \brief Register type.
     */
    using Type = Fixed_Size_Array<std::uint8_t, 4>;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x000C };
};

/**
 * \brief WIZnet W5500 Socket N Destination Port Register (SN_DPORT).
 */
struct SN_DPORT {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0010 };
};

/**
 * \brief WIZnet W5500 Socket N Maximum Segment Size Register (SN_MSSR).
 */
struct SN_MSSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0012 };
};

/**
 * \brief WIZnet W5500 Socket N TOS Register (SN_TOS).
 */
struct SN_TOS {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0015 };
};

/**
 * \brief WIZnet W5500 Socket N TTL Register (SN_TTL).
 */
struct SN_TTL {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0016 };
};

/**
 * \brief WIZnet W5500 Socket N Receive Buffer Size Register (SN_RXBUF_SIZE).
 */
struct SN_RXBUF_SIZE {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001E };
};

/**
 * \brief WIZnet W5500 Socket N Transmit Buffer Size Register (SN_TXBUF_SIZE).
 */
struct SN_TXBUF_SIZE {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x001F };
};

/**
 * \brief WIZnet W5500 Socket N Transmit Buffer Free Size Register (SN_TX_FSR).
 */
struct SN_TX_FSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0020 };
};

/**
 * \brief WIZnet W5500 Socket N Transmit Buffer Read Pointer Register (SN_TX_RD).
 */
struct SN_TX_RD {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0022 };
};

/**
 * \brief WIZnet W5500 Socket N Transmit Buffer Write Pointer Register (SN_TX_WR).
 */
struct SN_TX_WR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0024 };
};

/**
 * \brief WIZnet W5500 Socket N Receive Buffer Received Size Register (SN_RX_RSR).
 */
struct SN_RX_RSR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0026 };
};

/**
 * \brief WIZnet W5500 Socket N Receive Buffer Read Pointer Register (SN_RX_RD).
 */
struct SN_RX_RD {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x0028 };
};

/**
 * \brief WIZnet W5500 Socket N Receive Buffer Write Pointer Register (SN_RX_WR).
 */
struct SN_RX_WR {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002A };
};

/**
 * \brief WIZnet W5500 Socket N Interrupt Mask Register (SN_IMR).
 *
 * This register has the following fields:
 * - Peer Connected (CON)
 * - Peer Disconnected (DISCON)
 * - Data Received (RECV)
 * - Timeout (TIMEOUT)
 * - Transmission Complete (SEND_OK)
 */
struct SN_IMR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002C };

    /**
     * \brief Field sizes.
     */
    struct Size {
        static constexpr auto CON       = std::uint_fast8_t{ 1 }; ///< CON.
        static constexpr auto DISCON    = std::uint_fast8_t{ 1 }; ///< DISCON.
        static constexpr auto RECV      = std::uint_fast8_t{ 1 }; ///< RECV.
        static constexpr auto TIMEOUT   = std::uint_fast8_t{ 1 }; ///< TIMEOUT.
        static constexpr auto SEND_OK   = std::uint_fast8_t{ 1 }; ///< SEND_OK.
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
        static constexpr auto SEND_OK = std::uint_fast8_t{ TIMEOUT + Size::TIMEOUT }; ///< SEND_OK.
        static constexpr auto RESERVED5 = std::uint_fast8_t{ SEND_OK + Size::SEND_OK }; ///< RESERVED5.
    };

    /**
     * \brief Field bit masks.
     */
    struct Mask {
        static constexpr auto CON = mask<std::uint8_t>( Size::CON, Bit::CON ); ///< CON.
        static constexpr auto DISCON = mask<std::uint8_t>( Size::DISCON, Bit::DISCON ); ///< DISCON.
        static constexpr auto RECV = mask<std::uint8_t>( Size::RECV, Bit::RECV ); ///< RECV.
        static constexpr auto TIMEOUT = mask<std::uint8_t>( Size::TIMEOUT, Bit::TIMEOUT ); ///< TIMEOUT.
        static constexpr auto SEND_OK = mask<std::uint8_t>( Size::SEND_OK, Bit::SEND_OK ); ///< SEND_OK.
        static constexpr auto RESERVED5 = mask<std::uint8_t>( Size::RESERVED5, Bit::RESERVED5 ); ///< RESERVED5.
    };
};

/**
 * \brief WIZnet W5500 Socket N Fragment Offset In IP Header Register (SN_FRAG).
 */
struct SN_FRAG {
    /**
     * \brief Register type.
     */
    using Type = std::uint16_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002D };
};

/**
 * \brief WIZnet W5500 Socket N Keep Alive Time Register (SN_KPALVTR).
 */
struct SN_KPALVTR {
    /**
     * \brief Register type.
     */
    using Type = std::uint8_t;

    /**
     * \brief Register offset.
     */
    static constexpr auto OFFSET = std::uint16_t{ 0x002F };
};

/**
 * \brief WIZnet W5500 driver.
 *
 * \tparam Controller_Type The type of SPI controller used to communicate with the W5500.
 * \tparam Device_Selector_Type The type of SPI device selector used to select and
 *         deselect the W5500.
 * \tparam Communication_Controller_Type The type of communication controller
 *         implementation used by the driver. The default communication controller
 *         implementation should be used unless a mock implementation is begin injected to
 *         support unit testing of this driver.
 */
template<typename Controller_Type, typename Device_Selector_Type, typename Communication_Controller_Type = Communication_Controller<Controller_Type, Device_Selector_Type>>
class Driver : public Communication_Controller_Type {
  public:
    /**
     * \brief The type of SPI controller used to communicate with the W5500.
     */
    using Controller = Controller_Type;

    /**
     * \brief The type of SPI device selector used to select and deselect the W5500.
     */
    using Device_Selector = Device_Selector_Type;

    /**
     * \brief Constructor.
     */
    constexpr Driver() = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the W5500.
     * \param[in] configuration The controller clock, and data exchange bit order
     *            configuration that meets the W5500's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            W5500.
     */
    constexpr Driver( Controller & controller, typename Controller::Configuration configuration, Device_Selector device_selector ) noexcept
        :
        Communication_Controller_Type{ controller, configuration, std::move( device_selector ) }
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
    auto operator=( Driver && expression ) noexcept -> Driver & = default;

    auto operator=( Driver const & ) = delete;

    /**
     * \brief Read the MR register.
     *
     * \return The data read from the MR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_mr() const noexcept
    {
        return this->read( MR::OFFSET );
    }

    /**
     * \brief Write to the MR register.
     *
     * \param[in] data The data to write to the MR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_mr( MR::Type data ) noexcept
    {
        return this->write( MR::OFFSET, data );
    }

    /**
     * \brief Read the GAR register.
     *
     * \return The data read from the GAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_gar() const noexcept -> Result<GAR::Type, Error_Code>
    {
        GAR::Type data;

        auto result = this->read( GAR::OFFSET, data.begin(), data.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return data;
    }

    /**
     * \brief Write to the GAR register.
     *
     * \param[in] data The data to write to the GAR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_gar( GAR::Type const & data ) noexcept
    {
        return this->write( GAR::OFFSET, data.begin(), data.end() );
    }

    /**
     * \brief Read the SUBR register.
     *
     * \return The data read from the SUBR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_subr() const noexcept -> Result<SUBR::Type, Error_Code>
    {
        SUBR::Type data;

        auto result = this->read( SUBR::OFFSET, data.begin(), data.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return data;
    }

    /**
     * \brief Write to the SUBR register.
     *
     * \param[in] data The data to write to the SUBR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_subr( SUBR::Type const & data ) noexcept
    {
        return this->write( SUBR::OFFSET, data.begin(), data.end() );
    }

    /**
     * \brief Read the SHAR register.
     *
     * \return The data read from the SHAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_shar() const noexcept -> Result<SHAR::Type, Error_Code>
    {
        SHAR::Type data;

        auto result = this->read( SHAR::OFFSET, data.begin(), data.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return data;
    }

    /**
     * \brief Write to the SHAR register.
     *
     * \param[in] data The data to write to the SHAR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_shar( SHAR::Type const & data ) noexcept
    {
        return this->write( SHAR::OFFSET, data.begin(), data.end() );
    }

    /**
     * \brief Read the SIPR register.
     *
     * \return The data read from the SIPR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sipr() const noexcept -> Result<SIPR::Type, Error_Code>
    {
        SIPR::Type data;

        auto result = this->read( SIPR::OFFSET, data.begin(), data.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return data;
    }

    /**
     * \brief Write to the SIPR register.
     *
     * \param[in] data The data to write to the SIPR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sipr( SIPR::Type const & data ) noexcept
    {
        return this->write( SIPR::OFFSET, data.begin(), data.end() );
    }

    /**
     * \brief Read the INTLEVEL register.
     *
     * \return The data read from the INTLEVEL register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_intlevel() const noexcept -> Result<INTLEVEL::Type, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 2> array;

        auto result = this->read( INTLEVEL::OFFSET, array.begin(), array.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return convert_array_to_data( array );
    }

    /**
     * \brief Write to the INTLEVEL register.
     *
     * \param[in] data The data to write to the INTLEVEL register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_intlevel( INTLEVEL::Type data ) noexcept
    {
        auto const array = convert_data_to_array( data );

        return this->write( INTLEVEL::OFFSET, array.begin(), array.end() );
    }

    /**
     * \brief Read the IR register.
     *
     * \return The data read from the IR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_ir() const noexcept
    {
        return this->read( IR::OFFSET );
    }

    /**
     * \brief Write to the IR register.
     *
     * \param[in] data The data to write to the IR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_ir( IR::Type data ) noexcept
    {
        return this->write( IR::OFFSET, data );
    }

    /**
     * \brief Read the IMR register.
     *
     * \return The data read from the IMR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_imr() const noexcept
    {
        return this->read( IMR::OFFSET );
    }

    /**
     * \brief Write to the IMR register.
     *
     * \param[in] data The data to write to the IMR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_imr( IMR::Type data ) noexcept
    {
        return this->write( IMR::OFFSET, data );
    }

  private:
    /**
     * \brief Convert an array to data.
     *
     * \param[in] array The array to convert.
     *
     * \return The result of the conversion.
     */
    static constexpr auto convert_array_to_data( Fixed_Size_Array<std::uint8_t, 2> const & array ) noexcept
    {
        return static_cast<std::uint16_t>(
            ( array[ 0 ] << std::numeric_limits<std::uint8_t>::digits ) | array[ 1 ] );
    }

    /**
     * \brief Convert data to an array.
     *
     * \param[in] data The data to convert.
     *
     * \return The result of the conversion.
     */
    static constexpr auto convert_data_to_array( std::uint16_t data ) noexcept
    {
        return Fixed_Size_Array<std::uint8_t, 2>{
            static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( data ),
        };
    }
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H
