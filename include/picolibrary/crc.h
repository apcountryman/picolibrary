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
#include <limits>

#include "picolibrary/bit_manipulation.h"
#include "picolibrary/fixed_size_array.h"

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
 * \brief Get a calculation's input processor.
 *
 * \param[in] input_is_reflected Calculation input is reflected.
 *
 * \return The calculation's input processor.
 */
constexpr auto input_processor( bool input_is_reflected ) noexcept
{
    return input_is_reflected ? static_cast<Input_Processor>( reflect ) : []( std::uint8_t value ) noexcept
    {
        return value;
    };
}

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
 * \brief Get a calculation's output processor.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] output_is_reflected Calculation output is reflected.
 *
 * \return The calculation's output processor.
 */
template<typename Register>
constexpr auto output_processor( bool output_is_reflected ) noexcept
{
    return output_is_reflected ? static_cast<Output_Processor<Register>>( reflect )
                               : []( Register value ) noexcept
    {
        return value;
    };
}

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
     *         std::uint8_t.
     *
     * \param[in] begin The beginning of the message to perform the calculation on.
     * \param[in] end The end of the message to perform the calculation on.
     *
     * \return The CRC remainder for the message.
     */
    template<typename Iterator>
    auto calculate( Iterator begin, Iterator end ) const noexcept -> Register;
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
    constexpr explicit Bitwise_Calculator( Parameters<Register> const & parameters ) noexcept :
        m_polynomial{ parameters.polynomial },
        m_initial_remainder{ parameters.initial_remainder },
        m_process_input{ input_processor( parameters.input_is_reflected ) },
        m_process_output{ output_processor<Register>( parameters.output_is_reflected ) },
        m_xor_output{ parameters.xor_output }
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
     * \brief Calculate the CRC remainder for a message.
     *
     * \tparam Iterator Message iterator. The iterated over type must be convertible to a
     *         std::uint8_t.
     *
     * \param[in] begin The beginning of the message to perform the calculation on.
     * \param[in] end The end of the message to perform the calculation on.
     *
     * \return The CRC remainder for the message.
     */
    template<typename Iterator>
    auto calculate( Iterator begin, Iterator end ) const noexcept -> Register
    {
        auto const augment =
            Fixed_Size_Array<std::uint8_t, std::numeric_limits<Register>::digits / std::numeric_limits<std::uint8_t>::digits>{};

        return ( *m_process_output )( feed(
                   feed( m_initial_remainder, begin, end ), augment.begin(), augment.end() ) )
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
     * \brief Feed data into the CRC calculation.
     *
     * \tparam Iterator Message/augment iterator. The iterated over type must be
     *         convertible to a std::uint8_t.
     *
     * \param[in] remainder The current CRC calculation remainder.
     * \param[in] begin The beginning of the message/augment to feed into the calculation.
     * \param[in] end The end of the message/augment to feed into the calculation.
     *
     * \return The resulting CRC calculation remainder.
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

                if ( xor_polynomial ) { remainder ^= m_polynomial; } // if
            }                                                        // for
        }                                                            // for

        return remainder;
    }
};

/**
 * \brief Augmented byte lookup table calculator.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Augmented_Byte_Lookup_Table_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Augmented_Byte_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] parameters The calculation parameters.
     */
    constexpr explicit Augmented_Byte_Lookup_Table_Calculator( Parameters<Register> const & parameters ) noexcept
    {
        static_cast<void>( parameters );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Augmented_Byte_Lookup_Table_Calculator( Augmented_Byte_Lookup_Table_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Augmented_Byte_Lookup_Table_Calculator(
        Augmented_Byte_Lookup_Table_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Augmented_Byte_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Byte_Lookup_Table_Calculator && expression ) noexcept
        -> Augmented_Byte_Lookup_Table_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Byte_Lookup_Table_Calculator const & expression ) noexcept
        -> Augmented_Byte_Lookup_Table_Calculator & = default;
};

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
