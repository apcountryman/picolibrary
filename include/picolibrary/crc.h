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
 * \brief Calculator concept.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Calculator_Concept {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    Calculator_Concept() noexcept = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] parameters The calculation parameters.
     */
    explicit Calculator_Concept( Parameters<Register> const & parameters ) noexcept = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Calculator_Concept( Calculator_Concept && source ) noexcept = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    Calculator_Concept( Calculator_Concept const & original ) noexcept = delete;

    /**
     * \brief Destructor.
     */
    ~Calculator_Concept() noexcept = delete;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Calculator_Concept && expression ) noexcept -> Calculator_Concept & = delete;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Calculator_Concept const & expression ) noexcept -> Calculator_Concept & = delete;

    /**
     * \brief Calculate the CRC remainder for a message.
     *
     * \tparam Iterator Message iterator. The iterated over type must be convertible to a
     *         std::uint8_t, and the conversion must not be a narrowing conversion.
     *
     * \param[in] begin The beginning of the message to perform the calculation on.
     * \param[in] end The end of the message to perform the calculation on.
     *
     * \return The CRC remainder for the message.
     */
    template<typename Iterator>
    auto calculate( Iterator begin, Iterator end ) noexcept -> Register;
};

/**
 * \brief Bitwise calculator.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Bitwise_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Bitwise_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] parameters The calculation parameters.
     */
    constexpr explicit Bitwise_Calculator( Parameters<Register> const & parameters ) noexcept
    {
        static_cast<void>( parameters );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Bitwise_Calculator( Bitwise_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Bitwise_Calculator( Bitwise_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Bitwise_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator     =( Bitwise_Calculator && expression ) noexcept
        -> Bitwise_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator     =( Bitwise_Calculator const & expression ) noexcept
        -> Bitwise_Calculator & = default;
};

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
