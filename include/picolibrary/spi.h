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
 * \brief picolibrary::SPI interface.
 */

#ifndef PICOLIBRARY_SPI_H
#define PICOLIBRARY_SPI_H

#include <cstdint>

#include "picolibrary/algorithm.h"

/**
 * \brief Serial Peripheral Interface (SPI) facilities.
 */
namespace picolibrary::SPI {

/**
 * \brief Basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Clock (frequency, polarity, and phase) and data exchange bit order
     *        configuration.
     */
    struct Configuration {
    };

    /**
     * \brief Constructor.
     */
    Basic_Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Controller_Concept( Basic_Controller_Concept && source ) noexcept;

    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Basic_Controller_Concept && expression ) noexcept -> Basic_Controller_Concept &;

    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Configure the controller's clock and data exchange bit order to meet a
     *        specific device's communication requirements.
     *
     * \param[in] configuration The clock and data exchange bit order configuration that
     *            meets the device's communication requirements.
     */
    void configure( Configuration const & configuration ) noexcept;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit to the device.
     *
     * \return The data received from the device.
     */
    auto exchange( std::uint8_t data ) noexcept -> std::uint8_t;
};

/**
 * \brief Controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Clock (frequency, polarity, and phase) and data exchange bit order
     *        configuration.
     */
    struct Configuration {
    };

    /**
     * \brief Constructor.
     */
    Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Controller_Concept( Controller_Concept && source ) noexcept;

    Controller_Concept( Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Controller_Concept && expression ) noexcept -> Controller_Concept &;

    auto operator=( Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Configure the controller's clock and data exchange bit order to meet a
     *        specific device's communication requirements.
     *
     * \param[in] configuration The clock and data exchange bit order configuration that
     *            meets the device's communication requirements.
     */
    void configure( Configuration const & configuration ) noexcept;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit to the device.
     *
     * \return The data received from the device.
     */
    auto exchange( std::uint8_t data ) noexcept -> std::uint8_t;

    /**
     * \brief Exchange a block of data with a device.
     *
     * \param[in] tx_begin The beginning of the block of data to transmit to the device.
     * \param[in] tx_end The end of the block of data to transmit to the device.
     * \param[out] rx_begin The beginning of the block of data received from the device.
     * \param[out] rx_end The end of the block of data received from the device.
     *
     * \warning Implementations of this function may not verify that the transmit and
     *          receive data blocks are the same size.
     */
    void exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) noexcept;

    /**
     * \brief Receive data from a device.
     *
     * \return The data received from the device.
     */
    auto receive() noexcept -> std::uint8_t;

    /**
     * \brief Receive a block of data from a device.
     *
     * \param[out] begin The beginning of the block of data received from the device.
     * \param[out] end The end of the block of data received from the device.
     */
    void receive( std::uint8_t * begin, std::uint8_t * end ) noexcept;

    /**
     * \brief Transmit data to a device.
     *
     * \param[in] data The data to transmit to the device.
     */
    void transmit( std::uint8_t data ) noexcept;

    /**
     * \brief Transmit a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to transmit to the device.
     * \param[in] end The end of the block of data to transmit to the device.
     */
    void transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept;
};

/**
 * \brief Controller.
 *
 * \tparam Basic_Controller The type of basic controller to add controller functionality
 *         to.
 */
template<typename Basic_Controller>
class Controller : public Basic_Controller {
  public:
    using Basic_Controller::Basic_Controller;

    using Basic_Controller::exchange;

    /**
     * \brief Exchange a block of data with a device.
     *
     * \param[in] tx_begin The beginning of the block of data to transmit to the device.
     * \param[in] tx_end The end of the block of data to transmit to the device.
     * \param[out] rx_begin The beginning of the block of data received from the device.
     * \param[out] rx_end The end of the block of data received from the device.
     *
     * \warning This function does not verify that the transmit and receive data blocks
     *          are the same size.
     */
    void exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) noexcept
    {
        static_cast<void>( rx_end );

        for ( ; tx_begin != tx_end; ++tx_begin, ++rx_begin ) {
            *rx_begin = exchange( *tx_begin );
        } // for
    }

    /**
     * \brief Receive data from a device.
     *
     * \return The data received from the device.
     */
    auto receive() noexcept
    {
        return exchange( 0x00 );
    }

    /**
     * \brief Receive a block of data from a device.
     *
     * \param[out] begin The beginning of the block of data received from the device.
     * \param[out] end The end of the block of data received from the device.
     */
    void receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
    {
        ::picolibrary::generate( begin, end, [ this ]() noexcept { return receive(); } );
    }

    /**
     * \brief Transmit data to a device.
     *
     * \param[in] data The data to transmit to the device.
     */
    void transmit( std::uint8_t data ) noexcept
    {
        exchange( data );
    }

    /**
     * \brief Transmit a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to transmit to the device.
     * \param[in] end The end of the block of data to transmit to the device.
     */
    void transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        ::picolibrary::for_each(
            begin, end, [ this ]( auto data ) noexcept { transmit( data ); } );
    }
};

/**
 * \brief Device selector concept.
 */
class Device_Selector_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Device_Selector_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Device_Selector_Concept( Device_Selector_Concept && source ) noexcept;

    Device_Selector_Concept( Device_Selector_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Device_Selector_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Device_Selector_Concept && expression ) noexcept -> Device_Selector_Concept &;

    auto operator=( Device_Selector_Concept const & ) = delete;

    /**
     * \brief Initialize the device selector's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Select the device.
     */
    void select() noexcept;

    /**
     * \brief Deselect the device.
     */
    void deselect() noexcept;
};

} // namespace picolibrary::SPI

#endif // PICOLIBRARY_SPI_H
