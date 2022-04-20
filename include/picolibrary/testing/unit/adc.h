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
 * \brief picolibrary::Testing::Unit::ADC interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ADC_H
#define PICOLIBRARY_TESTING_UNIT_ADC_H

#include <cstdint>

#include "picolibrary/adc.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast8_t, 8>>(
    ADC::Sample<std::uint_fast8_t, 8> min,
    ADC::Sample<std::uint_fast8_t, 8> max )
{
    return ADC::Sample<std::uint_fast8_t, 8>{ random<ADC::Sample<std::uint_fast8_t, 8>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8> greater
 *        than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast8_t, 8>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast8_t, 8>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast8_t, 8>>( ADC::Sample<std::uint_fast8_t, 8> min )
{
    return random<ADC::Sample<std::uint_fast8_t, 8>>( min, ADC::Sample<std::uint_fast8_t, 8>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast8_t, 8> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast8_t,
 *         8>::min(),picolibrary::ADC::Sample<std::uint_fast8_t, 8>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast8_t, 8>>()
{
    return random<ADC::Sample<std::uint_fast8_t, 8>>(
        ADC::Sample<std::uint_fast8_t, 8>::min(), ADC::Sample<std::uint_fast8_t, 8>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 10>>(
    ADC::Sample<std::uint_fast16_t, 10> min,
    ADC::Sample<std::uint_fast16_t, 10> max )
{
    return ADC::Sample<std::uint_fast16_t, 10>{ random<ADC::Sample<std::uint_fast16_t, 10>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast16_t,
 *        10>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast16_t, 10>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 10>>( ADC::Sample<std::uint_fast16_t, 10> min )
{
    return random<ADC::Sample<std::uint_fast16_t, 10>>(
        min, ADC::Sample<std::uint_fast16_t, 10>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 10> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast16_t,
 *         10>::min(),picolibrary::ADC::Sample<std::uint_fast16_t, 10>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 10>>()
{
    return random<ADC::Sample<std::uint_fast16_t, 10>>(
        ADC::Sample<std::uint_fast16_t, 10>::min(), ADC::Sample<std::uint_fast16_t, 10>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 12>>(
    ADC::Sample<std::uint_fast16_t, 12> min,
    ADC::Sample<std::uint_fast16_t, 12> max )
{
    return ADC::Sample<std::uint_fast16_t, 12>{ random<ADC::Sample<std::uint_fast16_t, 12>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast16_t,
 *        12>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast16_t, 12>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 12>>( ADC::Sample<std::uint_fast16_t, 12> min )
{
    return random<ADC::Sample<std::uint_fast16_t, 12>>(
        min, ADC::Sample<std::uint_fast16_t, 12>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 12> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast16_t,
 *         12>::min(),picolibrary::ADC::Sample<std::uint_fast16_t, 12>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 12>>()
{
    return random<ADC::Sample<std::uint_fast16_t, 12>>(
        ADC::Sample<std::uint_fast16_t, 12>::min(), ADC::Sample<std::uint_fast16_t, 12>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 14>>(
    ADC::Sample<std::uint_fast16_t, 14> min,
    ADC::Sample<std::uint_fast16_t, 14> max )
{
    return ADC::Sample<std::uint_fast16_t, 14>{ random<ADC::Sample<std::uint_fast16_t, 14>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast16_t,
 *        14>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast16_t, 14>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 14>>( ADC::Sample<std::uint_fast16_t, 14> min )
{
    return random<ADC::Sample<std::uint_fast16_t, 14>>(
        min, ADC::Sample<std::uint_fast16_t, 14>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 14> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast16_t,
 *         14>::min(),picolibrary::ADC::Sample<std::uint_fast16_t, 14>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 14>>()
{
    return random<ADC::Sample<std::uint_fast16_t, 14>>(
        ADC::Sample<std::uint_fast16_t, 14>::min(), ADC::Sample<std::uint_fast16_t, 14>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 16>>(
    ADC::Sample<std::uint_fast16_t, 16> min,
    ADC::Sample<std::uint_fast16_t, 16> max )
{
    return ADC::Sample<std::uint_fast16_t, 16>{ random<ADC::Sample<std::uint_fast16_t, 16>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast16_t,
 *        16>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast16_t, 16>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 16>>( ADC::Sample<std::uint_fast16_t, 16> min )
{
    return random<ADC::Sample<std::uint_fast16_t, 16>>(
        min, ADC::Sample<std::uint_fast16_t, 16>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast16_t, 16> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast16_t,
 *         16>::min(),picolibrary::ADC::Sample<std::uint_fast16_t, 16>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast16_t, 16>>()
{
    return random<ADC::Sample<std::uint_fast16_t, 16>>(
        ADC::Sample<std::uint_fast16_t, 16>::min(), ADC::Sample<std::uint_fast16_t, 16>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 18>>(
    ADC::Sample<std::uint_fast32_t, 18> min,
    ADC::Sample<std::uint_fast32_t, 18> max )
{
    return ADC::Sample<std::uint_fast32_t, 18>{ random<ADC::Sample<std::uint_fast32_t, 18>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast32_t,
 *        18>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast32_t, 18>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 18>>( ADC::Sample<std::uint_fast32_t, 18> min )
{
    return random<ADC::Sample<std::uint_fast32_t, 18>>(
        min, ADC::Sample<std::uint_fast32_t, 18>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 18> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast32_t,
 *         18>::min(),picolibrary::ADC::Sample<std::uint_fast32_t, 18>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 18>>()
{
    return random<ADC::Sample<std::uint_fast32_t, 18>>(
        ADC::Sample<std::uint_fast32_t, 18>::min(), ADC::Sample<std::uint_fast32_t, 18>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 20>>(
    ADC::Sample<std::uint_fast32_t, 20> min,
    ADC::Sample<std::uint_fast32_t, 20> max )
{
    return ADC::Sample<std::uint_fast32_t, 20>{ random<ADC::Sample<std::uint_fast32_t, 20>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast32_t,
 *        20>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast32_t, 20>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 20>>( ADC::Sample<std::uint_fast32_t, 20> min )
{
    return random<ADC::Sample<std::uint_fast32_t, 20>>(
        min, ADC::Sample<std::uint_fast32_t, 20>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 20> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast32_t,
 *         20>::min(),picolibrary::ADC::Sample<std::uint_fast32_t, 20>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 20>>()
{
    return random<ADC::Sample<std::uint_fast32_t, 20>>(
        ADC::Sample<std::uint_fast32_t, 20>::min(), ADC::Sample<std::uint_fast32_t, 20>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24> within
 *        the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24> in the range
 *         [min,max].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 24>>(
    ADC::Sample<std::uint_fast32_t, 24> min,
    ADC::Sample<std::uint_fast32_t, 24> max )
{
    return ADC::Sample<std::uint_fast32_t, 24>{ random<ADC::Sample<std::uint_fast32_t, 24>::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24>
 *        greater than or equal to a minimum picolibrary::ADC::Sample<std::uint_fast32_t,
 *        24>.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24> in the range
 *         [min,picolibrary::ADC::Sample<std::uint_fast32_t, 24>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 24>>( ADC::Sample<std::uint_fast32_t, 24> min )
{
    return random<ADC::Sample<std::uint_fast32_t, 24>>(
        min, ADC::Sample<std::uint_fast32_t, 24>::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24>.
 *
 * \return A pseudo-random picolibrary::ADC::Sample<std::uint_fast32_t, 24> in the range
 *         [picolibrary::ADC::Sample<std::uint_fast32_t,
 *         24>::min(),picolibrary::ADC::Sample<std::uint_fast32_t, 24>::max()].
 */
template<>
inline auto random<ADC::Sample<std::uint_fast32_t, 24>>()
{
    return random<ADC::Sample<std::uint_fast32_t, 24>>(
        ADC::Sample<std::uint_fast32_t, 24>::min(), ADC::Sample<std::uint_fast32_t, 24>::max() );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Analog-to-Digital Converter (ADC) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::ADC {
} // namespace picolibrary::Testing::Unit::ADC

#endif // PICOLIBRARY_TESTING_UNIT_ADC_H
