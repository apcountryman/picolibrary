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
 * \brief picolibrary::Testing::Interactive::Asynchronous_Serial stream interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H

#include <utility>

#include "picolibrary/testing/interactive/stream.h"

namespace picolibrary::Testing::Interactive::Asynchronous_Serial {

/**
 * \brief Asynchronous serial reliable output stream hello world interactive test helper.
 *
 * \tparam Reliable_Output_Stream The type of reliable output stream to use.
 * \tparam Transmitter The type of transmitter to use.
 *
 * \param[in] transmitter The transmitter to use.
 */
template<template<typename> typename Reliable_Output_Stream, typename Transmitter>
void hello_world( Transmitter transmitter ) noexcept
{
    transmitter.initialize();

    auto stream = Reliable_Output_Stream{ std::move( transmitter ) };

    ::picolibrary::Testing::Interactive::hello_world( stream );
}

} // namespace picolibrary::Testing::Interactive::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ASYNCHRONOUS_SERIAL_STREAM_H
