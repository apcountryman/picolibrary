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
 * \brief WIZnet W5500 socket buffer size.
 */
enum class Buffer_Size : std::uint8_t {
    _0_KIB  = 0,  ///< 0 KiB.
    _1_KIB  = 1,  ///< 1 KiB.
    _2_KIB  = 2,  ///< 2 KiB.
    _4_KIB  = 4,  ///< 4 KiB.
    _8_KIB  = 8,  ///< 8 KiB.
    _16_KIB = 16, ///< 16 KiB.
};

/**
 * \brief WIZnet W5500 socket protocol.
 */
enum class Protocol : std::uint8_t {
    CLOSED = 0b0000 << SN_MR::Bit::P, ///< Closed.
    TCP    = 0b0001 << SN_MR::Bit::P, ///< TCP.
    UDP    = 0b0010 << SN_MR::Bit::P, ///< UDP.
    MACRAW = 0b0100 << SN_MR::Bit::P, ///< MACRAW.
};

/**
 * \brief WIZnet W5500 socket MACRAW IPv6 packet blocking configuration.
 */
enum class MACRAW_IPv6_Packet_Blocking : std::uint8_t {
    DISABLED = 0b0 << SN_MR::Bit::MIP6B, ///< Disabled.
    ENABLED  = 0b1 << SN_MR::Bit::MIP6B, ///< Enabled.
};

/**
 * \brief WIZnet W5500 socket MACRAW MAC filtering configuration.
 */
enum class MACRAW_MAC_Filtering : std::uint8_t {
    DISABLED = 0b0 << SN_MR::Bit::MFEN, ///< Disabled.
    ENABLED  = 0b1 << SN_MR::Bit::MFEN, ///< Enabled.
};

/**
 * \brief WIZnet W5500 socket command.
 */
enum class Command : std::uint8_t {
    NONE                         = 0x00, ///< None.
    OPEN                         = 0x01, ///< Open.
    LISTEN                       = 0x02, ///< Listen.
    CONNECT                      = 0x04, ///< Connect.
    DISCONNECT                   = 0x08, ///< Disconnect.
    CLOSE                        = 0x10, ///< Close.
    SEND                         = 0x20, ///< Send.
    SEND_UDP_DATAGRAM_MANUAL_MAC = 0x21, ///< Send a UDP datagram using a manually configured destination MAC address instead of one obtained by ARP.
    SEND_TCP_KEEP_ALIVE_PACKET   = 0x22, ///< Send a TCP keep-alive packet.
    RECEIVE                      = 0x40, ///< Receive.
};

/**
 * \brief WIZnet W5500 socket status.
 */
enum class Socket_Status : std::uint8_t {
    CLOSED        = 0x00, ///< Closed.
    OPENED_TCP    = 0x13, ///< Opened (TCP).
    LISTEN        = 0x14, ///< Waiting for connection request from remote endpoint.
    ESTABLISED    = 0x17, ///< Established.
    CLOSE_WAIT    = 0x1C, ///< Waiting for connection termination request from local user.
    OPENED_UDP    = 0x22, ///< Opened (UDP).
    OPENED_MACRAW = 0x42, ///< Opened (MACRAW).
    SYN_SENT = 0x15, ///< Connection request sent, waiting for matching connection request.
    SYN_RECEIVED = 0x16, ///< Connection request sent and received, waiting for request acknowledgement.
    FIN_WAIT     = 0x18, ///< Waiting for connection termination request, or acknowledgement of previously sent termination request, from remote endpoint.
    CLOSING      = 0x1A, ///< Waiting for connection termination request acknowledgement from remote endpoint.
    TIME_WAIT    = 0x1B, ///< Waiting for enough time to pass to be sure the remote endpoint received the connection termination request acknowledgement.
    LAST_ACK     = 0x1D, ///< Waiting for connection termination request acknowledgement from remote endpoint.
};

/**
 * \brief WIZnet W5500 socket interrupt masks.
 */
struct Socket_Interrupt {
    static constexpr auto PEER_CONNECTED    = SN_IR::Mask::CON;    ///< Peer connected.
    static constexpr auto PEER_DISCONNECTED = SN_IR::Mask::DISCON; ///< Peer disconnected.
    static constexpr auto DATA_RECEIVED     = SN_IR::Mask::RECV;   ///< Data received.
    static constexpr auto TIMEOUT   = SN_IR::Mask::TIMEOUT; ///< ARP/TCP timeout occurred.
    static constexpr auto DATA_SENT = SN_IR::Mask::SEND_OK; ///< SEND command completed.
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
template<typename Controller_Type, typename Device_Selector_Type, typename Communication_Controller = ::picolibrary::WIZnet::W5500::Communication_Controller<Controller_Type, Device_Selector_Type>>
class Driver : public Communication_Controller {
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
        return read<MR::Type>( MR::OFFSET );
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
        return write( MR::OFFSET, data );
    }

    /**
     * \brief Read the GAR register.
     *
     * \return The data read from the GAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_gar() const noexcept
    {
        return read<GAR::Type>( GAR::OFFSET );
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
        return write( GAR::OFFSET, data );
    }

    /**
     * \brief Read the SUBR register.
     *
     * \return The data read from the SUBR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_subr() const noexcept
    {
        return read<SUBR::Type>( SUBR::OFFSET );
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
        return write( SUBR::OFFSET, data );
    }

    /**
     * \brief Read the SHAR register.
     *
     * \return The data read from the SHAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_shar() const noexcept
    {
        return read<SHAR::Type>( SHAR::OFFSET );
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
        return write( SHAR::OFFSET, data );
    }

    /**
     * \brief Read the SIPR register.
     *
     * \return The data read from the SIPR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sipr() const noexcept
    {
        return read<SIPR::Type>( SIPR::OFFSET );
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
        return write( SIPR::OFFSET, data );
    }

    /**
     * \brief Read the INTLEVEL register.
     *
     * \return The data read from the INTLEVEL register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_intlevel() const noexcept
    {
        return read<INTLEVEL::Type>( INTLEVEL::OFFSET );
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
        return write( INTLEVEL::OFFSET, data );
    }

    /**
     * \brief Read the IR register.
     *
     * \return The data read from the IR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_ir() const noexcept
    {
        return read<IR::Type>( IR::OFFSET );
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
        return write( IR::OFFSET, data );
    }

    /**
     * \brief Read the IMR register.
     *
     * \return The data read from the IMR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_imr() const noexcept
    {
        return read<IMR::Type>( IMR::OFFSET );
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
        return write( IMR::OFFSET, data );
    }

    /**
     * \brief Read the SIR register.
     *
     * \return The data read from the SIR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sir() const noexcept
    {
        return read<SIR::Type>( SIR::OFFSET );
    }

    /**
     * \brief Read the SIMR register.
     *
     * \return The data read from the SIMR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_simr() const noexcept
    {
        return read<SIMR::Type>( SIMR::OFFSET );
    }

    /**
     * \brief Write to the SIMR register.
     *
     * \param[in] data The data to write to the SIMR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_simr( SIMR::Type data ) noexcept
    {
        return write( SIMR::OFFSET, data );
    }

    /**
     * \brief Read the RTR register.
     *
     * \return The data read from the RTR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_rtr() const noexcept
    {
        return read<RTR::Type>( RTR::OFFSET );
    }

    /**
     * \brief Write to the RTR register.
     *
     * \param[in] data The data to write to the RTR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_rtr( RTR::Type data ) noexcept
    {
        return write( RTR::OFFSET, data );
    }

    /**
     * \brief Read the RCR register.
     *
     * \return The data read from the RCR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_rcr() const noexcept
    {
        return read<RCR::Type>( RCR::OFFSET );
    }

    /**
     * \brief Write to the RCR register.
     *
     * \param[in] data The data to write to the RCR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_rcr( RCR::Type data ) noexcept
    {
        return write( RCR::OFFSET, data );
    }

    /**
     * \brief Read the PTIMER register.
     *
     * \return The data read from the PTIMER register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_ptimer() const noexcept
    {
        return read<PTIMER::Type>( PTIMER::OFFSET );
    }

    /**
     * \brief Write to the PTIMER register.
     *
     * \param[in] data The data to write to the PTIMER register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_ptimer( PTIMER::Type data ) noexcept
    {
        return write( PTIMER::OFFSET, data );
    }

    /**
     * \brief Read the PMAGIC register.
     *
     * \return The data read from the PMAGIC register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_pmagic() const noexcept
    {
        return read<PMAGIC::Type>( PMAGIC::OFFSET );
    }

    /**
     * \brief Write to the PMAGIC register.
     *
     * \param[in] data The data to write to the PMAGIC register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_pmagic( PMAGIC::Type data ) noexcept
    {
        return write( PMAGIC::OFFSET, data );
    }

    /**
     * \brief Read the PHAR register.
     *
     * \return The data read from the PHAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_phar() const noexcept
    {
        return read<PHAR::Type>( PHAR::OFFSET );
    }

    /**
     * \brief Write to the PHAR register.
     *
     * \param[in] data The data to write to the PHAR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_phar( PHAR::Type const & data ) noexcept
    {
        return write( PHAR::OFFSET, data );
    }

    /**
     * \brief Read the PSID register.
     *
     * \return The data read from the PSID register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_psid() const noexcept
    {
        return read<PSID::Type>( PSID::OFFSET );
    }

    /**
     * \brief Write to the PSID register.
     *
     * \param[in] data The data to write to the PSID register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_psid( PSID::Type data ) noexcept
    {
        return write( PSID::OFFSET, data );
    }

    /**
     * \brief Read the PMRU register.
     *
     * \return The data read from the PMRU register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_pmru() const noexcept
    {
        return read<PMRU::Type>( PMRU::OFFSET );
    }

    /**
     * \brief Write to the PMRU register.
     *
     * \param[in] data The data to write to the PMRU register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_pmru( PMRU::Type data ) noexcept
    {
        return write( PMRU::OFFSET, data );
    }

    /**
     * \brief Read the UIPR register.
     *
     * \return The data read from the UIPR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_uipr() const noexcept
    {
        return read<UIPR::Type>( UIPR::OFFSET );
    }

    /**
     * \brief Read the UPORTR register.
     *
     * \return The data read from the UPORTR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_uportr() const noexcept
    {
        return read<UPORTR::Type>( UPORTR::OFFSET );
    }

    /**
     * \brief Read the PHYCFGR register.
     *
     * \return The data read from the PHYCFGR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_phycfgr() const noexcept
    {
        return read<PHYCFGR::Type>( PHYCFGR::OFFSET );
    }

    /**
     * \brief Write to the PHYCFGR register.
     *
     * \param[in] data The data to write to the PHYCFGR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_phycfgr( PHYCFGR::Type data ) noexcept
    {
        return write( PHYCFGR::OFFSET, data );
    }

    /**
     * \brief Read the VERSIONR register.
     *
     * \return The data read from the VERSIONR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_versionr() const noexcept
    {
        return read<VERSIONR::Type>( VERSIONR::OFFSET );
    }

    /**
     * \brief Read a socket's SN_MR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MR register is to be read.
     *
     * \return The data read from the socket's SN_MR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_mr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_MR::Type>( socket_id, SN_MR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_MR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MR register is to be written to.
     * \param[in] data The data to write to the socket's SN_MR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_mr( Socket_ID socket_id, SN_MR::Type data ) noexcept
    {
        return write( socket_id, SN_MR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_CR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_CR register is to be read.
     *
     * \return The data read from the socket's SN_CR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_cr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_CR::Type>( socket_id, SN_CR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_CR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_CR register is to be written to.
     * \param[in] data The data to write to the socket's SN_CR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_cr( Socket_ID socket_id, SN_CR::Type data ) noexcept
    {
        return write( socket_id, SN_CR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_IR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IR register is to be read.
     *
     * \return The data read from the socket's SN_IR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_ir( Socket_ID socket_id ) const noexcept
    {
        return read<SN_IR::Type>( socket_id, SN_IR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_IR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IR register is to be written to.
     * \param[in] data The data to write to the socket's SN_IR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_ir( Socket_ID socket_id, SN_IR::Type data ) noexcept
    {
        return write( socket_id, SN_IR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_SR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_SR register is to be read.
     *
     * \return The data read from the socket's SN_SR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_sr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_SR::Type>( socket_id, SN_SR::OFFSET );
    }

    /**
     * \brief Read a socket's SN_PORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_PORT register is to be read.
     *
     * \return The data read from the socket's SN_PORT register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_port( Socket_ID socket_id ) const noexcept
    {
        return read<SN_PORT::Type>( socket_id, SN_PORT::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_PORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_PORT register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_PORT register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_port( Socket_ID socket_id, SN_PORT::Type data ) noexcept
    {
        return write( socket_id, SN_PORT::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DHAR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DHAR register is to be read.
     *
     * \return The data read from the socket's SN_DHAR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_dhar( Socket_ID socket_id ) const noexcept
    {
        return read<SN_DHAR::Type>( socket_id, SN_DHAR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DHAR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DHAR register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DHAR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_dhar( Socket_ID socket_id, SN_DHAR::Type const & data ) noexcept
    {
        return write( socket_id, SN_DHAR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DIPR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DIPR register is to be read.
     *
     * \return The data read from the socket's SN_DIPR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_dipr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_DIPR::Type>( socket_id, SN_DIPR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DIPR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DIPR register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DIPR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_dipr( Socket_ID socket_id, SN_DIPR::Type const & data ) noexcept
    {
        return write( socket_id, SN_DIPR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_DPORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DPORT register is to be read.
     *
     * \return The data read from the socket's SN_DPORT register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_dport( Socket_ID socket_id ) const noexcept
    {
        return read<SN_DPORT::Type>( socket_id, SN_DPORT::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_DPORT register.
     *
     * \param[in] socket_id The ID of the socket whose SN_DPORT register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_DPORT register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_dport( Socket_ID socket_id, SN_DPORT::Type data ) noexcept
    {
        return write( socket_id, SN_DPORT::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_MSSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MSSR register is to be read.
     *
     * \return The data read from the socket's SN_MSSR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_mssr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_MSSR::Type>( socket_id, SN_MSSR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_MSSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_MSSR register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_MSSR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_mssr( Socket_ID socket_id, SN_MSSR::Type data ) noexcept
    {
        return write( socket_id, SN_MSSR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TOS register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TOS register is to be read.
     *
     * \return The data read from the socket's SN_TOS register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_tos( Socket_ID socket_id ) const noexcept
    {
        return read<SN_TOS::Type>( socket_id, SN_TOS::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TOS register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TOS register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_TOS register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_tos( Socket_ID socket_id, SN_TOS::Type data ) noexcept
    {
        return write( socket_id, SN_TOS::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TTL register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TTL register is to be read.
     *
     * \return The data read from the socket's SN_TTL register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_ttl( Socket_ID socket_id ) const noexcept
    {
        return read<SN_TTL::Type>( socket_id, SN_TTL::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TTL register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TTL register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_TTL register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_ttl( Socket_ID socket_id, SN_TTL::Type data ) noexcept
    {
        return write( socket_id, SN_TTL::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RXBUF_SIZE register is to be
     *            read.
     *
     * \return The data read from the socket's SN_RXBUF_SIZE register if the read
     *         succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_rxbuf_size( Socket_ID socket_id ) const noexcept
    {
        return read<SN_RXBUF_SIZE::Type>( socket_id, SN_RXBUF_SIZE::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_RXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RXBUF_SIZE register is to be
     *            written to.
     * \param[in] data The data to write to the socket's SN_RXBUF_SIZE register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_rxbuf_size( Socket_ID socket_id, SN_RXBUF_SIZE::Type data ) noexcept
    {
        return write( socket_id, SN_RXBUF_SIZE::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TXBUF_SIZE register is to be
     *            read.
     *
     * \return The data read from the socket's SN_TXBUF_SIZE register if the read
     *         succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_txbuf_size( Socket_ID socket_id ) const noexcept
    {
        return read<SN_TXBUF_SIZE::Type>( socket_id, SN_TXBUF_SIZE::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TXBUF_SIZE register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TXBUF_SIZE register is to be
     *            written to.
     * \param[in] data The data to write to the socket's SN_TXBUF_SIZE register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_txbuf_size( Socket_ID socket_id, SN_TXBUF_SIZE::Type data ) noexcept
    {
        return write( socket_id, SN_TXBUF_SIZE::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_TX_FSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_FSR register is to be read.
     *
     * \return The data read from the socket's SN_TX_FSR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_tx_fsr( Socket_ID socket_id ) const noexcept
    {
        return read_unstable<SN_TX_FSR::Type>( socket_id, SN_TX_FSR::OFFSET );
    }

    /**
     * \brief Read a socket's SN_TX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_RD register is to be read.
     *
     * \return The data read from the socket's SN_TX_RD register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_tx_rd( Socket_ID socket_id ) const noexcept
    {
        return read<SN_TX_RD::Type>( socket_id, SN_TX_RD::OFFSET );
    }

    /**
     * \brief Read a socket's SN_TX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_WR register is to be read.
     *
     * \return The data read from the socket's SN_TX_WR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_tx_wr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_TX_WR::Type>( socket_id, SN_TX_WR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_TX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_TX_WR register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_TX_WR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_tx_wr( Socket_ID socket_id, SN_TX_WR::Type data ) noexcept
    {
        return write( socket_id, SN_TX_WR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RX_RSR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RSR register is to be read.
     *
     * \return The data read from the socket's SN_RX_RSR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_rx_rsr( Socket_ID socket_id ) const noexcept
    {
        return read_unstable<SN_RX_RSR::Type>( socket_id, SN_RX_RSR::OFFSET );
    }

    /**
     * \brief Read a socket's SN_RX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RD register is to be read.
     *
     * \return The data read from the socket's SN_RX_RD register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_rx_rd( Socket_ID socket_id ) const noexcept
    {
        return read<SN_RX_RD::Type>( socket_id, SN_RX_RD::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_RX_RD register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_RD register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_RX_RD register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_rx_rd( Socket_ID socket_id, SN_RX_RD::Type data ) noexcept
    {
        return write( socket_id, SN_RX_RD::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_RX_WR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_RX_WR register is to be read.
     *
     * \return The data read from the socket's SN_RX_WR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_rx_wr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_RX_WR::Type>( socket_id, SN_RX_WR::OFFSET );
    }

    /**
     * \brief Read a socket's SN_IMR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IMR register is to be read.
     *
     * \return The data read from the socket's SN_IMR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_imr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_IMR::Type>( socket_id, SN_IMR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_IMR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_IMR register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_IMR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_imr( Socket_ID socket_id, SN_IMR::Type data ) noexcept
    {
        return write( socket_id, SN_IMR::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_FRAG register.
     *
     * \param[in] socket_id The ID of the socket whose SN_FRAG register is to be read.
     *
     * \return The data read from the socket's SN_FRAG register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_frag( Socket_ID socket_id ) const noexcept
    {
        return read<SN_FRAG::Type>( socket_id, SN_FRAG::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_FRAG register.
     *
     * \param[in] socket_id The ID of the socket whose SN_FRAG register is to be written
     *            to.
     * \param[in] data The data to write to the socket's SN_FRAG register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_frag( Socket_ID socket_id, SN_FRAG::Type data ) noexcept
    {
        return write( socket_id, SN_FRAG::OFFSET, data );
    }

    /**
     * \brief Read a socket's SN_KPALVTR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_KPALVTR register is to be read.
     *
     * \return The data read from the socket's SN_KPALVTR register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_sn_kpalvtr( Socket_ID socket_id ) const noexcept
    {
        return read<SN_KPALVTR::Type>( socket_id, SN_KPALVTR::OFFSET );
    }

    /**
     * \brief Write to a socket's SN_KPALVTR register.
     *
     * \param[in] socket_id The ID of the socket whose SN_KPALVTR register is to be
     *            written to.
     * \param[in] data The data to write to the socket's SN_KPALVTR register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write_sn_kpalvtr( Socket_ID socket_id, SN_KPALVTR::Type data ) noexcept
    {
        return write( socket_id, SN_KPALVTR::OFFSET, data );
    }

    /**
     * \brief Read data from a socket's receive buffer.
     *
     * \param[in] socket_id The ID of the socket whose receive buffer is to be read.
     * \param[in] offset The offset of the buffer memory to read.
     * \param[out] begin The beginning of the data read from the buffer.
     * \param[out] end The end of the data read from the buffer.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const noexcept
    {
        return Communication_Controller::read( socket_id, Region::RX_BUFFER, offset, begin, end );
    }

    /**
     * \brief Write data to a socket's transmit buffer.
     *
     * \param[in] socket_id The ID of the socket whose transmit buffer is to be written
     *            to.
     * \param[in] offset The offset of the buffer memory to write to.
     * \param[in] begin The beginning of the data to write to the buffer.
     * \param[in] end The end of the data to write to the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        return Communication_Controller::write( socket_id, Region::TX_BUFFER, offset, begin, end );
    }

  private:
    /**
     * \brief Read a common register.
     *
     * \tparam Register The type of register to read.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    template<typename Register>
    auto read( std::uint16_t offset ) const noexcept
    {
        return read( offset, Register{} );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, std::uint8_t ) const noexcept
    {
        return Communication_Controller::read( offset );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, std::uint16_t ) const noexcept -> Result<std::uint16_t, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 2> buffer;

        auto result = Communication_Controller::read( offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::uint16_t>(
            ( buffer[ 0 ] << std::numeric_limits<std::uint8_t>::digits ) | buffer[ 1 ] );
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 4> const & ) const noexcept
        -> Result<Fixed_Size_Array<std::uint8_t, 4>, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 4> buffer;

        auto result = Communication_Controller::read( offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return buffer;
    }

    /**
     * \brief Read a common register.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 6> const & ) const noexcept
        -> Result<Fixed_Size_Array<std::uint8_t, 6>, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 6> buffer;

        auto result = Communication_Controller::read( offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return buffer;
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, std::uint8_t data ) noexcept
    {
        return Communication_Controller::write( offset, data );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, std::uint16_t data ) noexcept
    {
        auto const buffer = Fixed_Size_Array<std::uint8_t, 2>{
            static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( data ),
        };

        return Communication_Controller::write( offset, buffer.begin(), buffer.end() );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 4> const & data ) noexcept
    {
        return Communication_Controller::write( offset, data.begin(), data.end() );
    }

    /**
     * \brief Write to a common register.
     *
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 6> const & data ) noexcept
    {
        return Communication_Controller::write( offset, data.begin(), data.end() );
    }

    /**
     * \brief Read a socket register.
     *
     * \tparam Register The type of register to read.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    template<typename Register>
    auto read( Socket_ID socket_id, std::uint16_t offset ) const noexcept
    {
        return read( socket_id, offset, Register{} );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, std::uint16_t offset, std::uint8_t ) const noexcept
    {
        return Communication_Controller::read( socket_id, Region::REGISTERS, offset );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, std::uint16_t offset, std::uint16_t ) const noexcept
        -> Result<std::uint16_t, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 2> buffer;

        auto result = Communication_Controller::read(
            socket_id, Region::REGISTERS, offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<std::uint16_t>(
            ( buffer[ 0 ] << std::numeric_limits<std::uint8_t>::digits ) | buffer[ 1 ] );
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 4> const & ) const noexcept
        -> Result<Fixed_Size_Array<std::uint8_t, 4>, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 4> buffer;

        auto result = Communication_Controller::read(
            socket_id, Region::REGISTERS, offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return buffer;
    }

    /**
     * \brief Read a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 6> const & ) const noexcept
        -> Result<Fixed_Size_Array<std::uint8_t, 6>, Error_Code>
    {
        Fixed_Size_Array<std::uint8_t, 6> buffer;

        auto result = Communication_Controller::read(
            socket_id, Region::REGISTERS, offset, buffer.begin(), buffer.end() );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return buffer;
    }

    /**
     * \brief Read an unstable socket register.
     *
     * \tparam Register The type of register to read.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    template<typename Register>
    auto read_unstable( Socket_ID socket_id, std::uint16_t offset ) const noexcept
    {
        return read_unstable( socket_id, offset, Register{} );
    }

    /**
     * \brief Read an unstable socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be read.
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read_unstable( Socket_ID socket_id, std::uint16_t offset, std::uint16_t ) const noexcept
        -> Result<std::uint16_t, Error_Code>
    {
        // #lizard forgives the length

        std::uint16_t previous;

        {
            auto result = read<std::uint16_t>( socket_id, offset );
            if ( result.is_error() ) {
                return result.error();
            } // if

            previous = result.value();
        }

        for ( ;; ) {
            auto result = read<std::uint16_t>( socket_id, offset );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( result.value() == previous ) {
                return result.value();
            } // if

            previous = result.value();
        } // for
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be written to.
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, std::uint16_t offset, std::uint8_t data ) noexcept
    {
        return Communication_Controller::write( socket_id, Region::REGISTERS, offset, data );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be written to.
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, std::uint16_t offset, std::uint16_t data ) noexcept
    {
        auto const buffer = Fixed_Size_Array<std::uint8_t, 2>{
            static_cast<std::uint8_t>( data >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( data ),
        };

        return Communication_Controller::write(
            socket_id, Region::REGISTERS, offset, buffer.begin(), buffer.end() );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be written to.
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 4> const & data ) noexcept
    {
        return Communication_Controller::write(
            socket_id, Region::REGISTERS, offset, data.begin(), data.end() );
    }

    /**
     * \brief Write to a socket register.
     *
     * \param[in] socket_id The ID of the socket whose register is to be written to.
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, std::uint16_t offset, Fixed_Size_Array<std::uint8_t, 6> const & data ) noexcept
    {
        return Communication_Controller::write(
            socket_id, Region::REGISTERS, offset, data.begin(), data.end() );
    }
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H
