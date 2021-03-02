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
 * \brief picolibrary::ADC interface.
 */

#ifndef PICOLIBRARY_ADC_H
#define PICOLIBRARY_ADC_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

/**
 * \brief Analog-to-Digital Converter (ADC) facilities.
 */
namespace picolibrary::ADC {

/**
 * \brief ADC sample.
 *
 * \tparam Value_Type The integral type used to hold a sample value.
 * \tparam MIN_SAMPLE The minimum possible sample value.
 * \tparam MAX_SAMPLE The maximum possible sample value.
 */
template<typename Value_Type, Value_Type MIN_SAMPLE, Value_Type MAX_SAMPLE>
class Sample {
  public:
    /**
     * \brief The integral type used to hold a sample value;
     */
    using Value = Value_Type;

    /**
     * \brief The minimum possible sample value.
     */
    static constexpr auto MIN = MIN_SAMPLE;

    /**
     * \brief The maximum possible sample value.
     */
    static constexpr auto MAX = MAX_SAMPLE;

    /**
     * \brief Constructor.
     */
    constexpr Sample() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] value The sample value.
     */
    constexpr Sample( Value value ) noexcept : m_value{ value }
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
     * \brief Get the minimum possible sample value.
     *
     * \return The minimum possible sample value.
     */
    constexpr auto min() const noexcept
    {
        return MIN;
    }

    /**
     * \brief Get the maximum possible sample value.
     *
     * \return The maximum possible sample value.
     */
    constexpr auto max() const noexcept
    {
        return MAX;
    }

    /**
     * \brief Get the sample value.
     *
     * \return The sample value.
     */
    constexpr operator Value() const noexcept
    {
        return m_value;
    }

  private:
    /**
     * \brief The sample value.
     */
    Value m_value{};
};

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam Value The integral type used to hold a sample value.
 * \tparam MIN The minimum possible sample value.
 * \tparam MAX The maximum possible sample value.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
template<typename Value, Value MIN, Value MAX>
constexpr auto operator==( Sample<Value, MIN, MAX> lhs, Sample<Value, MIN, MAX> rhs ) noexcept
{
    return static_cast<Value>( lhs ) == static_cast<Value>( rhs );
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::ADC::Sample
 *
 * \tparam Value The integral type used to hold a sample value.
 * \tparam MIN The minimum possible sample value.
 * \tparam MAX The maximum possible sample value.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
template<typename Value, Value MIN, Value MAX>
constexpr auto operator!=( Sample<Value, MIN, MAX> lhs, Sample<Value, MIN, MAX> rhs ) noexcept
{
    return not( lhs == rhs );
}

/**
 * \brief Blocking, single sample ADC concept.
 */
class Blocking_Single_Sample_Converter_Concept {
  public:
    /**
     * \brief ADC sample.
     */
    using Sample = ::picolibrary::ADC::Sample<std::uint_fast16_t, 0, 1023>;

    /**
     * \brief Constructor.
     */
    Blocking_Single_Sample_Converter_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept && source ) noexcept = default;

    Blocking_Single_Sample_Converter_Concept( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Blocking_Single_Sample_Converter_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Blocking_Single_Sample_Converter_Concept && expression ) noexcept
        -> Blocking_Single_Sample_Converter_Concept & = default;

    auto operator=( Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Initialize the ADC's hardware.
     *
     * \return Nothing if ADC hardware initialization succeeded.
     * \return An error code if ADC hardware initialization failed. If ADC hardware
     *         initialization cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Get a sample.
     *
     * \return A sample if getting the sample succeeded.
     * \return An error code if getting the sample failed. If getting the sample cannot
     *         fail, return picolibrary::Result<Sample, picolibrary::Void>.
     */
    auto sample() noexcept -> Result<Sample, Error_Code>;
};

/**
 * \brief Non-blocking, single sample ADC concept.
 */
class Non_Blocking_Single_Sample_Converter_Concept {
  public:
    /**
     * \brief ADC sample.
     */
    using Sample = ::picolibrary::ADC::Sample<std::uint_fast16_t, 0, 1023>;

    /**
     * \brief Constructor.
     */
    Non_Blocking_Single_Sample_Converter_Concept() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Non_Blocking_Single_Sample_Converter_Concept(
        Non_Blocking_Single_Sample_Converter_Concept && source ) noexcept = default;

    Non_Blocking_Single_Sample_Converter_Concept( Non_Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Non_Blocking_Single_Sample_Converter_Concept() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Non_Blocking_Single_Sample_Converter_Concept && expression ) noexcept
        -> Non_Blocking_Single_Sample_Converter_Concept & = default;

    auto operator=( Non_Blocking_Single_Sample_Converter_Concept const & ) = delete;

    /**
     * \brief Initialize the ADC's hardware.
     *
     * \return Nothing if ADC hardware initialization succeeded.
     * \return An error code if ADC hardware initialization failed. If ADC hardware
     *         initialization cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto initialize() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Initiate a conversion.
     *
     * \return Nothing if initiation of the conversion succeeded.
     * \return An error code if initiation of the conversion failed. If initiation of the
     *         conversion cannot fail, return picolibrary::Result<picolibrary::Void,
     *         picolibrary::Void>.
     */
    auto initiate_conversion() noexcept -> Result<Void, Error_Code>;

    /**
     * \brief Check if a sample is available.
     *
     * \return true if a sample is available.
     * \return false if a sample is not available.
     * \return An error code if the check failed. If the check cannot fail, return
     *         picolibrary::Result<bool, picolibrary::Void>.
     */
    auto sample_available() const noexcept -> Result<bool, Error_Code>;

    /**
     * \brief Get a sample.
     *
     * \return A sample if getting the sample succeeded.
     * \return An error code if getting the sample failed. If getting the sample cannot
     *         fail, return picolibrary::Result<Sample, picolibrary::Void>.
     */
    auto sample() noexcept -> Result<Sample, Error_Code>;
};

} // namespace picolibrary::ADC

#endif // PICOLIBRARY_ADC_H
