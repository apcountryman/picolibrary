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
 * \brief picolibrary::Testing::Automated::RGB interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_RGB_H
#define PICOLIBRARY_TESTING_AUTOMATED_RGB_H

#include <cstdint>

#include "picolibrary/rgb.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::RGB::Color.
 *
 * \brief  A pseudo-randomly generated picolibrary::RGB::Color.
 */
template<>
inline auto random<RGB::Color>() -> RGB::Color
{
    return RGB::Color{ random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>() };
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief RGB automated testing facilities.
 */
namespace picolibrary::Testing::Automated::RGB {
} // namespace picolibrary::Testing::Automated::RGB

#endif // PICOLIBRARY_TESTING_AUTOMATED_RGB_H