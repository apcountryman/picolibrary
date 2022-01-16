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
 * \brief picolibrary algorithm interface.
 */

#ifndef PICOLIBRARY_ALGORITHM_H
#define PICOLIBRARY_ALGORITHM_H

#include <type_traits>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief Functor can fail algorithm policy.
 */
struct Functor_Can_Fail {
};

/**
 * \brief Functor can fail, return functor algorithm policy.
 */
struct Functor_Can_Fail_Return_Functor {
};

/**
 * \brief Functor can fail, discard functor algorithm policy.
 */
struct Functor_Can_Fail_Discard_Functor {
};

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference.
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return The functor.
 */
template<typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor ) noexcept
{
    for ( ; begin != end; ++begin ) { functor( *begin ); } // for

    return functor;
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts and
 *         the error is returned.
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return The functor if application of the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor, Functor_Can_Fail_Return_Functor ) noexcept
    -> Result<Functor, typename std::invoke_result_t<Functor, decltype( *std::declval<Iterator>() )>::Error>
{
    for ( ; begin != end; ++begin ) {
        auto result = functor( *begin );
        if ( result.is_error() ) {
            return result.error();
        } // if
    }     // for

    return functor;
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts and
 *         the error is returned.
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return Nothing if application of the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor, Functor_Can_Fail_Discard_Functor ) noexcept
    -> Result<Void, typename std::invoke_result_t<Functor, decltype( *std::declval<Iterator>() )>::Error>
{
    for ( ; begin != end; ++begin ) {
        auto result = functor( *begin );
        if ( result.is_error() ) {
            return result.error();
        } // if
    }     // for

    return {};
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Policy The algorithm policy to use (
 *         picolibrary::Functor_Can_Fail_Return_Functor or
 *         picolibrary::Functor_Can_Fail_Discard_Functor).
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts and
 *         the error is returned.
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return The functor if Policy is picolibrary::Functor_Can_Fail_Return_Functor and
 *         application of the functor to the range succeeded.
 * \return Nothing if Policy is picolibrary::Functor_Can_Fail_Discard_Functor and
 *         application of the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Policy, typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor ) noexcept
{
    return ::picolibrary::for_each( begin, end, std::move( functor ), Policy{} );
}

/**
 * \brief Get the minimum of two values.
 *
 * \tparam T The type to compare.
 *
 * \param[in] a The first of the two values to compare.
 * \param[in] b The second of the two values to compare.
 *
 * \return The minimum of the two values. If the two values are equal, the first argument
 *         will be returned.
 */
template<typename T>
constexpr auto const & min( T const & a, T const & b ) noexcept
{
    return b < a ? b : a;
}

} // namespace picolibrary

#endif // PICOLIBRARY_ALGORITHM_H
