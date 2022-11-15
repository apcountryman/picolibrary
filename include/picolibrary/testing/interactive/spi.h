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
 * \brief picolibrary::Testing::Interactive::SPI interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_SPI_H
#define PICOLIBRARY_TESTING_INTERACTIVE_SPI_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/format.h"
#include "picolibrary/precondition.h"
#include "picolibrary/stream.h"

/**
 * \brief Serial Peripheral Interface (SPI) interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::SPI {

/**
 * \brief Controller echo interactive test helper.
 *
 * \tparam Controller The type of controller to test.
 * \tparam Delayer A nullary functor called to introduce a delay each time data is
 *         exchanged.
 *
 * \param[in] stream The output stream to write test output to.
 * \param[in] controller The controller to test.
 * \param[in] configuration The clock and data exchange bit order configuration to use for
 *            the test.
 * \param[in] delay The nullary functor to call to introduce a delay each time data is
 *            exchanged.
 */
template<typename Controller, typename Delayer>
[[noreturn]] void echo(
    Reliable_Output_Stream &                   stream,
    Controller                                 controller,
    typename Controller::Configuration const & configuration,
    Delayer                                    delay ) noexcept
{
    controller.initialize();
    controller.configure( configuration );

    for ( auto tx = std::uint8_t{};; ++tx ) {
        delay();

        auto const rx = controller.exchange( tx );
        stream.print( "exchange( ", Format::Hex{ tx }, " ) -> ", Format::Hex{ rx }, '\n' );
        stream.flush();
        expect( rx == tx, Generic_Error::RUNTIME_ERROR );
    } // for
}

} // namespace picolibrary::Testing::Interactive::SPI

#endif // PICOLIBRARY_TESTING_INTERACTIVE_SPI_H
