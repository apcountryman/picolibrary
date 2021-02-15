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
 * \brief picolibrary::Testing::Unit::I2C interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_I2C_H
#define PICOLIBRARY_TESTING_UNIT_I2C_H

#include <cstdint>

#include "picolibrary/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random I2C device address within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random I2C device address in the range [min,max].
 */
template<>
inline auto random<I2C::Address>( I2C::Address min, I2C::Address max )
{
    return I2C::Address{ I2C::Address::NUMERIC,
                         random<std::uint_fast8_t>( min.numeric(), max.numeric() ) };
}

/**
 * \brief Generate a pseudo-random I2C device address greater than or equal to a minimum
 *        address.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random I2C device address in the range
 *         [min,picolibrary::I2C::Address::max()].
 */
template<>
inline auto random<I2C::Address>( I2C::Address min )
{
    return random<I2C::Address>( min, I2C::Address::max() );
}

/**
 * \brief Generate a pseudo-random I2C device address.
 *
 * \return A pseudo-random I2C device address in the range
 *         [picolibrary::I2C::Address::min(),picolibrary::I2C::Address::max()].
 */
template<>
inline auto random<I2C::Address>()
{
    return random<I2C::Address>( I2C::Address::min(), I2C::Address::max() );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Inter-Integrated Circuit (I2C) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::I2C {
} // namespace picolibrary::Testing::Unit::I2C

#endif // PICOLIBRARY_TESTING_UNIT_I2C_H
