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
 * \brief picolibrary::CRC interface.
 */

#ifndef PICOLIBRARY_CRC_H
#define PICOLIBRARY_CRC_H

#include <cstdint>

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
struct Parameters {
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
 * \return The input byte or its reflection.
 */
using Input_Processor = auto ( * )( std::uint8_t byte ) -> std::uint8_t;

/**
 * \brief Calculation output processor.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] remainder The remainder of the calculation.
 *
 * \return The remainder of the calculation or its reflection.
 */
template<typename Register>
using Output_Processor = auto ( * )( Register remainder ) -> Register;

/**
 * \brief Calculator implementation selector.
 */
enum class Implementation : std::uint_fast8_t {};

/**
 * \brief Calculator.
 *
 * \tparam IMPLEMENTATION The calculator implementation to use.
 * \tparam Register_Type Calculator register type.
 */
template<Implementation IMPLEMENTATION, typename Register_Type>
class Calculator;

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
