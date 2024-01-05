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
 * \brief picolibrary::Testing::Automated event interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_EVENT_H
#define PICOLIBRARY_TESTING_AUTOMATED_EVENT_H

#include <cstddef>
#include <ios>
#include <ostream>
#include <typeinfo>

#include "gmock/gmock.h"
#include "picolibrary/event.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Mock event category.
 */
class Mock_Event_Category : public Event_Category {
  public:
    static auto instance() -> Mock_Event_Category const &
    {
        static auto const category = Mock_Event_Category{};

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

/**
 * \brief Mock event.
 */
class Mock_Event : public Event {
  public:
    Mock_Event( Mock_Event_Category const & category, Event_ID id ) :
        Event{ category, id }
    {
    }

    Mock_Event( Mock_Event && ) = delete;

    Mock_Event( Mock_Event const & ) = delete;

    ~Mock_Event() noexcept override = default;

    auto operator=( Mock_Event && ) = delete;

    auto operator=( Mock_Event const & ) = delete;

    MOCK_METHOD( (Result<std::size_t>), print_details, (Output_Stream &), ( const, noexcept, override ) );

    MOCK_METHOD( std::size_t, print_details, (Reliable_Output_Stream &), ( const, noexcept, override ) );

    void print_details( std::ostream & stream ) const override
    {
        static_cast<void>( stream );
    }
};

} // namespace picolibrary::Testing::Automated

namespace picolibrary {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Event to.
 * \param[in] event The picolibrary::Event to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Event const & event ) -> std::ostream &
{
    if ( &event.category() == &Testing::Automated::Mock_Event_Category::instance() ) {
        return stream << "::picolibrary::Testing::Automated::Mock_Event::" << std::dec
                      << static_cast<std::uint_fast16_t>( event.id() );
    } // if

    if ( typeid( event.category() ) == typeid( Testing::Automated::Mock_Event_Category ) ) {
        return stream << "::picolibrary::Testing::Automated::Mock_Event( " << &event.category()
                      << " )::" << std::dec << static_cast<std::uint_fast16_t>( event.id() );
    } // if

    stream << event.category().name() << "::" << event.description();

    event.print_details( stream );

    return stream;
}

} // namespace picolibrary

#endif // PICOLIBRARY_TESTING_AUTOMATED_EVENT_H
