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

static_assert( MR::Bit::RST == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( IR::Bit::CONFLICT == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( IMR::Bit::CONFLICT == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( PHYCFGR::Bit::RST == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

static_assert( SN_MR::Bit::UCASTB == 4 );
static_assert( SN_MR::Bit::MIP6B == 4 );
static_assert( SN_MR::Bit::ND == 5 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( SN_MR::Bit::MC == 5 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( SN_MR::Bit::MMB == 5 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( SN_MR::Bit::MULTI == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( SN_MR::Bit::MFEN == 7 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert( SN_IR::Bit::RESERVED5 == 5 ); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace picolibrary::WIZnet::W5500
