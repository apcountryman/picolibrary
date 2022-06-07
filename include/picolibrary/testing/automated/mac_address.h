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
 * \brief picolibrary::Testing::Automated::MAC_Address interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H
#define PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H

#include "picolibrary/mac_address.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::MAC_Address within the specified range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::MAC_Address in the range [min,max].
 */
template<>
inline auto random<MAC_Address>( MAC_Address min, MAC_Address max ) -> MAC_Address
{
    return MAC_Address{ random<MAC_Address::Unsigned_Integer>(
        min.as_unsigned_integer(), max.as_unsigned_integer() ) };
}

/**
 * \brief Generate a pseudo-random picolibrary::MAC_Address greater than or equal to a
 *        minimum picolibrary::MAC_Address.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random picolibrary::MAC_Address in the range
 *         [min,picolibrary::MAC_Address::max()].
 */
template<>
inline auto random<MAC_Address>( MAC_Address min ) -> MAC_Address
{
    return random<MAC_Address>( min, MAC_Address::max() );
}

/**
 * \brief Generate a pseudo-random picolibrary::MAC_Address.
 *
 * \return A pseudo-random picolibrary::MAC_Address in the range
 *         [picolibrary::MAC_Address::min(),picolibrary::MAC_Address::max()].
 */
template<>
inline auto random<MAC_Address>() -> MAC_Address
{
    return random<MAC_Address>( MAC_Address::min(), MAC_Address::max() );
}

} // namespace picolibrary::Testing::Automated

#endif // PICOLIBRARY_TESTING_AUTOMATED_MAC_ADDRESS_H