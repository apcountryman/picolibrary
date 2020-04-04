/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Testing::Unit::Error interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ERROR_H
#define PICOLIBRARY_TESTING_UNIT_ERROR_H

#include <type_traits>

#include "gmock/gmock.h"
#include "picolibrary/error.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock errors.
 */
enum class Mock_Error : Error_ID {};

} // namespace picolibrary::Testing::Unit

namespace picolibrary {

/**
 * \brief picolibrary::Testing::Unit::Mock_Error error code enum registration.
 */
template<>
struct is_error_code_enum<Testing::Unit::Mock_Error> : std::true_type {
};

} // namespace picolibrary

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock error category.
 */
class Mock_Error_Category : public Error_Category {
public:
    /**
     * \brief Constructor.
     */
    Mock_Error_Category() = default;

    /**
     * \todo #29
     */
    Mock_Error_Category( Mock_Error_Category && ) = delete;

    /**
     * \todo #29
     */
    Mock_Error_Category( Mock_Error_Category const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Error_Category() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Error_Category && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Error_Category const & ) = delete;

    MOCK_METHOD( char const *, name, (), ( const, noexcept, override ) );

    MOCK_METHOD( char const *, error_description, ( Error_ID ), ( const, noexcept, override, final ) );
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_ERROR_H
