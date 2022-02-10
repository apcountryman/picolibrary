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
 * \brief picolibrary::Testing::Unit event interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_EVENT_H
#define PICOLIBRARY_TESTING_UNIT_EVENT_H

#include "gmock/gmock.h"
#include "picolibrary/event.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock event category.
 */
class Mock_Event_Category : public Event_Category {
  public:
    static auto const & instance()
    {
        static auto category = Mock_Event_Category{};

        return category;
    }

    Mock_Event_Category() = default;

    Mock_Event_Category( Mock_Event_Category && ) = delete;

    Mock_Event_Category( Mock_Event_Category const & ) = delete;

    ~Mock_Event_Category() noexcept = default;

    auto operator=( Mock_Event_Category && ) = delete;

    auto operator=( Mock_Event_Category const & ) = delete;

    MOCK_METHOD( char const *, name, (), ( const, noexcept, override ) );

    MOCK_METHOD( char const *, event_description, ( Event_ID ), ( const, noexcept, override ) );
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_EVENT_H
