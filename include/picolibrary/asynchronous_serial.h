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
 * \brief picolibrary::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_ASYNCHRONOUS_SERIAL_H

#include <cstdint>

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

    /**
     * \brief Constructor.
     */
    Basic_Transmitter_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Basic_Transmitter_Concept( Basic_Transmitter_Concept && source ) noexcept = default;

    Basic_Transmitter_Concept( Basic_Transmitter_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Basic_Transmitter_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Basic_Transmitter_Concept && expression ) noexcept
        -> Basic_Transmitter_Concept & = default;

    auto operator=( Basic_Transmitter_Concept const & ) = delete;

    /**
     * \brief Initialize the transmitter's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Transmit data.
     *
     * \param[in] data The data to transmit.
     */
    void transmit( Data data ) noexcept;
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

    /**
     * \brief Constructor.
     */
    Transmitter_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Transmitter_Concept( Transmitter_Concept && source ) noexcept = default;

    Transmitter_Concept( Transmitter_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Transmitter_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Transmitter_Concept && expression ) noexcept -> Transmitter_Concept & = default;

    auto operator=( Transmitter_Concept const & ) = delete;

    /**
     * \brief Initialize the transmitter's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Transmit data.
     *
     * \param[in] data The data to transmit.
     */
    void transmit( Data data ) noexcept;

    /**
     * \brief Transmit a block of data.
     *
     * \param[in] begin The beginning of the block of data to transmit.
     * \param[in] end The end of the block of data to transmit.
     */
    void transmit( Data const * begin, Data const * end ) noexcept;
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_H
