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
    Calculator_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] calculation_parameters The calculation parameters.
     */
    Calculator_Concept( Calculation_Parameters<Register> const & calculation_parameters ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Calculator_Concept( Calculator_Concept && source ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    Calculator_Concept( Calculator_Concept const & original ) noexcept;

    /**
     * \brief Destructor.
     */
    ~Calculator_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Calculator_Concept && expression ) noexcept -> Calculator_Concept &;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Calculator_Concept const & expression ) noexcept -> Calculator_Concept &;

    /**
     * \brief Calculate the remainder for a message.
     *
     * \tparam Iterator Message iterator. The iterated over type must be convertible to
     *         std::uint8_t.
     *
     * \param[in] begin The beginning of the message.
     * \param[in] end The end of the message.
     *
     * \return The remainder for the message.
     */
    template<typename Iterator>
    auto calculate( Iterator begin, Iterator end ) const noexcept -> Register;
};

/**
 * \brief Bitwise calculator.
 *
 * This calculator implementation processes message one bit at a time, and requires a
 * message augment to push the entirety of the message through the calculation. While this
 * results in lower performance than table driven implementations, memory use is lower due
 * to the lack of a lookup table.
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
     * \param[in] calculation_parameters The calculation parameters.
     */
    constexpr Bitwise_Calculator( Calculation_Parameters<Register> const & calculation_parameters ) noexcept :
        m_polynomial{ calculation_parameters.polynomial },
        m_initial_remainder{ calculation_parameters.initial_remainder },
        m_process_input{ input_processor( calculation_parameters.input_is_reflected ) },
        m_process_output{ output_processor<Register>( calculation_parameters.output_is_reflected ) },
        m_xor_output{ calculation_parameters.xor_output }
    {
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

    /**
     * \brief Calculate the remainder for a message.
     *
     * \tparam Iterator Message iterator. The iterated over type must be convertible to
     *         std::uint8_t.
     *
     * \param[in] begin The beginning of the message.
     * \param[in] end The end of the message.
     *
     * \return The remainder for the message.
     */
    template<typename Iterator>
    constexpr auto calculate( Iterator begin, Iterator end ) const noexcept -> Register
    {
        return ( *m_process_output )( feed(
                   feed( m_initial_remainder, begin, end ),
                   MESSAGE_AUGMENT<Register>.begin(),
                   MESSAGE_AUGMENT<Register>.end() ) )
               ^ m_xor_output;
    }

  private:
    /**
     * \brief Calculation polynomial.
     */
    Register m_polynomial{};

    /**
     * \brief Calculation initial remainder.
     */
    Register m_initial_remainder{};

    /**
     * \brief Calculation input processor.
     */
    Input_Processor m_process_input{};

    /**
     * \brief Calculation output processor.
     */
    Output_Processor<Register> m_process_output{};

    /**
     * \brief Calculation XOR output value.
     */
    Register m_xor_output{};

    /**
     * \brief Feed data into the calculation.
     *
     * \tparam Iterator Message / message augment iterator. The iterated over type must be
     *         convertible to std::uint8_t.
     *
     * \param[in] remainder The current calculation remainder.
     * \param[in] begin The beginning of the message / message augment.
     * \param[in] end The end of the message / message augment.
     *
     * \return The new calculation remainder.
     */
    template<typename Iterator>
    auto feed( Register remainder, Iterator begin, Iterator end ) const noexcept
    {
        for ( ; begin != end; ++begin ) {
            auto const processed_input = ( *m_process_input )( *begin );

            for ( auto bit = std::numeric_limits<std::uint8_t>::digits - 1; bit >= 0; --bit ) {
                auto const xor_polynomial = static_cast<bool>(
                    remainder & ~( std::numeric_limits<Register>::max() >> 1 ) );

                remainder = ( remainder << 1 ) | ( ( processed_input >> bit ) & 0b1 );

                if ( xor_polynomial ) {
                    remainder ^= m_polynomial;
                } // if
            }     // for
        }         // for

        return remainder;
    }
};

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
