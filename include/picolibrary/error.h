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
 * \brief picolibrary error interface.
 */

#ifndef PICOLIBRARY_ERROR_H
#define PICOLIBRARY_ERROR_H

#include <cstdint>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Error ID.
 *
 * \attention This type is the expected underlying type for all error code enums.
 */
using Error_ID = std::uint_fast8_t;

/**
 * \brief Check if an enum is an error code enum.
 *
 * \tparam Enum The enum to check.
 */
template<typename Enum>
struct is_error_code_enum : std::false_type {
};

/**
 * \brief Check if an enum is an error code enum.
 *
 * \tparam Enum The enum to check.
 */
template<typename Enum>
constexpr auto is_error_code_enum_v = is_error_code_enum<Enum>::value;

/**
 * \brief Error category.
 */
class Error_Category {
  public:
    Error_Category( Error_Category && ) = delete;

    Error_Category( Error_Category const & ) = delete;

    auto operator=( Error_Category && ) = delete;

    auto operator=( Error_Category const & ) = delete;

    /**
     * \brief Get the name of the error category.
     *
     * \return The name of the error category.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto name() const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    constexpr auto const * name() const noexcept
    {
        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

    /**
     * \brief Get an error ID's description.
     *
     * \param[in] id The error ID whose description is to be got.
     *
     * \return The error ID's description.
     */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    virtual auto error_description( Error_ID id ) const noexcept -> char const * = 0;
#else  // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    constexpr auto const * error_description( Error_ID id ) const noexcept
    {
        static_cast<void>( id );

        return "";
    }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Error_Category() noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Error_Category() noexcept = default;
};

} // namespace picolibrary

#endif // PICOLIBRARY_ERROR_H
