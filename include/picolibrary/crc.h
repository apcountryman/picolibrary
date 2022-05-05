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
 * \brief picolibrary::CRC interface.
 */

#ifndef PICOLIBRARY_CRC_H
#define PICOLIBRARY_CRC_H

#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/bit_manipulation.h"

/**
 * \brief Cyclic Redundancy Check (CRC) facilities.
 */
namespace picolibrary::CRC {

/**
 * \brief Calculation parameters.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
struct Calculation_Parameters {
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Calculation polynomial.
     */
    Register polynomial;

    /**
     * \brief Calculation initial remainder.
     */
    Register initial_remainder;

    /**
     * \brief Calculation input is reflected.
     */
    bool input_is_reflected;

    /**
     * \brief Calculation output is reflected.
     */
    bool output_is_reflected;

    /**
     * \brief Calculation XOR output value.
     */
    Register xor_output;
};

/**
 * \brief Calculation input processor.
 *
 * \param[in] byte A calculation input byte.
 *
 * \return The calculation input byte or its reflection.
 */
using Input_Processor = auto ( * )( std::uint8_t byte ) -> std::uint8_t;

/**
 * \brief Get a calculation's input processor.
 *
 * \param[in] input_is_reflected Calculation input is reflected.
 *
 * \return The calculation's input processor.
 */
constexpr auto input_processor( bool input_is_reflected ) noexcept -> Input_Processor
{
    return input_is_reflected ? static_cast<Input_Processor>( reflect )
                              : []( std::uint8_t byte ) noexcept { return byte; };
}

/**
 * \brief Calculation output processor.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] remainder The calculation's remainder.
 *
 * \return The calculation's remainder or its reflection.
 */
template<typename Register>
using Output_Processor = auto ( * )( Register remainder ) -> Register;

/**
 * \brief Get a calculation's output processor.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] output_is_reflected Calculation output is reflected.
 *
 * \return The calculation's output processor.
 */
template<typename Register>
constexpr auto output_processor( bool output_is_reflected ) noexcept -> Output_Processor<Register>
{
    return output_is_reflected ? static_cast<Output_Processor<Register>>( reflect )
                               : []( Register remainder ) noexcept { return remainder; };
}

/**
 * \brief Calculation message augment.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
using Message_Augment =
    Array<std::uint8_t, std::numeric_limits<Register>::digits / std::numeric_limits<std::uint8_t>::digits>;

/**
 * \brief Calculation message augment.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
constexpr auto MESSAGE_AUGMENT = Message_Augment<Register>{};

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
