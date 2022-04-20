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
 * \brief picolibrary::ADC interface.
 */

#ifndef PICOLIBRARY_ADC_H
#define PICOLIBRARY_ADC_H

#include <cstdint>
#include <limits>
#include <type_traits>

#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"

/**
 * \brief Analog-to-Digital Converter (ADC) facilities.
 */
namespace picolibrary::ADC {

/**
 * \brief Sample.
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 */
template<typename T, std::uint_fast8_t N>
class Sample {
  public:
    static_assert( std::is_unsigned_v<T> );

    static_assert( N <= std::numeric_limits<T>::digits );

    /**
     * \brief Sample unsigned integer representation.
     */
    using Unsigned_Integer = T;

    /**
     * \brief Get the minimum valid sample.
     *
     * \return The minimum valid sample.
     */
    static constexpr auto min() noexcept -> Sample
    {
        return Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0 };
    }

    /**
     * \brief Get the maximum valid sample.
     *
     * \return The maximum valid sample.
     */
    static constexpr auto max() noexcept -> Sample
    {
        return Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, mask<Unsigned_Integer>( N, 0 ) };
    }

    /**
     * \brief Constructor.
     */
    constexpr Sample() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] sample The sample.
     *
     * \pre sample <= picolibrary::ADC::Sample::max().as_unsigned_integer()
     */
    constexpr Sample( Unsigned_Integer sample ) noexcept : m_sample{ sample }
    {
        expect( sample <= max().as_unsigned_integer(), Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] sample The sample.
     */
    constexpr Sample( Bypass_Precondition_Expectation_Checks, Unsigned_Integer sample ) noexcept :
        m_sample{ sample }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Sample( Sample && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Sample( Sample const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Sample() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Sample && expression ) noexcept -> Sample & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Sample const & expression ) noexcept -> Sample & = default;

    /**
     * \brief Get the sample in its unsigned integer representation.
     *
     * \return The sample in its unsigned integer representation.
     */
    constexpr auto as_unsigned_integer() const noexcept -> Unsigned_Integer
    {
        return m_sample;
    }

  private:
    /**
     * \brief The sample.
     */
    Unsigned_Integer m_sample{ 0 };
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator==( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() == rhs.as_unsigned_integer();
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator!=( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return not( lhs == rhs );
}

/**
 * \brief Less than operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than rhs.
 * \return false if lhs is not less than rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator<( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return lhs.as_unsigned_integer() < rhs.as_unsigned_integer();
}

/**
 * \brief Greater than operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than rhs.
 * \return false if lhs is not greater than rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator>( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return rhs < lhs;
}

/**
 * \brief Less than or equal to operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is less than or equal to rhs.
 * \return false if lhs is not less than or equal to rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator<=( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return not( lhs > rhs );
}

/**
 * \brief Greater than or equal to operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam T The sample unsigned integer representation.
 * \tparam N The number of bits in the sample.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is greater than or equal to rhs.
 * \return false if lhs is not greater than or equal to rhs.
 */
template<typename T, std::uint_fast8_t N>
constexpr auto operator>=( Sample<T, N> lhs, Sample<T, N> rhs ) noexcept -> bool
{
    return not( lhs < rhs );
}

/**
 * \brief Blocking, single sample ADC concept.
 */
class Blocking_Single_Sample_Converter_Concept {
  public:
    /**
     * \brief Sample.
     */
    using Sample = ::picolibrary::ADC::Sample<std::uint_fast8_t, 8>;

    /**
     * \brief Constructor.
     */
    Blocking_Single_Sample_Converter_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept && source ) noexcept;

    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Blocking_Single_Sample_Converter_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Blocking_Single_Sample_Converter_Concept && expression ) noexcept
        -> Blocking_Single_Sample_Converter_Concept &;

    auto operator=( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Initialize the ADC's hardware.
     */
    void initialize() noexcept;

    /**
     * \brief Get a sample.
     *
     * \return The sample.
     */
    auto sample() noexcept -> Sample;
};

} // namespace picolibrary::ADC

#endif // PICOLIBRARY_ADC_H
