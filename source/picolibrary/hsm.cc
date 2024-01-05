/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::HSM implementation.
 */

#include "picolibrary/hsm.h"

#include "picolibrary/event.h"
#include "picolibrary/utility.h"

namespace picolibrary {

HSM::Pseudo_Event_Category const HSM::Pseudo_Event_Category::INSTANCE{};

Simple_Event const HSM::DISCOVERY{ Pseudo_Event_Category::instance(),
                                   to_underlying( Pseudo_Event::DISCOVERY ) };

Simple_Event const HSM::ENTRY{ Pseudo_Event_Category::instance(),
                               to_underlying( Pseudo_Event::ENTRY ) };

Simple_Event const HSM::EXIT{ Pseudo_Event_Category::instance(), to_underlying( Pseudo_Event::EXIT ) };

Simple_Event const HSM::NESTED_INITIAL_TRANSITION{ Pseudo_Event_Category::instance(),
                                                   to_underlying( Pseudo_Event::NESTED_INITIAL_TRANSITION ) };

} // namespace picolibrary
