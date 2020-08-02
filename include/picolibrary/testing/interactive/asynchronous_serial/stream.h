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
 * \brief picolibrary::Testing::Interactive::Asynchronous_Serial::Stream interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H

#include <utility>

/**
 * \brief Asynchronous serial stream interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Asynchronous_Serial::Stream {

/**
 * \brief Construct an asynchronous serial output stream, initialize the asynchronous
 *        serial transmitter abstracted by the stream, and output "Hello, world!\n".
 *
 * \tparam Output_Stream The type of output stream to use.
 * \tparam Transmitter The type of transmitter to use.
 *
 * \param[in] transmitter The transmitter to use.
 */
template<template<typename> typename Output_Stream, typename Transmitter>
void hello_world( Transmitter transmitter ) noexcept
{
    auto stream = Output_Stream{ std::move( transmitter ) };

    if ( stream.initialize().is_error() ) { return; } // if

    static_cast<void>( stream.put( "Hello, world!\n" ) );
}

} // namespace picolibrary::Testing::Interactive::Asynchronous_Serial::Stream

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
