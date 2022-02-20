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
 * \brief picolibrary::Asynchronous_Serial stream interface.
 */

#ifndef PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H

#include "picolibrary/stream.h"

namespace picolibrary::Asynchronous_Serial {

/**
 * \brief Output stream concept.
 *
 * \tparam Transmitter The type of transmitter to abstract with the stream.
 */
template<typename Transmitter>
class Output_Stream_Concept : public Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Output_Stream_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream.
     */
    Output_Stream_Concept( Transmitter transmitter ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Output_Stream_Concept( Output_Stream_Concept && source ) noexcept;

    Output_Stream_Concept( Output_Stream_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Stream_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Stream_Concept && expression ) noexcept -> Output_Stream_Concept &;

    auto operator=( Output_Stream_Concept const & ) = delete;
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
