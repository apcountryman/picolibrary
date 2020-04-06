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

#include "picolibrary/utility.h"

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
     */
    constexpr Error_Code( Void ) noexcept
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] category The error's category.
     * \param[in] id The error's ID.
     */
    constexpr Error_Code( Error_Category const & category, Error_ID id ) noexcept :
        m_category{&category},
        m_id{id}
    {
    }

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

    /**
     * \brief Check if the picolibrary::Error_Code identifies as an error (i.e. was not
     *        default constructed, and was not constructed from picolibrary::Void).
     *
     * \return true if the picolibrary::Error_Code identifies as an error.
     * \return false if the picolibrary::Error_Code does not identify as an error.
     */
    constexpr explicit operator bool() const noexcept
    {
        return m_category != &Default_Error_Category::instance();
    }

    /**
     * \brief Get the error's category.
     *
     * \return The error's category.
     */
    constexpr auto const & category() const noexcept
    {
        return *m_category;
    }

    /**
     * \brief Get the error's ID.
     *
     * \return The error's ID.
     */
    constexpr auto id() const noexcept
    {
        return m_id;
    }

    /**
     * \brief Get the error's description.
     *
     * \return The error's description.
     */
    auto description() const noexcept
    {
        return category().error_description( id() );
    }

  private:
    /**
     * \brief Default error category.
     */
    class Default_Error_Category : public Error_Category {
      public:
        /**
         * \brief Get a reference to the default error category instance.
         *
         * \return A reference to the default error category instance.
         */
        static constexpr auto instance() noexcept -> Default_Error_Category const &
        {
            return INSTANCE;
        }

        /**
         * \todo #29
         */
        Default_Error_Category( Default_Error_Category && ) = delete;

        /**
         * \todo #29
         */
        Default_Error_Category( Default_Error_Category const & ) = delete;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Default_Error_Category && ) = delete;

        /**
         * \todo #29
         *
         * \return
         */
        auto operator=( Default_Error_Category const & ) = delete;

        /**
         * \copydoc picolibrary::Error_Category::name()
         */
        virtual auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::Default_Error";
        }

        /**
         * \copydoc picolibrary::Error_Category::error_description()
         */
        virtual auto error_description( Error_ID id ) const noexcept -> char const * override final
        {
            static_cast<void>( id );

            return "NONE";
        }

      private:
        /**
         * \brief The default error category instance.
         */
        static Default_Error_Category const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Default_Error_Category() noexcept = default;

        /**
         * \brief Destructor.
         */
        ~Default_Error_Category() noexcept = default;
    };

    /**
     * \brief The error's category.
     */
    Error_Category const * m_category{&Default_Error_Category::instance()};

    /**
     * \brief The error's ID.
     */
    Error_ID m_id{};
};

} // namespace picolibrary

#endif // PICOLIBRARY_ERROR_H
