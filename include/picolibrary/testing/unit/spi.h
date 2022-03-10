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
 * \brief picolibrary::Testing::Unit::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_SPI_H
#define PICOLIBRARY_TESTING_UNIT_SPI_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/testing/unit/mock_handle.h"

/**
 * \brief Serial Peripheral Interface (SPI) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::SPI {

/**
 * \brief Mock basic controller.
 */
class Mock_Basic_Controller {
  public:
    using Configuration = std::uint_fast8_t;

    class Handle : public Mock_Handle<Mock_Basic_Controller> {
      public:
        using Configuration = Mock_Basic_Controller::Configuration;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Basic_Controller & mock ) noexcept :
            Mock_Handle<Mock_Basic_Controller>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        void initialize()
        {
            Mock_Handle<Mock_Basic_Controller>::mock().initialize();
        }

        void configure( Configuration const & configuration )
        {
            Mock_Handle<Mock_Basic_Controller>::mock().configure( configuration );
        }

        auto exchange( std::uint8_t data )
        {
            return Mock_Handle<Mock_Basic_Controller>::mock().exchange( data );
        }
    };

    Mock_Basic_Controller() = default;

    Mock_Basic_Controller( Mock_Basic_Controller && ) = delete;

    Mock_Basic_Controller( Mock_Basic_Controller const & ) = delete;

    ~Mock_Basic_Controller() noexcept = default;

    auto operator=( Mock_Basic_Controller && ) = delete;

    auto operator=( Mock_Basic_Controller const & ) = delete;

    auto handle() noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, initialize, () );

    MOCK_METHOD( void, configure, (Configuration const &));

    MOCK_METHOD( std::uint8_t, exchange, ( std::uint8_t ) );
};

} // namespace picolibrary::Testing::Unit::SPI

#endif // PICOLIBRARY_TESTING_UNIT_SPI_H
