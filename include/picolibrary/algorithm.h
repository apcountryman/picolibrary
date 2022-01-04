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
 * \brief picolibrary::Algorithm interface.
 */

#ifndef PICOLIBRARY_ALGORITHM_H
#define PICOLIBRARY_ALGORITHM_H

#include <type_traits>
#include <utility>

#include "picolibrary/result.h"
#include "picolibrary/void.h"

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

/**
 * \brief Get the maximum of two values.
 *
 * \tparam T The type to compare.
 *
 * \param[in] a The first of the two values to compare.
 * \param[in] b The second of the two values to compare.
 *
 * \return The maximum of the two values. If the two values are equal, the first argument
 *         will be returned.
 */
template<typename T>
constexpr auto const & max( T const & a, T const & b ) noexcept
{
    return b > a ? b : a;
}

/**
 * Check if two ranges are equal.
 *
 * \tparam Iterator_1 Range iterator for the first of the two ranges to be compared.
 * \tparam Iterator_2 Range iterator for the second of the two ranges to be compared.
 *
 * \param[in] begin_1 The beginning of the first of the two ranges to be compared.
 * \param[in] end_1 The end of the first of the two ranges to be compared.
 * \param[in] begin_2 The beginning of the second of the two ranges to be compared.
 *
 * \return true if the two ranges are equal.
 * \return false if the two ranges are not equal.
 */
template<typename Iterator_1, typename Iterator_2>
constexpr auto equal( Iterator_1 begin_1, Iterator_1 end_1, Iterator_2 begin_2 ) noexcept -> bool
{
    for ( ; begin_1 != end_1; ++begin_1, ++begin_2 ) {
        if ( *begin_1 != *begin_2 ) {
            return false;
        } // if
    }     // for

    return true;
}

/**
 * Check if two ranges are equal.
 *
 * \tparam Iterator_1 Range iterator for the first of the two ranges to be compared.
 * \tparam Iterator_2 Range iterator for the second of the two ranges to be compared.
 *
 * \param[in] begin_1 The beginning of the first of the two ranges to be compared.
 * \param[in] end_1 The end of the first of the two ranges to be compared.
 * \param[in] begin_2 The beginning of the second of the two ranges to be compared.
 * \param[in] end_2 The end of the second of the two ranges to be compared.
 *
 * \return true if the two ranges are equal.
 * \return false if the two ranges are not equal.
 */
template<typename Iterator_1, typename Iterator_2>
constexpr auto equal( Iterator_1 begin_1, Iterator_1 end_1, Iterator_2 begin_2, Iterator_2 end_2 ) noexcept
    -> bool
{
    if ( ( end_2 - begin_2 ) != ( end_1 - begin_1 ) ) {
        return false;
    } // if

    return ::picolibrary::equal( begin_1, end_1, begin_2 );
}

/**
 * Check if two ranges are equal.
 *
 * \tparam Iterator_1 Range iterator for the first of the two ranges to be compared.
 * \tparam Iterator_2 Range iterator for the second of the two ranges to be compared.
 * \tparam Predicate A binary predicate that takes two values to be compared by value or
 *         const reference, and reports whether they are are equal. Illustrative
 *         signatures:
 * \code
 * auto predicate( auto a, auto b ) noexcept -> bool;
 *
 * auto predicate( auto const & a, auto const & b ) noexcept -> bool;
 * \endcode
 *
 * \param[in] begin_1 The beginning of the first of the two ranges to be compared.
 * \param[in] end_1 The end of the first of the two ranges to be compared.
 * \param[in] begin_2 The beginning of the second of the two ranges to be compared.
 * \param[in] predicate The predicate to use to compare values.
 *
 * \return true if the two ranges are equal.
 * \return false if the two ranges are not equal.
 */
template<typename Iterator_1, typename Iterator_2, typename Predicate>
constexpr auto equal( Iterator_1 begin_1, Iterator_1 end_1, Iterator_2 begin_2, Predicate predicate ) noexcept
    -> bool
{
    for ( ; begin_1 != end_1; ++begin_1, ++begin_2 ) {
        if ( not predicate( *begin_1, *begin_2 ) ) {
            return false;
        } // if
    }     // for

    return true;
}

/**
 * Check if two ranges are equal.
 *
 * \tparam Iterator_1 Range iterator for the first of the two ranges to be compared.
 * \tparam Iterator_2 Range iterator for the second of the two ranges to be compared.
 * \tparam Predicate A binary predicate that takes two values to be compared by value or
 *         const reference, and reports whether they are are equal. Illustrative
 *         signatures:
 * \code
 * auto predicate( auto a, auto b ) noexcept -> bool;
 *
 * auto predicate( auto const & a, auto const & b ) noexcept -> bool;
 * \endcode
 *
 * \param[in] begin_1 The beginning of the first of the two ranges to be compared.
 * \param[in] end_1 The end of the first of the two ranges to be compared.
 * \param[in] begin_2 The beginning of the second of the two ranges to be compared.
 * \param[in] end_2 The end of the second of the two ranges to be compared.
 * \param[in] predicate The predicate to use to compare values.
 *
 * \return true if the two ranges are equal.
 * \return false if the two ranges are not equal.
 */
template<typename Iterator_1, typename Iterator_2, typename Predicate>
constexpr auto equal( Iterator_1 begin_1, Iterator_1 end_1, Iterator_2 begin_2, Iterator_2 end_2, Predicate predicate ) noexcept
    -> bool
{
    if ( ( end_2 - begin_2 ) != ( end_1 - begin_1 ) ) {
        return false;
    } // if

    return ::picolibrary::equal( begin_1, end_1, begin_2, std::move( predicate ) );
}

/**
 * \brief Apply a functor to a range.
 *
 * \tparam Iterator Range iterator.
 * \tparam Functor A unary functor that takes the iterated over type by value or const
 *         reference, and returns nothing. Illustrative signatures:
 * \code
 * void functor( auto value ) noexcept;
 *
 * void functor( auto const & value ) noexcept;
 * \endcode
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
        if ( result.is_error() ) {
            return result.error();
        } // if
    }     // for

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
        if ( result.is_error() ) {
            return result.error();
        } // if

        *begin++ = result.value();
    } // for

    return {};
}

} // namespace picolibrary

#endif // PICOLIBRARY_ALGORITHM_H
