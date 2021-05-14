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
 * \brief picolibrary::Testing::Unit::HSM interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_HSM_H
#define PICOLIBRARY_TESTING_UNIT_HSM_H

#include "gmock/gmock.h"
#include "picolibrary/hsm.h"

/**
 * \brief Hierarchical State Machine (HSM) unit testing facilities.
 */
namespace picolibrary::Testing::Unit::HSM {

/**
 * \brief Mock event category.
 */
class Mock_Event_Category : public ::picolibrary::HSM::Event_Category {
  public:
    /**
     * \brief Get a reference to the mock event category instance.
     *
     * \return A reference to the mock event category instance.
     */
    static auto const & instance()
    {
        static auto category = Mock_Event_Category{};

        return category;
    }

    /**
     * \brief Constructor.
     */
    Mock_Event_Category() = default;

    Mock_Event_Category( Mock_Event_Category && ) = delete;

    Mock_Event_Category( Mock_Event_Category const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Event_Category() noexcept = default;

    auto operator=( Mock_Event_Category && ) = delete;

    auto operator=( Mock_Event_Category const & ) = delete;

    MOCK_METHOD( char const *, name, (), ( const, noexcept, override ) );

    MOCK_METHOD( char const *, event_description, ( ::picolibrary::HSM::Event_ID ), ( const, noexcept, override ) );
};

/**
 * \brief Mock event.
 */
class Mock_Event : public ::picolibrary::HSM::Event {
  public:
    Mock_Event() = delete;

    Mock_Event( Mock_Event && ) = delete;

    Mock_Event( Mock_Event const & ) = delete;

    virtual ~Mock_Event() noexcept override = default;

    auto operator=( Mock_Event && ) = delete;

    auto operator=( Mock_Event const & ) = delete;
};

} // namespace picolibrary::Testing::Unit::HSM

#endif // PICOLIBRARY_TESTING_UNIT_HSM_H
