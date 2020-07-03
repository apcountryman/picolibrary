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

namespace picolibrary {

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
 *     -> picolibrary::Result<picolibrary::Void, picolibrary::Error_Code>
 *
 * auto functor( auto value ) noexcept
 * -> picolibrary::Result<picolibrary::Void, picolibrary::Void>
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
constexpr auto for_each( Iterator begin, Iterator end, Functor functor ) noexcept
    -> Result<Functor, typename std::invoke_result_t<Functor, decltype( *std::declval<Iterator>() )>::Error>
{
    for ( ; begin != end; ++begin ) {
        auto result = functor( *begin );
        if ( result.is_error() ) { return result.error(); } // if
    }                                                       // for

    return functor;
}

} // namespace picolibrary

#endif // PICOLIBRARY_ALGORITHM_H
