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
 * \brief picolibrary::Error interface.
 */

#ifndef PICOLIBRARY_ERROR_H
#define PICOLIBRARY_ERROR_H

#include <cstdint>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Error ID number.
 *
 * \attention This type is the underlying integer type for all error code enums.
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
 * \copydoc picolibrary::is_error_code_enum
 */
template<typename Enum>
constexpr auto is_error_code_enum_v = is_error_code_enum<Enum>::value;

/**
 * \brief Error category.
 */
class Error_Category {
public:
    /**
     * \todo #29
     */
    Error_Category( Error_Category && ) = delete;

    /**
     * \todo #29
     */
    Error_Category( Error_Category const & ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Error_Category && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Error_Category const & ) = delete;

    /**
     * \brief Get the name of the error category.
     *
     * \return The name of the error category.
     */
    virtual auto name() const noexcept -> char const * = 0;

    /**
     * \brief Get an error ID's description.
     *
     * \param[in] id The error ID whose description is to be got.
     *
     * \return The error ID's description.
     */
    virtual auto error_description( Error_ID id ) const noexcept -> char const * = 0;

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

/**
 * \brief Error code.
 */
class Error_Code {
public:
    /**
     * \brief Constructor.
     */
    constexpr Error_Code() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Error_Code( Error_Code && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Error_Code( Error_Code const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Error_Code() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Error_Code && expression ) noexcept -> Error_Code & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Error_Code const & expression ) noexcept -> Error_Code & = default;
};

} // namespace picolibrary

#endif // PICOLIBRARY_ERROR_H
