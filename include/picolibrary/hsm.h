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
 * \brief picolibrary::HSM interface.
 */

#ifndef PICOLIBRARY_HSM_H
#define PICOLIBRARY_HSM_H

#include <cstdint>

/**
 * \brief Hierarchical State Machine (HSM) facilities.
 */
namespace picolibrary::HSM {

/**
 * \brief Event ID number.
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
     * \return The name of the event category if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION is not defined.
     * \return An empty string if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION is defined.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION
    virtual auto name() const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION
    auto const * name() const noexcept
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION

    /**
     * \brief Get an event ID's description.
     *
     * \param[in] id The event ID whose description is to be got.
     *
     * \return The event ID's description if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION is not defined.
     * \return An empty string if
     *         PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION is defined.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION
    virtual auto event_description( Event_ID id ) const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION
    auto const * name() const noexcept
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_HSM_EVENT_INFORMATION

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

/**
 * \brief Event.
 */
class Event {
  public:
    Event() = delete;

    /**
     * \brief Destructor.
     */
    virtual ~Event() noexcept = default;

  protected:
    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Event( Event && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Event( Event const & original ) noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Event && expression ) noexcept -> Event & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Event const & expression ) noexcept -> Event & = default;
};

} // namespace picolibrary::HSM

#endif // PICOLIBRARY_HSM_H
