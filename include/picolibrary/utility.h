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
 * \brief picolibrary::Utility interface.
 */

#ifndef PICOLIBRARY_UTILITY_H
#define PICOLIBRARY_UTILITY_H

namespace picolibrary {

/**
 * \brief Replacement for void in contexts that require construction.
 */
struct Void {
};

} // namespace picolibrary

#endif // PICOLIBRARY_UTILITY_H
