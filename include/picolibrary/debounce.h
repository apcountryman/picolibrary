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
 * \brief picolibrary::Debounce interface.
 */

#ifndef PICOLIBRARY_DEBOUNCE_H
#define PICOLIBRARY_DEBOUNCE_H

#include <cstdint>
#include <limits>
#include <type_traits>

#include "picolibrary/bit_manipulation.h"

/**
 * \brief Debounce facilities.
 */
namespace picolibrary::Debounce {

/**
 * \brief Single signal debouncer.
 *
 * \tparam Samples The unsigned integer type used to hold a signal sample stream.
 * \tparam SAMPLES The number of signal samples to use to determine whether the signal
 *         state is stable.
 */
template<typename Samples, std::uint_fast8_t SAMPLES>
class Single_Signal_Debouncer {
  public:
    static_assert( std::is_unsigned_v<Samples> );
    static_assert( SAMPLES <= std::numeric_limits<Samples>::digits );

    Single_Signal_Debouncer() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] is_high The initial debounced signal state.
     */
    constexpr Single_Signal_Debouncer( bool is_high ) noexcept :
        m_samples{ is_high ? SAMPLES_MASK : Samples{ 0 } },
        m_is_high{ is_high }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Single_Signal_Debouncer( Single_Signal_Debouncer && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Single_Signal_Debouncer( Single_Signal_Debouncer const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Single_Signal_Debouncer() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator          =( Single_Signal_Debouncer && expression ) noexcept
        -> Single_Signal_Debouncer & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Single_Signal_Debouncer const & expression ) noexcept
        -> Single_Signal_Debouncer & = default;

    /**
     * \brief Update the debouncer with a new signal sample.
     *
     * \param[in] is_high The new signal sample to update the debouncer with.
     */
    constexpr void debounce( bool is_high ) noexcept
    {
        m_samples = ( ( m_samples << 1 ) | is_high ) & SAMPLES_MASK;

        if ( m_samples == SAMPLES_MASK ) {
            m_is_high = true;
        } // if

        if ( m_samples == 0 ) {
            m_is_high = false;
        } // if
    }

    /**
     * \brief Check if the debounced signal is in the high state.
     *
     * \return true if the debounced signal is in the high state.
     * \return false if the debounced signal is not in the high state.
     */
    constexpr auto is_high() const noexcept -> bool
    {
        return m_is_high;
    }

    /**
     * \brief Check if the debounced signal is in the low state.
     *
     * \return true if the debounced signal is in the low state.
     * \return false if the debounced signal is not in the low state.
     */
    constexpr auto is_low() const noexcept -> bool
    {
        return not m_is_high;
    }

  private:
    /**
     * \brief Samples mask.
     */
    static constexpr auto SAMPLES_MASK = mask<Samples>( SAMPLES, 0 );

    /**
     * \brief Signal sample stream.
     */
    Samples m_samples{};

    /**
     * \brief The debounced signal is in the high state.
     */
    bool m_is_high{};
};

} // namespace picolibrary::Debounce

#endif // PICOLIBRARY_DEBOUNCE_H
