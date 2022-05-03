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
 * \brief picolibrary::Testing::Interactive::Asynchronous_Serial stream interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H

#include <utility>

#include "picolibrary/precondition.h"

namespace picolibrary::Testing::Interactive::Asynchronous_Serial {

/**
 * \brief Asynchronous serial output stream hello world interactive test helper.
 *
 * \tparam Output_Stream The type of output stream to use.
 * \tparam Transmitter The type of transmitter to use.
 *
 * \param[in] transmitter The transmitter to use.
 *
 * \pre writing to the stream succeeds
 */
template<template<typename> typename Output_Stream, typename Transmitter>
void hello_world( Transmitter transmitter ) noexcept
{
    transmitter.initialize();

    auto stream = Output_Stream{ std::move( transmitter ) };

    {
        auto result = stream.put( "Hello, world!\n" );
        expect( not result.is_error(), result.error() );
    }

    {
        auto result = stream.flush();
        expect( not result.is_error(), result.error() );
    }
}

} // namespace picolibrary::Testing::Interactive::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
