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
 * \brief picolibrary::WIZnet::W5500 implementation.
 */

#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::WIZnet::W5500 {

static_assert( Control_Byte::Bit::BSB == 3 );
static_assert( Control_Byte::Bit::REGION == 3 );
static_assert( Control_Byte::Bit::SOCKET == 5 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace picolibrary::WIZnet::W5500
