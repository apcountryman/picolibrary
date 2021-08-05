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
 * \brief picolibrary::Testing::Unit::Error interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_ERROR_H
#define PICOLIBRARY_TESTING_UNIT_ERROR_H

#include <type_traits>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock errors.
 *
 * \relatedalso picolibrary::Testing::Unit::Mock_Error_Category
 */
enum class Mock_Error : Error_ID {};

/**
 * \brief Generate a pseudo-random picolibrary::Testing::Unit::Mock_Error.
 *
 * \relatedalso picolibrary::Testing::Unit::Mock_Error_Category
 *
 * \return A pseudo-randomly generated picolibrary::Testing::Unit::Mock_Error.
 */
template<>
inline auto random<Mock_Error>()
{
    return static_cast<Mock_Error>( random<Error_ID>() );
}

/**
 * \brief Mock error category.
 */
class Mock_Error_Category : public Error_Category {
  public:
    /**
     * \brief Get a reference to the mock error category instance.
     *
     * \return A reference to the mock error category instance.
     */
    static auto const & instance()
    {
        static auto category = Mock_Error_Category{};

        return category;
    }

    /**
     * \brief Constructor.
     */
    Mock_Error_Category();

    Mock_Error_Category( Mock_Error_Category && ) = delete;

    Mock_Error_Category( Mock_Error_Category const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Error_Category() noexcept;

    auto operator=( Mock_Error_Category && ) = delete;

    auto operator=( Mock_Error_Category const & ) = delete;

    MOCK_METHOD( char const *, name, (), ( const, noexcept, override ) );

    MOCK_METHOD( char const *, error_description, ( Error_ID ), ( const, noexcept, override ) );
};

/**
 * \brief Build an error code from a mock error.
 *
 * \relatedalso picolibrary::Testing::Unit::Mock_Error_Category
 *
 * \param[in] error The mock error to build the error code from.
 *
 * \return The built error code.
 */
inline auto make_error_code( Mock_Error error ) noexcept
{
    return Error_Code{ Mock_Error_Category::instance(), static_cast<Error_ID>( error ) };
}

} // namespace picolibrary::Testing::Unit

namespace picolibrary {

/**
 * \brief picolibrary::Testing::Unit::Mock_Error error code enum registration.
 *
 * \relatedalso picolibrary::Testing::Unit::Mock_Error_Category
 */
template<>
struct is_error_code_enum<Testing::Unit::Mock_Error> : std::true_type {
};

} // namespace picolibrary

#endif // PICOLIBRARY_TESTING_UNIT_ERROR_H
