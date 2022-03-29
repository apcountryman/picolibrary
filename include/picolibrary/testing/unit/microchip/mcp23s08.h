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
 * \brief picolibrary::Testing::Unit::Microchip::MCP23S08 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23S08_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23S08_H

#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric
 *        within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [min,max].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Numeric>(
    Microchip::MCP23S08::Address_Numeric min,
    Microchip::MCP23S08::Address_Numeric max )
{
    return Microchip::MCP23S08::Address_Numeric{ random<Microchip::MCP23S08::Address_Numeric::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric
 *        greater than or equal to a minimum
 *        picolibrary::Microchip::MCP23S08::Address_Numeric.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [min,picolibrary::Microchip::MCP23S08::Address_Numeric::max()].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Numeric>( Microchip::MCP23S08::Address_Numeric min )
{
    return random<Microchip::MCP23S08::Address_Numeric>(
        min, Microchip::MCP23S08::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Numeric in the range
 *         [picolibrary::Microchip::MCP23S08::Address_Numeric::min(),picolibrary::Microchip::MCP23S08::Address_Numeric::max()].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Numeric>()
{
    return random<Microchip::MCP23S08::Address_Numeric>(
        Microchip::MCP23S08::Address_Numeric::min(), Microchip::MCP23S08::Address_Numeric::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted
 *        within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range [min,max].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Transmitted>(
    Microchip::MCP23S08::Address_Transmitted min,
    Microchip::MCP23S08::Address_Transmitted max )
{
    return Microchip::MCP23S08::Address_Transmitted{
        static_cast<Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer>(
            random<Microchip::MCP23S08::Address_Transmitted::Unsigned_Integer>(
                min.as_unsigned_integer(), max.as_unsigned_integer() )
            & 0b11111'11'0 )
    };
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted
 *        greater than or equal to a minimum
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range [min,picolibrary::Microchip::MCP23S08::Address_Transmitted::max()].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Transmitted>( Microchip::MCP23S08::Address_Transmitted min )
{
    return random<Microchip::MCP23S08::Address_Transmitted>(
        min, Microchip::MCP23S08::Address_Transmitted::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted.
 *
 * \return A pseudo-random picolibrary::Microchip::MCP23S08::Address_Transmitted in the
 *         range
 *         [picolibrary::Microchip::MCP23S08::Address_Transmitted::min(),picolibrary::Microchip::MCP23S08::Address_Transmitted::max()].
 */
template<>
inline auto random<Microchip::MCP23S08::Address_Transmitted>()
{
    return random<Microchip::MCP23S08::Address_Transmitted>(
        Microchip::MCP23S08::Address_Transmitted::min(),
        Microchip::MCP23S08::Address_Transmitted::max() );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Microchip MCP23S08 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP23S08 {
} // namespace picolibrary::Testing::Unit::Microchip::MCP23S08

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP23S08_H
