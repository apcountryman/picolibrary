/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Testing::Unit::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H

#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"

/**
 * \brief Asynchronous serial unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Asynchronous_Serial {

/**
 * \brief Mock asynchronous serial transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Transmitter {
  public:
    /**
     * \copydoc picolibrary::Asynchronous_Serial::Transmitter_Concept::Data
     */
    using Data = Data_Type;

    /**
     * \brief Movable mock transmitter handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_transmitter The mock transmitter.
         */
        Handle( Mock_Transmitter & mock_transmitter ) noexcept :
            m_mock_transmitter{ &mock_transmitter }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept = default;

        /**
         * \todo #29
         */
        Handle( Handle const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock transmitter.
         *
         * \return The mock transmitter.
         */
        auto & mock() noexcept
        {
            return *m_mock_transmitter;
        }

        /**
         * \brief Initialize the transmitter's hardware.
         *
         * \return Nothing if initializing the transmitter's hardware succeeded.
         * \return An error code if initializing the transmitter's hardware failed.
         */
        auto initialize()
        {
            return m_mock_transmitter->initialize();
        }

        /**
         * \brief Transmit data.
         *
         * \param[in] data The data to transmit.
         *
         * \return Nothing if data transmission succeeded.
         * \return An error code if data transmission failed.
         */
        auto transmit( Data data )
        {
            return m_mock_transmitter->transmit( data );
        }

        /**
         * \brief Transmit a block of data.
         *
         * \param[in] begin The beginning of the block of data to transmit.
         * \param[in] end The end of the block of data to transmit.
         *
         * \return Nothing if data transmission succeeded.
         * \return An error code if data transmission failed.
         */
        auto transmit( Data const * begin, Data const * end )
        {
            return m_mock_transmitter->transmit( begin, end );
        }

      private:
        /**
         * \brief The mock transmitter.
         */
        Mock_Transmitter * m_mock_transmitter;
    };

    /**
     * \brief Constructor.
     */
    Mock_Transmitter() = default;

    /**
     * \todo #29
     */
    Mock_Transmitter( Mock_Transmitter && ) = delete;

    /**
     * \todo #29
     */
    Mock_Transmitter( Mock_Transmitter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Transmitter() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Transmitter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Transmitter const & ) = delete;

    /**
     * \brief Get a movable handle to the mock transmitter.
     *
     * \return A movable handle to the mock transmitter.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, ( Data ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, (std::vector<Data>));

    /**
     * \brief Transmit a block of data.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed.
     */
    auto transmit( Data const * begin, Data const * end ) noexcept
    {
        return transmit( std::vector<Data>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H
