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
     * \brief Read a common register.
     *
     * \param[in] offset The offset of the register to read.
     *
     * \return The data read from the register if the read succeeded.
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
     * \brief Read a block of common register.
     *
     * \param[in] offset The offset of the block of registers to read.
     * \param[in] begin The beginning of the data read from the block of registers.
     * \param[in] end The end of the data read from the block of registers.
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
     * \brief Write to a common register.
     *
     * \param[in] offset The offset of the register to write to.
     * \param[in] data The data to write to the register.
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
     * \brief Write to a block of common registers.
     *
     * \param[in] offset The offset of the block of registers to write to.
     * \param[in] begin The beginning of the data to write to the block of registers.
     * \param[in] end The end of the data to write to the block of registers.
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

  private:
    /**
     * \brief SPI communication frame.
     */
    using Frame = Fixed_Size_Array<std::uint8_t, 3>;

    /**
     * \brief Construct an SPI communication frame for accessing common registers.
     *
     * \param[in] offset The offset of the register or block of registers to be accessed.
     * \param[in] operation The operation to be performed.
     *
     * \return The SPI communication frame.
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
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_H
