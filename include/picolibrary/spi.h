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
 * \brief picolibrary::SPI interface.
 */

#ifndef PICOLIBRARY_SPI_H
#define PICOLIBRARY_SPI_H

#include <cstdint>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Serial Peripheral Interface (SPI) facilities.
 */
namespace picolibrary::SPI {

/**
 * \brief SPI basic controller concept.
 */
class Basic_Controller_Concept {
  public:
    /**
     * \brief Clock (frequency, polarity, and phase), and data exchange bit order
     *        configuration.
     */
    struct Configuration {
    };

    /**
     * \brief Constructor.
     */
    Basic_Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Controller_Concept( Basic_Controller_Concept && source ) noexcept = default;

    Basic_Controller_Concept( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator                     =( Basic_Controller_Concept && expression ) noexcept
        -> Basic_Controller_Concept & = default;

    auto operator=( Basic_Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     *
     * \return Nothing if controller hardware initialization succeeded.
     * \return An error code if controller hardware initialization failed. If controller
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Configure the controller's clock, and data exchange bit order to meet a
     *        specific device's communication requirements.
     *
     * \param[in] configuration The clock, and data exchange bit order configuration that
     *            meets the device's communication requirements.
     *
     * \return Nothing if controller clock configuration succeeded.
     * \return An error code if controller clock configuration failed. If controller clock
     *         configuration cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto configure( Configuration configuration ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return The received data if data exchange succeeded.
     * \return An error code if data exchange failed. If data exchange cannot fail, return
     *         picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto exchange( std::uint8_t data ) noexcept -> Result<std::uint8_t, Error_Code>;
};

/**
 * \brief SPI controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Clock (frequency, polarity, and phase), and data exchange bit order
     *        configuration.
     */
    struct Configuration {
    };

    /**
     * \brief Constructor.
     */
    Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Controller_Concept( Controller_Concept && source ) noexcept = default;

    Controller_Concept( Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Controller_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Controller_Concept && expression ) noexcept -> Controller_Concept & = default;

    auto operator=( Controller_Concept const & ) = delete;

    /**
     * \brief Initialize the controller's hardware.
     *
     * \return Nothing if controller hardware initialization succeeded.
     * \return An error code if controller hardware initialization failed. If controller
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Configure the controller's clock, and data exchange bit order to meet a
     *        specific device's communication requirements.
     *
     * \param[in] configuration The clock, and data exchange bit order configuration that
     *            meets the device's communication requirements.
     *
     * \return Nothing if controller clock configuration succeeded.
     * \return An error code if controller clock configuration failed. If controller clock
     *         configuration cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto configure( Configuration configuration ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Exchange data with a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return The received data if data exchange succeeded.
     * \return An error code if data exchange failed. If data exchange cannot fail, return
     *         picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto exchange( std::uint8_t data ) noexcept -> Result<std::uint8_t, Error_Code>;

    /**
     * \brief Exchange a block of data with a device.
     *
     * \param[in] tx_begin The beginning of the block of data to transmit.
     * \param[in] tx_end The end of the block of data to transmit.
     * \param[out] rx_begin The beginning of the block of received data.
     * \param[out] rx_end The end of the block of received data.
     *
     * \warning This function may not verify that the transmit and receive data blocks are
     *          the same size.
     *
     * \return Nothing if data exchange succeeded.
     * \return An error code if data exchange failed. If data exchange cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) noexcept
        -> Result<Void, Error_Code>;

    /**
     * \brief Receive data from a device.
     *
     * \return The received data if data reception succeeded.
     * \return An error code if data reception failed. If data reception cannot fail,
     *         return picolibrary::Result<std::uint8_t, picolibrary::Void>.
     */
    auto receive() noexcept -> Result<std::uint8_t, ::picolibrary::Error_Code>;

    /**
     * \brief Receive a block of data from a device.
     *
     * \param[out] begin The beginning of the block of received data.
     * \param[out] end The end of the block of received data.
     *
     * \return Nothing if data reception succeeded.
     * \return An error code if data reception failed. If data reception cannot fail,
     *         return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit data to a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( std::uint8_t data ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>;
};

/**
 * \brief SPI controller.
 *
 * \tparam Basic_Controller The SPI basic controller to add SPI controller functionality
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
     * \param[in] tx_begin The beginning of the block of data to transmit.
     * \param[in] tx_end The end of the block of data to transmit.
     * \param[out] rx_begin The beginning of the block of received data.
     * \param[out] rx_end The end of the block of received data.
     *
     * \warning This function does not verify that the transmit and receive data blocks
     *          are the same size.
     *
     * \return Nothing if data exchange succeeded.
     * \return An error code if data exchange failed.
     */
    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) noexcept
    {
        static_cast<void>( tx_end );

        return generate(
            rx_begin, rx_end, [ & ]() noexcept { return exchange( *tx_begin++ ); } );
    }

    /**
     * \brief Receive data from a device.
     *
     * \return The received data if data reception succeeded.
     * \return An error code if data reception failed.
     */
    auto receive() noexcept
    {
        return exchange( 0x00 );
    }

    /**
     * \brief Receive a block of data from a device.
     *
     * \param[out] begin The beginning of the block of received data.
     * \param[out] end The end of the block of received data.
     *
     * \return Nothing if data reception succeeded.
     * \return An error code if data reception failed.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
    {
        return ::picolibrary::generate(
            begin, end, [ this ]() noexcept { return receive(); } );
    }

    /**
     * \brief Transmit data to a device.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed.
     */
    auto transmit( std::uint8_t data ) noexcept
        -> Result<Void, typename decltype( std::declval<Basic_Controller>().exchange( std::declval<std::uint8_t>() ) )::Error>
    {
        auto result = exchange( data );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Transmit a block of data to a device.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        return for_each<Discard_Functor>(
            begin, end, [ this ]( auto data ) noexcept { return transmit( data ); } );
    }
};

/**
 * \brief SPI device selector concept.
 */
class Device_Selector_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Device_Selector_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Device_Selector_Concept( Device_Selector_Concept && source ) noexcept = default;

    Device_Selector_Concept( Device_Selector_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Device_Selector_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator                    =( Device_Selector_Concept && expression ) noexcept
        -> Device_Selector_Concept & = default;

    auto operator=( Device_Selector_Concept const & ) = delete;

    /**
     * \brief Initialize the device selector's hardware.
     *
     * \return Nothing if device selector hardware initialization succeeded.
     * \return An error code if device selector hardware initialization failed. If device
     *         selector hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Select the device.
     *
     * \return Nothing if device selection succeeded.
     * \return An error code if device selection failed. If device selection cannot fail,
     *         return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto select() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Deselect the device.
     *
     * \return Nothing if device deselection succeeded.
     * \return An error code if device deselection failed. If device deselection cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto deselect() noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief GPIO output pin SPI device selector.
 *
 * \tparam GPIO_Output_Pin The type of GPIO output pin used to select a device.
 */
template<typename GPIO_Output_Pin>
class GPIO_Output_Pin_Device_Selector : public GPIO_Output_Pin {
  public:
    using GPIO_Output_Pin::GPIO_Output_Pin;

    /**
     * \brief Select the device.
     *
     * \return Nothing if device selection succeeded.
     * \return The error reported by the underlying pin if device selection failed.
     */
    auto select() noexcept
    {
        return GPIO_Output_Pin::transition_to_high();
    }

    /**
     * \brief Deselect the device.
     *
     * \return Nothing if device deselection succeeded.
     * \return The error reported by the underlying pin if device deselection failed.
     */
    auto deselect() noexcept
    {
        return GPIO_Output_Pin::transition_to_low();
    }
};

/**
 * \brief RAII SPI device selection guard.
 *
 * \tparam Device_Selector The type of SPI device selector used to select and deselect the
 *         device.
 *
 * \warning Device deselection failures are ignored. A device selector wrapper class can
 *          be used to add device selection failure error handling to the device
 *          selector's device deselection function.
 */
template<typename Device_Selector>
class Device_Selection_Guard;

/**
 * \brief Select a device and construct a picolibrary::SPI::Device_Selection_Guard.
 *
 * \relatedalso picolibrary::SPI::Device_Selection_Guard
 *
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         device.
 *
 * \param[in] device_selector The device selector used to select and deselect the device.
 *
 * \return The constructed picolibrary::SPI::Device_Selection_Guard if device selection
 *         succeeded.
 * \return The error reported by the device selector if device selection failed.
 */
template<typename Device_Selector>
auto make_device_selection_guard( Device_Selector & device_selector ) noexcept -> Result<
    Device_Selection_Guard<Device_Selector>,
    typename decltype( std::declval<Device_Selector>().select() )::Error,
    false>;

template<typename Device_Selector>
class Device_Selection_Guard {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Device_Selection_Guard() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Device_Selection_Guard( Device_Selection_Guard && source ) noexcept :
        m_device_selector{ source.m_device_selector }
    {
        source.m_device_selector = nullptr;
    }

    Device_Selection_Guard( Device_Selection_Guard const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Device_Selection_Guard() noexcept
    {
        if ( m_device_selector ) {
            static_cast<void>( m_device_selector->deselect() );
        } // if
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Device_Selection_Guard && expression ) noexcept
    {
        if ( &expression != this ) {
            if ( m_device_selector ) {
                static_cast<void>( m_device_selector->deselect() );
            } // if

            m_device_selector = expression.m_device_selector;

            expression.m_device_selector = nullptr;
        } // if

        return *this;
    }

    auto operator=( Device_Selection_Guard const & ) = delete;

  private:
    /**
     * \brief The device selector used to select and deselect the device.
     */
    Device_Selector * m_device_selector{};

    friend auto make_device_selection_guard<Device_Selector>( Device_Selector & device_selector ) noexcept
        -> Result<
            Device_Selection_Guard<Device_Selector>,
            typename decltype( std::declval<Device_Selector>().select() )::Error,
            false>;

    /**
     * \brief Constructor.
     *
     * \param[in] device_selector The device selector used to select and deselect the
     *            device.
     */
    constexpr Device_Selection_Guard( Device_Selector & device_selector ) noexcept :
        m_device_selector{ &device_selector }
    {
    }
};

template<typename Device_Selector>
auto make_device_selection_guard( Device_Selector & device_selector ) noexcept -> Result<
    Device_Selection_Guard<Device_Selector>,
    typename decltype( std::declval<Device_Selector>().select() )::Error,
    false>
{
    auto result = device_selector.select();
    if ( result.is_error() ) {
        return result.error();
    } // if

    return Device_Selection_Guard{ device_selector };
}

/**
 * \brief SPI device
 *
 * \tparam Controller_Type The type of controller used to communicate with the device.
 * \tparam Device_Selector_Type The type of device selector used to select and
 *         deselect the device.
 */
template<typename Controller_Type, typename Device_Selector_Type>
class Device {
  public:
    /**
     * \brief The type of controller used to communicate with the device.
     */
    using Controller = Controller_Type;

    /**
     * \brief The type of device selector used to select and deselect the device.
     */
    using Device_Selector = Device_Selector_Type;

    Device( Device const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Device() noexcept = default;

    auto operator=( Device const & ) = delete;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Device() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the device.
     * \param[in] configuration The controller clock, and data exchange bit order
     *            configuration that meets the device's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            device.
     */
    constexpr Device( Controller & controller, typename Controller::Configuration configuration, Device_Selector device_selector ) noexcept
        :
        m_controller{ &controller },
        m_configuration{ configuration },
        m_device_selector{ std::move( device_selector ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Device( Device && source ) noexcept :
        m_controller{ source.m_controller },
        m_configuration{ source.m_configuration },
        m_device_selector{ std::move( source.m_device_selector ) }
    {
        source.m_controller = nullptr;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto & operator=( Device && expression ) noexcept
    {
        if ( &expression != this ) {
            m_controller      = expression.m_controller;
            m_configuration   = expression.m_configuration;
            m_device_selector = std::move( expression.m_device_selector );

            expression.m_controller = nullptr;
        } // if

        return *this;
    }

    /**
     * \brief Initialize the device selector's hardware.
     *
     * \return Nothing if device selector hardware initialization succeeded.
     * \return The error reported by the device selector if device selector hardware
     *         initialization failed.
     */
    constexpr auto initialize() noexcept
    {
        return m_device_selector.initialize();
    }

    /**
     * \brief Configure the controller's clock, and data exchange bit order to meet the
     *        device's communication requirements.
     *
     * \return Nothing if controller clock configuration succeeded.
     * \return The error reported by the controller if controller clock configuration
     *         failed.
     */
    constexpr auto configure() const noexcept
    {
        return m_controller->configure( m_configuration );
    }

    /**
     * \brief Access the device's device selector.
     *
     * \return The device's device selector.
     */
    constexpr auto & device_selector() const noexcept
    {
        return m_device_selector;
    }

    /**
     * \brief Exchange data with the device.
     *
     * \param[in] data The data to transmit.
     *
     * \return The received data if data exchange succeeded.
     * \return The error reported by the controller if data exchange failed.
     */
    auto exchange( std::uint8_t data ) const noexcept
    {
        return m_controller->exchange( data );
    }

    /**
     * \brief Exchange a block of data with the device.
     *
     * \param[in] tx_begin The beginning of the block of data to transmit.
     * \param[in] tx_end The end of the block of data to transmit.
     * \param[out] rx_begin The beginning of the block of received data.
     * \param[out] rx_end The end of the block of received data.
     *
     * \warning This function may not verify that the transmit and receive data blocks are
     *          the same size.
     *
     * \return Nothing if data exchange succeeded.
     * \return The error reported by the controller if data exchange failed.
     */
    auto exchange( std::uint8_t const * tx_begin, std::uint8_t const * tx_end, std::uint8_t * rx_begin, std::uint8_t * rx_end ) const noexcept
    {
        return m_controller->exchange( tx_begin, tx_end, rx_begin, rx_end );
    }

    /**
     * \brief Receive data from the device.
     *
     * \return The received data if data reception succeeded.
     * \return The error reported by the controller if data reception failed.
     */
    auto receive() const noexcept
    {
        return m_controller->receive();
    }

    /**
     * \brief Receive a block of data from the device.
     *
     * \param[out] begin The beginning of the block of received data.
     * \param[out] end The end of the block of received data.
     *
     * \return Nothing if data reception succeeded.
     * \return The error reported by the controller if data reception failed.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) const noexcept
    {
        return m_controller->receive( begin, end );
    }

    /**
     * \brief Transmit data to the device.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return The error reported by the controller if data transmission failed.
     */
    auto transmit( std::uint8_t data ) const noexcept
    {
        return m_controller->transmit( data );
    }

    /**
     * \brief Transmit a block of data to the device.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return The error reported by the controller if data transmission failed.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) const noexcept
    {
        return m_controller->transmit( begin, end );
    }

  private:
    /**
     * \brief The controller used to communicate with the device.
     */
    Controller * m_controller{};

    /**
     * \brief The controller clock, and data exchange bit order configuration that meets
     *        the device's communication requirements.
     */
    typename Controller::Configuration m_configuration{};

    /**
     * \brief The device selector used to select and deselect the device.
     */
    Device_Selector mutable m_device_selector{};
};

} // namespace picolibrary::SPI

#endif // PICOLIBRARY_SPI_H
