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
 * \brief picolibrary event interface.
 */

#ifndef PICOLIBRARY_EVENT_H
#define PICOLIBRARY_EVENT_H

#include <cstdint>

namespace picolibrary {

/**
 * \brief Event ID.
 */
using Event_ID = std::uint_fast8_t;

/**
 * \brief Event category.
 */
class Event_Category {
  public:
    Event_Category( Event_Category && ) = delete;

    Event_Category( Event_Category const & ) = delete;

    auto operator=( Event_Category && ) = delete;

    auto operator=( Event_Category const & ) = delete;

    /**
     * \brief Get the name of the event category.
     *
     * \return The name of the event category.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    virtual auto name() const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    constexpr auto const * name() const noexcept
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

    /**
     * \brief Get an event ID's description.
     *
     * \param[in] id The event ID whose description is to be got.
     *
     * \return The event ID's description.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    virtual auto event_description( Event_ID id ) const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    constexpr auto const * event_description( Event_ID id ) const noexcept
    {
        static_cast<void>( id );

        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Event_Category() noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Event_Category() noexcept = default;
};

} // namespace picolibrary

#endif // PICOLIBRARY_EVENT_H
