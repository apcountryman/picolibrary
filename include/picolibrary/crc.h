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
 * \brief picolibrary::CRC interface.
 */

#ifndef PICOLIBRARY_CRC_H
#define PICOLIBRARY_CRC_H

#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/bit_manipulation.h"

/**
 * \brief CRC facilities.
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
 * This calculator implementation processes messages one bit at a time, and requires a
 * message augment to push the entirety of a message through the calculation. While this
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
        auto const message_augment = Message_Augment<Register>{};

        return ( *m_process_output )( feed(
                   feed( m_initial_remainder, begin, end ),
                   message_augment.begin(),
                   message_augment.end() ) )
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

/**
 * \brief Byte indexed lookup table.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
using Byte_Indexed_Lookup_Table = Array<Register, std::numeric_limits<std::uint8_t>::max() + 1>;

/**
 * \brief Generate a byte indexed lookup table.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] polynomial Calculation polynomial.
 *
 * \return The generated byte indexed lookup table.
 */
template<typename Register>
// NOLINTNEXTLINE(readability-function-size)
constexpr auto generate_byte_indexed_lookup_table( Register polynomial ) noexcept
    -> Byte_Indexed_Lookup_Table<Register>
{
    // #lizard forgives the length

    Byte_Indexed_Lookup_Table<Register> lookup_table;

    for ( auto i = std::uint_fast16_t{ 0 }; i < lookup_table.size(); ++i ) {
        auto const byte = static_cast<std::uint8_t>( i );

        auto remainder = static_cast<Register>(
            Register{ byte } << ( std::numeric_limits<Register>::digits - std::numeric_limits<std::uint8_t>::digits ) );

        for ( auto bit = std::numeric_limits<std::uint8_t>::digits - 1; bit >= 0; --bit ) {
            auto const xor_polynomial = static_cast<bool>(
                remainder & ~( std::numeric_limits<Register>::max() >> 1 ) );

            remainder <<= 1;

            if ( xor_polynomial ) {
                remainder ^= polynomial;
            } // if
        }     // for

        lookup_table[ byte ] = remainder;
    } // for

    return lookup_table;
}

/**
 * \brief Augmented byte indexed lookup table calculator.
 *
 * \attention picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator is strictly
 *            superior to this calculator implementation since it does not have to process
 *            a message augment in addition to the message itself.
 *
 * This calculator implementation processes messages one byte at a time, and requires a
 * message augment to push the entirety of a message through the calculation. While this
 * results in higher memory use than bitwise implementations and table driven
 * implementations that process messages one nibble at a time, performance is higher due
 * to the message processing loop requiring fewer iterations to process a message.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Augmented_Byte_Indexed_Lookup_Table_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Augmented_Byte_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] calculation_parameters The calculation parameters.
     */
    constexpr Augmented_Byte_Indexed_Lookup_Table_Calculator(
        Calculation_Parameters<Register> const & calculation_parameters ) noexcept :
        m_lookup_table{ generate_byte_indexed_lookup_table( calculation_parameters.polynomial ) },
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
    constexpr Augmented_Byte_Indexed_Lookup_Table_Calculator(
        Augmented_Byte_Indexed_Lookup_Table_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Augmented_Byte_Indexed_Lookup_Table_Calculator(
        Augmented_Byte_Indexed_Lookup_Table_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Augmented_Byte_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Byte_Indexed_Lookup_Table_Calculator && expression ) noexcept
        -> Augmented_Byte_Indexed_Lookup_Table_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Byte_Indexed_Lookup_Table_Calculator const & expression ) noexcept
        -> Augmented_Byte_Indexed_Lookup_Table_Calculator & = default;

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
        auto const message_augment = Message_Augment<Register>{};

        return ( *m_process_output )( feed(
                   feed( m_initial_remainder, begin, end ),
                   message_augment.begin(),
                   message_augment.end() ) )
               ^ m_xor_output;
    }

  private:
    /**
     * \brief Calculation lookup table.
     */
    Byte_Indexed_Lookup_Table<Register> m_lookup_table{};

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

            auto const i = static_cast<std::uint8_t>(
                remainder
                >> ( std::numeric_limits<Register>::digits - std::numeric_limits<std::uint8_t>::digits ) );

            remainder = ( ( remainder << std::numeric_limits<std::uint8_t>::digits ) | processed_input )
                        ^ m_lookup_table[ i ];
        } // for

        return remainder;
    }
};

/**
 * \brief Direct byte indexed lookup table calculator.
 *
 * This calculator implementation processes messages one byte at a time, and does not
 * require a message augment to push the entirety of a message through the calculation.
 * While this results in higher memory use than bitwise implementations and table driven
 * implementations that process messages one nibble at a time, performance is higher due
 * to the message processing loop requiring fewer iterations to process a message.
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Direct_Byte_Indexed_Lookup_Table_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Direct_Byte_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] calculation_parameters The calculation parameters.
     */
    constexpr Direct_Byte_Indexed_Lookup_Table_Calculator( Calculation_Parameters<Register> const & calculation_parameters ) noexcept
        :
        m_lookup_table{ generate_byte_indexed_lookup_table( calculation_parameters.polynomial ) },
        m_preprocessed_initial_remainder{
            preprocess_initial_remainder( calculation_parameters.initial_remainder, m_lookup_table )
        },
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
    constexpr Direct_Byte_Indexed_Lookup_Table_Calculator(
        Direct_Byte_Indexed_Lookup_Table_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Direct_Byte_Indexed_Lookup_Table_Calculator(
        Direct_Byte_Indexed_Lookup_Table_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Direct_Byte_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Direct_Byte_Indexed_Lookup_Table_Calculator && expression ) noexcept
        -> Direct_Byte_Indexed_Lookup_Table_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Direct_Byte_Indexed_Lookup_Table_Calculator const & expression ) noexcept
        -> Direct_Byte_Indexed_Lookup_Table_Calculator & = default;

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
        auto remainder = m_preprocessed_initial_remainder;

        for ( ; begin != end; ++begin ) {
            auto const processed_input = ( *m_process_input )( *begin );

            auto const i = static_cast<std::uint8_t>(
                static_cast<std::uint8_t>(
                    remainder
                    >> ( std::numeric_limits<Register>::digits - std::numeric_limits<std::uint8_t>::digits ) )
                ^ processed_input );

            remainder = ( remainder << std::numeric_limits<std::uint8_t>::digits )
                        ^ m_lookup_table[ i ];
        } // for

        return ( *m_process_output )( remainder ) ^ m_xor_output;
    }

  private:
    /**
     * \brief Calculation lookup table.
     */
    Byte_Indexed_Lookup_Table<Register> m_lookup_table{};

    /**
     * \brief Calculation preprocessed initial remainder.
     */
    Register m_preprocessed_initial_remainder{};

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
     * \brief Preprocess a calculation initial remainder.
     *
     * \param[in] initial_remainder The calculation initial remainder.
     * \param[in] lookup_table The calculation lookup table.
     *
     * \return The preprocessed calculation initial remainder.
     */
    static constexpr auto preprocess_initial_remainder(
        Register                                    initial_remainder,
        Byte_Indexed_Lookup_Table<Register> const & lookup_table ) noexcept
    {
        auto preprocessed_initial_remainder = initial_remainder;

        for ( auto byte = 0; byte < std::numeric_limits<Register>::digits
                                        / std::numeric_limits<std::uint8_t>::digits;
              ++byte ) {
            auto const i = static_cast<std::uint8_t>(
                preprocessed_initial_remainder
                >> ( std::numeric_limits<Register>::digits - std::numeric_limits<std::uint8_t>::digits ) );

            preprocessed_initial_remainder = ( preprocessed_initial_remainder
                                               << std::numeric_limits<std::uint8_t>::digits )
                                             ^ lookup_table[ i ];
        } // for

        return preprocessed_initial_remainder;
    }
};

/**
 * \brief The number of bits in a nibble.
 */
constexpr auto NIBBLE_DIGITS = 4;

/**
 * \brief The largest value a nibble can hold.
 */
constexpr auto NIBBLE_MAX = std::uint_fast8_t{ 0xF };

/**
 * \brief The pair of nibbles that make up a byte.
 */
using Byte_Nibbles = Array<std::uint_fast8_t, std::numeric_limits<std::uint8_t>::digits / NIBBLE_DIGITS>;

/**
 * \brief Get the pair of nibbles that make up a byte (most significant nibble first).
 *
 * \param[in] byte The byte to get the pair of nibbles from.
 *
 * \return The pair of nibbles that make up the byte (most significant nibble first).
 */
constexpr auto get_byte_nibbles( std::uint8_t byte ) noexcept -> Byte_Nibbles
{
    return Byte_Nibbles{
        static_cast<std::uint_fast8_t>( byte >> NIBBLE_DIGITS ),
        static_cast<std::uint_fast8_t>( byte & NIBBLE_MAX ),
    };
}

/**
 * \brief Nibble indexed lookup table.
 *
 * \tparam Register Calculation register type.
 */
template<typename Register>
using Nibble_Indexed_Lookup_Table = Array<Register, NIBBLE_MAX + 1>;

/**
 * \brief Generate a nibble indexed lookup table.
 *
 * \tparam Register Calculation register type.
 *
 * \param[in] polynomial Calculation polynomial.
 *
 * \return The generated nibble indexed lookup table.
 */
template<typename Register>
constexpr auto generate_nibble_indexed_lookup_table( Register polynomial ) noexcept
    -> Nibble_Indexed_Lookup_Table<Register>
{
    Nibble_Indexed_Lookup_Table<Register> lookup_table;

    for ( auto nibble = std::uint_fast8_t{ 0 }; nibble < lookup_table.size(); ++nibble ) {
        auto remainder = static_cast<Register>(
            Register{ nibble } << ( std::numeric_limits<Register>::digits - NIBBLE_DIGITS ) );

        for ( auto bit = NIBBLE_DIGITS - 1; bit >= 0; --bit ) {
            auto const xor_polynomial = static_cast<bool>(
                remainder & ~( std::numeric_limits<Register>::max() >> 1 ) );

            remainder <<= 1;

            if ( xor_polynomial ) {
                remainder ^= polynomial;
            } // if
        }     // for

        lookup_table[ nibble ] = remainder;
    } // for

    return lookup_table;
}

/**
 * \brief Augmented nibble indexed lookup table calculator.
 *
 * \attention picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator is strictly
 *            superior to this calculator implementation since it does not have to process
 *            a message augment in addition to the message itself.
 *
 * This calculator implementation processes messages one nibble at a time, and requires a
 * message augment to push the entirety of a message through the calculation. This
 * achieves a balance between implementations that are optimized for performance (table
 * driven implementations that process messages one or more bytes at a time), and those
 * that are optimized for memory use (bitwise implementations).
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Augmented_Nibble_Indexed_Lookup_Table_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Augmented_Nibble_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] calculation_parameters The calculation parameters.
     */
    constexpr Augmented_Nibble_Indexed_Lookup_Table_Calculator(
        Calculation_Parameters<Register> const & calculation_parameters ) noexcept :
        m_lookup_table{ generate_nibble_indexed_lookup_table( calculation_parameters.polynomial ) },
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
    constexpr Augmented_Nibble_Indexed_Lookup_Table_Calculator(
        Augmented_Nibble_Indexed_Lookup_Table_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Augmented_Nibble_Indexed_Lookup_Table_Calculator(
        Augmented_Nibble_Indexed_Lookup_Table_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Augmented_Nibble_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Nibble_Indexed_Lookup_Table_Calculator && expression ) noexcept
        -> Augmented_Nibble_Indexed_Lookup_Table_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Augmented_Nibble_Indexed_Lookup_Table_Calculator const & expression ) noexcept
        -> Augmented_Nibble_Indexed_Lookup_Table_Calculator & = default;

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
        auto const message_augment = Message_Augment<Register>{};

        return ( *m_process_output )( feed(
                   feed( m_initial_remainder, begin, end ),
                   message_augment.begin(),
                   message_augment.end() ) )
               ^ m_xor_output;
    }

  private:
    /**
     * \brief Calculation lookup table.
     */
    Nibble_Indexed_Lookup_Table<Register> m_lookup_table{};

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
            auto const nibbles = get_byte_nibbles( ( *m_process_input )( *begin ) );

            for ( auto const nibble : nibbles ) {
                auto const i = static_cast<std::uint_fast8_t>(
                    remainder >> ( std::numeric_limits<Register>::digits - NIBBLE_DIGITS ) );

                remainder = ( ( remainder << NIBBLE_DIGITS ) | nibble ) ^ m_lookup_table[ i ];
            } // for
        }     // for

        return remainder;
    }
};

/**
 * \brief Direct nibble indexed lookup table calculator.
 *
 * This calculator implementation processes messages one nibble at a time, and does not
 * require a message augment to push the entirety of a message through the calculation.
 * This achieves a balance between implementations that are optimized for performance
 * (table driven implementations that process messages one or more bytes at a time), and
 * those that are optimized for memory use (bitwise implementations).
 *
 * \tparam Register_Type Calculation register type.
 */
template<typename Register_Type>
class Direct_Nibble_Indexed_Lookup_Table_Calculator {
  public:
    /**
     * \brief Calculation register type.
     */
    using Register = Register_Type;

    /**
     * \brief Constructor.
     */
    constexpr Direct_Nibble_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] calculation_parameters The calculation parameters.
     */
    constexpr Direct_Nibble_Indexed_Lookup_Table_Calculator(
        Calculation_Parameters<Register> const & calculation_parameters ) noexcept :
        m_lookup_table{ generate_nibble_indexed_lookup_table( calculation_parameters.polynomial ) },
        m_preprocessed_initial_remainder{
            preprocess_initial_remainder( calculation_parameters.initial_remainder, m_lookup_table )
        },
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
    constexpr Direct_Nibble_Indexed_Lookup_Table_Calculator(
        Direct_Nibble_Indexed_Lookup_Table_Calculator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Direct_Nibble_Indexed_Lookup_Table_Calculator(
        Direct_Nibble_Indexed_Lookup_Table_Calculator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Direct_Nibble_Indexed_Lookup_Table_Calculator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Direct_Nibble_Indexed_Lookup_Table_Calculator && expression ) noexcept
        -> Direct_Nibble_Indexed_Lookup_Table_Calculator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Direct_Nibble_Indexed_Lookup_Table_Calculator const & expression ) noexcept
        -> Direct_Nibble_Indexed_Lookup_Table_Calculator & = default;

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
        auto remainder = m_preprocessed_initial_remainder;

        for ( ; begin != end; ++begin ) {
            auto const nibbles = get_byte_nibbles( ( *m_process_input )( *begin ) );

            for ( auto const nibble : nibbles ) {
                auto const i = static_cast<std::uint_fast8_t>(
                    static_cast<std::uint_fast8_t>(
                        remainder >> ( std::numeric_limits<Register>::digits - NIBBLE_DIGITS ) )
                    ^ nibble );

                remainder = ( remainder << NIBBLE_DIGITS ) ^ m_lookup_table[ i ];
            } // for
        }     // for

        return ( *m_process_output )( remainder ) ^ m_xor_output;
    }

  private:
    /**
     * \brief Calculation lookup table.
     */
    Nibble_Indexed_Lookup_Table<Register> m_lookup_table{};

    /**
     * \brief Calculation preprocessed initial remainder.
     */
    Register m_preprocessed_initial_remainder{};

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
     * \brief Preprocess a calculation initial remainder.
     *
     * \param[in] initial_remainder The calculation initial remainder.
     * \param[in] lookup_table The calculation lookup table.
     *
     * \return The preprocessed calculation initial remainder.
     */
    static constexpr auto preprocess_initial_remainder(
        Register                                      initial_remainder,
        Nibble_Indexed_Lookup_Table<Register> const & lookup_table ) noexcept
    {
        auto preprocessed_initial_remainder = initial_remainder;

        for ( auto nibble = 0; nibble < std::numeric_limits<Register>::digits / NIBBLE_DIGITS; ++nibble ) {
            auto const i = static_cast<std::uint_fast8_t>(
                preprocessed_initial_remainder
                >> ( std::numeric_limits<Register>::digits - NIBBLE_DIGITS ) );

            preprocessed_initial_remainder = ( preprocessed_initial_remainder << NIBBLE_DIGITS )
                                             ^ lookup_table[ i ];
        } // for

        return preprocessed_initial_remainder;
    }
};

} // namespace picolibrary::CRC

#endif // PICOLIBRARY_CRC_H
