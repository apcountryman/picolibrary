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
 * \brief picolibrary::Testing::Unit::I2C interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_I2C_H
#define PICOLIBRARY_TESTING_UNIT_I2C_H

#include "picolibrary/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric within the specified
 *        range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric in the range [min,max].
 */
template<>
inline auto random<I2C::Address_Numeric>( I2C::Address_Numeric min, I2C::Address_Numeric max )
{
    return I2C::Address_Numeric{ random<I2C::Address_Numeric::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric greater than or equal
 *        to a minimum picolibrary::I2C::Address_Numeric.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric in the range
 *         [min,picolibrary::I2C::Address_Numeric::max()].
 */
template<>
inline auto random<I2C::Address_Numeric>( I2C::Address_Numeric min )
{
    return random<I2C::Address_Numeric>( min, I2C::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Numeric.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Numeric.
 */
template<>
inline auto random<I2C::Address_Numeric>()
{
    return random<I2C::Address_Numeric>( I2C::Address_Numeric::min(), I2C::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted within the
 *        specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted in the range [min,max].
 */
template<>
inline auto random<I2C::Address_Transmitted>( I2C::Address_Transmitted min, I2C::Address_Transmitted max )
{
    return I2C::Address_Transmitted{ static_cast<I2C::Address_Transmitted::Unsigned_Integer>(
        random<I2C::Address_Transmitted::Unsigned_Integer>( min.as_unsigned_integer(), max.as_unsigned_integer() )
        & 0b1111111'0 ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted greater than or
 *        equal to a minimum picolibrary::I2C::Address_Transmitted.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted in the range
 *         [min,picolibrary::I2C::Address_Transmitted::max()].
 */
template<>
inline auto random<I2C::Address_Transmitted>( I2C::Address_Transmitted min )
{
    return random<I2C::Address_Transmitted>( min, I2C::Address_Transmitted::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::I2C::Address_Transmitted.
 *
 * \return A pseudo-random picolibrary::I2C::Address_Transmitted.
 */
template<>
inline auto random<I2C::Address_Transmitted>()
{
    return random<I2C::Address_Transmitted>(
        I2C::Address_Transmitted::min(), I2C::Address_Transmitted::max() );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Inter-Integrated Circuit (I2C) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::I2C {
} // namespace picolibrary::Testing::Unit::I2C

#endif // PICOLIBRARY_TESTING_UNIT_I2C_H
