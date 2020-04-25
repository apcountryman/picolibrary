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
 * \brief picolibrary::Result interface.
 */

#ifndef PICOLIBRARY_RESULT_H
#define PICOLIBRARY_RESULT_H

#include <type_traits>

#include "picolibrary/utility.h"

namespace picolibrary {

/**
 * \brief Operation result wrapper.
 *
 * \tparam Value_Type Operation succeeded result type.
 * \tparam Error_Type Operation failed result type.
 */
template<typename Value_Type, typename Error_Type, bool = std::is_trivially_destructible_v<Value_Type>>
class Result;

/**
 * \brief Operation result wrapper specialized for cases where no information is
 *        generated, and the operation cannot fail.
 */
template<>
class [[nodiscard]] Result<Void, Void, true>
{
  public:
    /**
     * \brief Operation succeeded result type.
     */
    using Value = Void;

    /**
     * \brief Operation failed result type.
     */
    using Error = Void;

    /**
     * \brief Constructor.
     */
    constexpr Result() noexcept = default;

    /**
     * \brief Constructor.
     */
    constexpr Result( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Result( Result && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Result( Result const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Result() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Result && expression ) noexcept->Result & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Result const & expression ) noexcept->Result & = default;

    /**
     * \brief Check if the operation result is a value (operation succeeded).
     *
     * \return true (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_value() const noexcept
    {
        return true;
    }

    /**
     * \brief Check if the operation result is an error (operation failed).
     *
     * \return false (operation succeeded).
     */
    [[nodiscard]] constexpr auto is_error() const noexcept
    {
        return false;
    }

    /**
     * \brief Access the result of a failed operation.
     *
     * \attention This function is only provided for interface consistency.
     *
     * \return picolibrary::Void.
     */
    [[nodiscard]] constexpr auto error() const noexcept
    {
        return Void{};
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_RESULT_H
