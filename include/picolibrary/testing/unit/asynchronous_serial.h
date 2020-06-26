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
 * \brief picolibrary::Testing::Unit::Asynchronous_Serial interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H
#define PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"

/**
 * \brief Asynchronous serial unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Asynchronous_Serial {

/**
 * \brief Mock asynchronous serial transmitter.
 *
 * \tparam The integral type used to hold the data to be transmitted.
 */
template<typename Data_Type>
class Mock_Transmitter {
  public:
    /**
     * \copydoc picolibrary::Asynchronous_Serial::Transmitter_Concept::Data
     */
    using Data = Data_Type;

    /**
     * \brief Constructor.
     */
    Mock_Transmitter() = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Mock_Transmitter( Mock_Transmitter && source ) = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    Mock_Transmitter( Mock_Transmitter const & original ) = default;

    /**
     * \brief Destructor.
     */
    ~Mock_Transmitter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Mock_Transmitter && expression ) -> Mock_Transmitter &;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Mock_Transmitter const & expression ) -> Mock_Transmitter &;

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, ( Data ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit, (Data const *, Data const *));
};

} // namespace picolibrary::Testing::Unit::Asynchronous_Serial

#endif // PICOLIBRARY_TESTING_UNIT_ASYNCHRONOUS_SERIAL_H
