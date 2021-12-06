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
 * \brief picolibrary::Iterator interface.
 */

#ifndef PICOLIBRARY_ITERATOR_H
#define PICOLIBRARY_ITERATOR_H

#include <cstddef>
#include <memory>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Input iterator category tag.
 */
struct Input_Iterator_Tag {
};

/**
 * \brief Output iterator category tag.
 */
struct Output_Iterator_Tag {
};

/**
 * \brief Forward iterator category tag.
 */
struct Forward_Iterator_Tag : Input_Iterator_Tag {
};

/**
 * \brief Bidirectional iterator category tag.
 */
struct Bidirectional_Iterator_Tag : Forward_Iterator_Tag {
};

/**
 * \brief Random access iterator category tag.
 */
struct Random_Access_Iterator_Tag : Bidirectional_Iterator_Tag {
};

/**
 * \brief Contiguous iterator category tag.
 */
struct Contiguous_Iterator_Tag : Random_Access_Iterator_Tag {
};

/**
 * \brief Iterator traits.
 *
 * \tparam Iterator The iterator type whose traits will be retrieved.
 */
template<typename Iterator, typename = std::void_t<>>
struct Iterator_Traits {
};

/**
 * \brief Iterator traits.
 *
 * \tparam Iterator The iterator type whose traits will be retrieved.
 */
template<typename Iterator>
struct Iterator_Traits<Iterator, std::void_t<typename Iterator::Difference, typename Iterator::Value, typename Iterator::Pointer, typename Iterator::Reference, typename Iterator::Iterator_Category>> {
    /**
     * \brief Type that can be used to identify the distance between two iterators.
     */
    using Difference = typename Iterator::Difference;

    /**
     * \brief The iterated over type.
     */
    using Value = typename Iterator::Value;

    /**
     * \brief Pointer to the iterated over type.
     */
    using Pointer = typename Iterator::Pointer;

    /**
     * \brief Reference to the iterated over type.
     */
    using Reference = typename Iterator::Reference;

    /**
     * \brief Iterator category tag.
     */
    using Iterator_Category = typename Iterator::Iterator_Category;
};

/**
 * \brief Pointer iterator traits.
 *
 * \tparam T The iterated over type.
 */
template<typename T>
struct Iterator_Traits<T *> {
    /**
     * \brief Type that can be used to identify the distance between two iterators.
     */
    using Difference = std::ptrdiff_t;

    /**
     * \brief The iterated over type.
     */
    using Value = std::remove_cv_t<T>;

    /**
     * \brief Pointer to the iterated over type.
     */
    using Pointer = T *;

    /**
     * \brief Reference to the iterated over type.
     */
    using Reference = T &;

    /**
     * \brief Iterator category tag.
     */
    using Iterator_Category = Contiguous_Iterator_Tag;
};

} // namespace picolibrary

#endif // PICOLIBRARY_ITERATOR_H
