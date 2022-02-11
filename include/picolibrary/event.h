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

#include <cstddef>
#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

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

    /**
     * \brief Get the event's category.
     *
     * \return The event's category.
     */
    constexpr auto const & category() const noexcept
    {
        return *m_category;
    }

    /**
     * \brief Get the event's ID.
     *
     * \return The event's ID.
     */
    constexpr auto id() const noexcept
    {
        return m_id;
    }

    /**
     * \brief Get the event's description.
     *
     * \return The event's description.
     */
    auto const * description() const noexcept
    {
        return category().event_description( id() );
    }

    /**
     * \brief Write the event's details to a stream.
     *
     * \param[in] stream The stream to write the event's details to.
     *
     * \return 0.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    virtual auto print_details( Output_Stream & stream ) const noexcept -> Result<std::size_t, Error_Code>
    {
        static_cast<void>( stream );

        return size_t{ 0 };
    }
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    constexpr auto print_details( Output_Stream & stream ) const noexcept -> Result<std::size_t, Void>
    {
        static_cast<void>( stream );

        return size_t{ 0 };
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

  protected:
    /**
     * \brief Constructor.
     *
     * \param[in] category The event's category.
     * \param[in] id The event's ID.
     */
    constexpr Event( Event_Category const & category, Event_ID id ) noexcept :
        m_category{ &category },
        m_id{ id }
    {
    }

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
    auto operator=( Event && expression ) noexcept -> Event & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Event const & expression ) noexcept -> Event & = default;

  private:
    /**
     * \brief The event's category.
     */
    Event_Category const * m_category{};

    /**
     * \brief The event's ID.
     */
    Event_ID m_id{};
};

/**
 * \brief picolibrary::Event output formatter.
 *
 * picolibrary::Event only supports the default format specification ("{}").
 */
template<>
class Output_Formatter<Event> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the picolibrary::Event to be formatted.
     *
     * \param[in] format The format specification for the picolibrary::Event to be
     *            formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted picolibrary::Event to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Event to.
     * \param[in] event The event to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Event const & event ) noexcept -> Result<std::size_t, Error_Code>
    {
        auto characters_written = std::size_t{};

        {
            auto result = stream.print( "{}::{}", event.category().name(), event.description() );
            if ( result.is_error() ) {
                return result.error();
            } // if

            characters_written += result.value();
        }

        {
            auto result = event.print_details( stream );
            if ( result.is_error() ) {
                return result.error();
            } // if

            characters_written += result.value();
        }

        return characters_written;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_EVENT_H
