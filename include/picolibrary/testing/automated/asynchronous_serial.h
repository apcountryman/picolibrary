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
 * \brief picolibrary::Testing::Automated::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_TESTING_AUTOMATED_ASYNCHRONOUS_SERIAL_H

#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/testing/automated/mock_handle.h"

/**
 * \brief Asynchronous serial automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Asynchronous_Serial {

/**
 * \brief Mock asynchronous serial basic transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Basic_Transmitter {
  public:
    using Data = Data_Type;

    class Handle : public Mock_Handle<Mock_Basic_Transmitter> {
      public:
        using Data = Mock_Basic_Transmitter::Data;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Basic_Transmitter & mock ) noexcept :
            Mock_Handle<Mock_Basic_Transmitter>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void initialize()
        {
            Mock_Handle<Mock_Basic_Transmitter>::mock().initialize();
        }

        void transmit( Data data )
        {
            Mock_Handle<Mock_Basic_Transmitter>::mock().transmit( data );
        }
    };

    Mock_Basic_Transmitter() = default;

    Mock_Basic_Transmitter( Mock_Basic_Transmitter && ) = delete;

    Mock_Basic_Transmitter( Mock_Basic_Transmitter const & ) = delete;

    ~Mock_Basic_Transmitter() noexcept = default;

    auto operator=( Mock_Basic_Transmitter && ) = delete;

    auto operator=( Mock_Basic_Transmitter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, transmit, ( Data ) );
};

/**
 * \brief Mock asynchronous serial transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Transmitter : public Mock_Basic_Transmitter<Data_Type> {
  public:
    using Data = typename Mock_Basic_Transmitter<Data_Type>::Data;

    class Handle : public Mock_Basic_Transmitter<Data_Type>::Handle {
      public:
        using Data = typename Mock_Basic_Transmitter<Data_Type>::Handle::Data;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Transmitter & mock ) noexcept :
            Mock_Basic_Transmitter<Data_Type>::Handle{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto mock() noexcept -> Mock_Transmitter &
        {
            return static_cast<Mock_Transmitter &>( Mock_Basic_Transmitter<Data_Type>::Handle::mock() );
        }

        auto mock() const noexcept -> Mock_Transmitter const &
        {
            return static_cast<Mock_Transmitter const &>(
                Mock_Basic_Transmitter<Data_Type>::Handle::mock() );
        }

        using Mock_Basic_Transmitter<Data_Type>::Handle::transmit;

        void transmit( Data const * begin, Data const * end )
        {
            mock().transmit( begin, end );
        }
    };

    Mock_Transmitter() = default;

    Mock_Transmitter( Mock_Transmitter && ) = delete;

    Mock_Transmitter( Mock_Transmitter const & ) = delete;

    ~Mock_Transmitter() noexcept = default;

    auto operator=( Mock_Transmitter && ) = delete;

    auto operator=( Mock_Transmitter const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    using Mock_Basic_Transmitter<Data_Type>::transmit;
    using Mock_Basic_Transmitter<Data_Type>::gmock_transmit;

    MOCK_METHOD( void, transmit, (std::vector<Data>));

    void transmit( Data const * begin, Data const * end )
    {
        return transmit( std::vector<Data>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Automated::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_AUTOMATED_ASYNCHRONOUS_SERIAL_H
