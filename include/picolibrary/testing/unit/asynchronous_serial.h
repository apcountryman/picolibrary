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
 * \brief Mock asynchronous serial basic transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Basic_Transmitter {
  public:
    /**
     * \brief The integral type used to hold the data to be transmitted.
     */
    using Data = Data_Type;

    /**
     * \brief Movable mock basic transmitter handle.
     */
    class Handle {
      public:
        /**
         * \brief The integral type used to hold the data to be transmitted.
         */
        using Data = Data_Type;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_basic_transmitter The mock basic transmitter.
         */
        Handle( Mock_Basic_Transmitter & mock_basic_transmitter ) noexcept :
            m_mock_basic_transmitter{ &mock_basic_transmitter }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept :
            m_mock_basic_transmitter{ source.m_mock_basic_transmitter }
        {
            source.m_mock_basic_transmitter = nullptr;
        }

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
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_basic_transmitter = expression.m_mock_basic_transmitter;

                expression.m_mock_basic_transmitter = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock basic transmitter.
         *
         * \return The mock basic transmitter.
         */
        auto & mock() noexcept
        {
            return *m_mock_basic_transmitter;
        }

        /**
         * \brief Initialize the transmitter's hardware.
         *
         * \return Nothing if transmitter hardware initialization succeeded.
         * \return An error code if transmitter hardware initialization failed.
         */
        auto initialize()
        {
            return m_mock_basic_transmitter->initialize();
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
            return m_mock_basic_transmitter->transmit( data );
        }

      private:
        /**
         * \brief The mock basic transmitter.
         */
        Mock_Basic_Transmitter * m_mock_basic_transmitter{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Basic_Transmitter() = default;

    Mock_Basic_Transmitter( Mock_Basic_Transmitter && ) = delete;

    Mock_Basic_Transmitter( Mock_Basic_Transmitter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Basic_Transmitter() noexcept = default;

    auto operator=( Mock_Basic_Transmitter && ) = delete;

    auto operator=( Mock_Basic_Transmitter const & ) = delete;

    /**
     * \brief Get a movable handle to the mock basic transmitter.
     *
     * \return A movable handle to the mock basic transmitter.
     */
    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, ( Data ) );
};

/**
 * \brief Mock asynchronous serial transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Transmitter {
  public:
    /**
     * \brief The integral type used to hold the data to be transmitted.
     */
    using Data = Data_Type;

    /**
     * \brief Movable mock transmitter handle.
     */
    class Handle {
      public:
        /**
         * \brief The integral type used to hold the data to be transmitted.
         */
        using Data = Data_Type;

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
        Handle( Handle && source ) noexcept :
            m_mock_transmitter{ source.m_mock_transmitter }
        {
            source.m_mock_transmitter = nullptr;
        }

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
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_transmitter = expression.m_mock_transmitter;

                expression.m_mock_transmitter = nullptr;
            } // if

            return *this;
        }

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
         * \return Nothing if transmitter hardware initialization succeeded.
         * \return An error code if transmitter hardware initialization failed.
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
        Mock_Transmitter * m_mock_transmitter{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Transmitter() = default;

    Mock_Transmitter( Mock_Transmitter && ) = delete;

    Mock_Transmitter( Mock_Transmitter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Transmitter() noexcept = default;

    auto operator=( Mock_Transmitter && ) = delete;

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
