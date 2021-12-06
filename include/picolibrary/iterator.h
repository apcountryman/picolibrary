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

} // namespace picolibrary

#endif // PICOLIBRARY_ITERATOR_H
