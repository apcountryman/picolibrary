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
 * \brief picolibrary::Testing::Interactive stream interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_STREAM_H
#define PICOLIBRARY_TESTING_INTERACTIVE_STREAM_H

#include "picolibrary/rom.h"
#include "picolibrary/stream.h"

namespace picolibrary::Testing::Interactive {

/**
 * \brief Output stream hello world interactive test helper.
 *
 * \param[in] stream The output stream to write "Hello, world!\n" to.
 */
inline void hello_world( Reliable_Output_Stream & stream ) noexcept
{
    stream.put( PICOLIBRARY_ROM_STRING( "Hello, world!\n" ) );
    stream.flush();
}

} // namespace picolibrary::Testing::Interactive

#endif // PICOLIBRARY_TESTING_INTERACTIVE_STREAM_H
