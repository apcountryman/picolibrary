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
 * \brief picolibrary::State_Machine implementation.
 */

#include "picolibrary/state_machine.h"

#include "picolibrary/event.h"
#include "picolibrary/utility.h"

namespace picolibrary {

State_Machine::Pseudo_Event_Category const State_Machine::Pseudo_Event_Category::INSTANCE{};

Simple_Event const State_Machine::ENTRY{ Pseudo_Event_Category::instance(),
                                         to_underlying( Pseudo_Event::ENTRY ) };

Simple_Event const State_Machine::EXIT{ Pseudo_Event_Category::instance(),
                                        to_underlying( Pseudo_Event::EXIT ) };

} // namespace picolibrary
