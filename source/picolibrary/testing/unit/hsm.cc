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
 * \brief picolibrary::Testing::Unit::HSM implementation.
 */

#include "picolibrary/testing/unit/hsm.h"

#include "picolibrary/hsm.h"

namespace picolibrary::Testing::Unit::HSM {

Mock_Event_Category::Mock_Event_Category()
{
}

Mock_Event_Category::~Mock_Event_Category() noexcept
{
}

Mock_Event::Mock_Event( Mock_Event_Category const & category, ::picolibrary::HSM::Event_ID id ) :
    ::picolibrary::HSM::Event{ category, id }
{
}

Mock_Event::~Mock_Event() noexcept
{
}

} // namespace picolibrary::Testing::Unit::HSM
