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
 * \brief picolibrary::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_ASYNCHRONOUS_SERIAL_H

#include <cstdint>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"

/**
 * \brief Asynchronous serial facilities.
 */
namespace picolibrary::Asynchronous_Serial {

/**
 * \brief Asynchronous serial basic transmitter concept.
 */
class Basic_Transmitter_Concept {
  public:
    /**
     * \brief The integral type used to hold the data to be transmitted.
     */
    using Data = std::uint8_t;

    Basic_Transmitter_Concept() = delete;

    Basic_Transmitter_Concept( Basic_Transmitter_Concept && ) = delete;

    Basic_Transmitter_Concept( Basic_Transmitter_Concept const & ) = delete;

    ~Basic_Transmitter_Concept() = delete;

    auto operator=( Basic_Transmitter_Concept && ) = delete;

    auto operator=( Basic_Transmitter_Concept const & ) = delete;

    /**
     * \brief Initialize the transmitter's hardware.
     *
     * \return Nothing if transmitter hardware initialization succeeded.
     * \return An error code if transmitter hardware initialization failed. If transmitter
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit data.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( Data data ) noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief Asynchronous serial transmitter concept.
 */
class Transmitter_Concept {
  public:
    /**
     * \brief The integral type used to hold the data to be transmitted.
     */
    using Data = std::uint8_t;

    Transmitter_Concept() = delete;

    Transmitter_Concept( Transmitter_Concept && ) = delete;

    Transmitter_Concept( Transmitter_Concept const & ) = delete;

    ~Transmitter_Concept() = delete;

    auto operator=( Transmitter_Concept && ) = delete;

    auto operator=( Transmitter_Concept const & ) = delete;

    /**
     * \brief Initialize the transmitter's hardware.
     *
     * \return Nothing if transmitter hardware initialization succeeded.
     * \return An error code if transmitter hardware initialization failed. If transmitter
     *         hardware initialization cannot fail, return
     *         picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit data.
     *
     * \param[in] data The data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( Data data ) noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Transmit a block of data.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     *
     * \return Nothing if data transmission succeeded.
     * \return An error code if data transmission failed. If data transmission cannot
     *         fail, return picolibrary::Result<picolibrary::Void, picolibrary::Void>.
     */
    auto transmit( Data const * begin, Data const * end ) noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief Asynchronous serial transmitter.
 *
 * \tparam Basic_Transmitter The asynchronous serial basic transmitter to add asynchronous
 *         serial transmitter functionality to.
 */
template<typename Basic_Transmitter>
class Transmitter : public Basic_Transmitter {
  public:
    /**
     * \brief The integral type used to hold the data to be transmitted.
     */
    using Data = typename Basic_Transmitter::Data;

    using Basic_Transmitter::Basic_Transmitter;

    using Basic_Transmitter::transmit;

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
        return for_each<Discard_Functor>(
            begin, end, [this]( auto data ) noexcept { return transmit( data ); } );
    }
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_H
