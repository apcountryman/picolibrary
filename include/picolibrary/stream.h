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
 * \brief picolibrary::Stream interface.
 */

#ifndef PICOLIBRARY_STREAM_H
#define PICOLIBRARY_STREAM_H

namespace picolibrary {

/**
 * \brief I/O stream device access buffer.
 */
class Stream_Buffer {
  public:
    /**
     * \brief Destructor.
     */
    ~Stream_Buffer() noexcept = default;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Stream_Buffer() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Stream_Buffer( Stream_Buffer && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Stream_Buffer( Stream_Buffer const & original ) noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Stream_Buffer && expression ) noexcept -> Stream_Buffer & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Stream_Buffer const & expression ) noexcept -> Stream_Buffer & = default;
};

} // namespace picolibrary

#endif // PICOLIBRARY_STREAM_H
