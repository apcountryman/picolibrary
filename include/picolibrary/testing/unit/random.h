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
 * \brief picolibrary::Testing::Unit random interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_RANDOM_H
#define PICOLIBRARY_TESTING_UNIT_RANDOM_H

#include <limits>
#include <random>

namespace picolibrary::Testing::Unit {

/**
 * \brief Get the unit testing pseudo-random number generator.
 *
 * \return The unit testing pseudo-random number generator.
 */
inline auto & pseudo_random_number_generator()
{
    static auto generator = std::mt19937{ std::random_device{}() };

    return generator;
}

/**
 * \brief Generate a pseudo-random value within the specified range.
 *
 * \tparam T The type to be pseudo-randomly generated.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random value in the range [min,max].
 */
template<typename T>
auto random( T min, T max )
{
    return std::uniform_int_distribution<T>{ min, max }( pseudo_random_number_generator() );
}

/**
 * \brief Generate a pseudo-random value greater than or equal to a minimum value.
 *
 * \tparam T The type to be pseudo-randomly generated.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random value in the range [min,std::numeric_limits<T>::max()].
 */
template<typename T>
auto random( T min )
{
    return random<T>( min, std::numeric_limits<T>::max() );
}

/**
 * \brief Generate a pseudo-random value.
 *
 * \tparam T The type to be pseudo-randomly generated.
 *
 * \return A pseudo-random value in the range
 *         [std::numeric_limits<T>::min(),std::numeric_limits<T>::max()].
 */
template<typename T>
auto random()
{
    return random<T>( std::numeric_limits<T>::min(), std::numeric_limits<T>::max() );
}

/**
 * \brief Generate a pseudo-random bool.
 *
 * \return A pseudo-random bool.
 */
template<>
inline auto random<bool>()
{
    return static_cast<bool>( random<std::uint_fast8_t>( 0, 1 ) );
}

/**
 * \brief Generate a pseudo-random char in the range [' ','~'].
 *
 * \return A pseudo-random char in the range [' ','~'].
 */
template<>
inline auto random<char>()
{
    return random<char>( ' ', '~' );
}

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_RANDOM_H
