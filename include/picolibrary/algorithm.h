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
 * \brief picolibrary::Algorithm interface.
 */

#ifndef PICOLIBRARY_ALGORITHM_H
#define PICOLIBRARY_ALGORITHM_H

#include <type_traits>
#include <utility>

#include "picolibrary/result.h"
#include "picolibrary/utility.h"

namespace picolibrary {

/**
 * \brief Return functor algorithm policy.
 */
struct Return_Functor {
};

/**
 * \brief Discard functor algorithm policy.
 */
struct Discard_Functor {
};

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts,
 *         and the error is returned. Illustrative signatures:
 * \code
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 * \endcode
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return The functor if application of the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor, Return_Functor ) noexcept
    -> Result<Functor, typename std::invoke_result_t<Functor, decltype( *std::declval<Iterator>() )>::Error>
{
    for ( ; begin != end; ++begin ) {
        auto result = functor( *begin );
        if ( result.is_error() ) { return result.error(); } // if
    }                                                       // for

    return functor;
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts,
 *         and the error is returned. Illustrative signatures:
 * \code
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 * \endcode
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return Nothing if application of the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor, Discard_Functor ) noexcept
    -> Result<Void, typename std::invoke_result_t<Functor, decltype( *std::declval<Iterator>() )>::Error>
{
    for ( ; begin != end; ++begin ) {
        auto result = functor( *begin );
        if ( result.is_error() ) { return result.error(); } // if
    }                                                       // for

    return {};
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Functor_Policy The functor policy (either picolibrary::Return_Functor, or
 *         picolibrary::Discard_Functor) to use.
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns either picolibrary::Result<picolibrary::Void,
 *         picolibrary::Error_Code> or picolibrary::Result<picolibrary::Void,
 *         picolibrary::Void>. If an error is returned by the functor, iteration halts,
 *         and the error is returned. Illustrative signatures:
 * \code
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>;
 *
 * auto functor( auto value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 *
 * auto functor( auto const & value ) noexcept
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Void>;
 * \endcode
 *
 * \param[in] begin The beginning of the range to apply the functor to.
 * \param[in] end The end of the range to apply the functor to.
 * \param[in] functor The functor to apply to the range.
 *
 * \return The functor if Functor_Policy is picolibrary::Return_Functor and application of
 *         the functor to the range succeeded.
 * \return Nothing if Functor_Policy is picolibrary::Discard_Functor and application of
 *         the functor to the range succeeded.
 * \return An error code if application of the functor to the range failed.
 */
template<typename Functor_Policy, typename Iterator, typename Functor>
constexpr auto for_each( Iterator begin, Iterator end, Functor functor ) noexcept
{
    return for_each( begin, end, std::move( functor ), Functor_Policy{} );
}

/**
 * \brief Fill a range with values generated by a functor.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A nullary functor that returns either picolibrary::Result<Foo,
 *         picolibrary::Error_Code> or picolibrary::Result<Foo, picolibrary::Void> where
 *         Foo is a type that can be assigned to a dereferenced Iterator. If an error is
 *         returned by the functor, iteration halts, and the error is returned.
 *         Illustrative signatures:
 * \code
 * auto functor() noexcept -> picolibrary::Result<Foo, picolibrary::Error_Code>;
 *
 * auto functor() noexcept -> picolibrary::Result<Foo, picolibrary::Void>;
 * \endcode
 *
 * \param[out] begin The beginning of the range to fill.
 * \param[out] end The end of the range to fill.
 * \param[in] functor The functor to use to generate the values used to fill the range.
 *
 * \return Nothing if filling the range succeeded.
 * \return An error code if filling the range failed.
 */
template<typename Iterator, typename Functor>
constexpr auto generate( Iterator begin, Iterator end, Functor functor ) noexcept
    -> Result<Void, typename std::invoke_result_t<Functor>::Error>
{
    while ( begin != end ) {
        auto result = functor();
        if ( result.is_error() ) { return result.error(); } // if

        *begin++ = result.value();
    } // for

    return {};
}

} // namespace picolibrary

#endif // PICOLIBRARY_ALGORITHM_H
