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
 * \brief picolibrary::Testing::Unit::Functor interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_FUNCTOR_H
#define PICOLIBRARY_TESTING_UNIT_FUNCTOR_H

#include <utility>

#include "gmock/gmock.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock nullary functor.
 *
 * \tparam Return_Type The functor's return type.
 */
template<typename Return_Type>
class Mock_Nullary_Functor {
  public:
    /**
     * \brief Movable, copyable mock nullary functor handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_nullary_functor The mock nullary functor.
         */
        Handle( Mock_Nullary_Functor const & mock_nullary_functor ) noexcept :
            m_mock_nullary_functor{ &mock_nullary_functor }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] original The original to copy.
         */
        Handle( Handle const & original ) noexcept = default;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle const & expression ) noexcept -> Handle & = default;

        /**
         * \brief Get the mock nullary functor.
         *
         * \return The mock nullary functor.
         */
        auto const & mock() const noexcept
        {
            return *m_mock_nullary_functor;
        }

        /**
         * \brief Call operator.
         *
         * \return The functor result.
         */
        auto operator()() const
        {
            return m_mock_nullary_functor->call();
        }

      private:
        /**
         * \brief The mock nullary functor.
         */
        Mock_Nullary_Functor const * m_mock_nullary_functor{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Nullary_Functor() = default;

    /**
     * \todo #29
     */
    Mock_Nullary_Functor( Mock_Nullary_Functor && ) = delete;

    /**
     * \todo #29
     */
    Mock_Nullary_Functor( Mock_Nullary_Functor const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Nullary_Functor() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Nullary_Functor && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Nullary_Functor const & ) = delete;

    /**
     * \brief Get a movable, copyable handle to the mock nullary functor.
     *
     * \return A movable, copyable handle to the mock nullary functor.
     */
    auto handle() const noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( Return_Type, call, (), ( const ) );

    /**
     * \brief Call operator.
     *
     * \return The functor result.
     */
    auto operator()() const
    {
        return call();
    }
};

/**
 * \brief Mock functor.
 *
 * \tparam Return_Type The functor's return type.
 * \tparam Argument_Types The functor's argument types.
 */
template<typename Return_Type, typename... Argument_Types>
class Mock_Functor {
  public:
    /**
     * \brief Movable, copyable mock functor handle.
     */
    class Handle {
      public:
        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_functor The mock functor.
         */
        Handle( Mock_Functor const & mock_functor ) noexcept :
            m_mock_functor{ &mock_functor }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] original The original to copy.
         */
        Handle( Handle const & original ) noexcept = default;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle && expression ) noexcept -> Handle & = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto operator=( Handle const & expression ) noexcept -> Handle & = default;

        /**
         * \brief Get the mock functor.
         *
         * \return The mock functor.
         */
        auto const & mock() const noexcept
        {
            return *m_mock_functor;
        }

        /**
         * \brief Call operator.
         *
         * \tparam Arguments Functor argument types.
         *
         * \param[in] arguments The arguments to the functor.
         *
         * \return The functor result.
         */
        template<typename... Arguments>
        auto operator()( Arguments &&... arguments ) const
        {
            return m_mock_functor->call( std::forward<Arguments>( arguments )... );
        }

      private:
        /**
         * \brief The mock functor.
         */
        Mock_Functor const * m_mock_functor{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Functor() = default;

    /**
     * \todo #29
     */
    Mock_Functor( Mock_Functor && ) = delete;

    /**
     * \todo #29
     */
    Mock_Functor( Mock_Functor const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Functor() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Functor && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Functor const & ) = delete;

    /**
     * \brief Get a movable, copyable handle to the mock functor.
     *
     * \return A movable, copyable handle to the mock functor.
     */
    auto handle() const noexcept
    {
        return Handle{ *this };
    }

    MOCK_METHOD( Return_Type, call, ( Argument_Types... ), ( const ) );

    /**
     * \brief Call operator.
     *
     * \tparam Arguments Functor argument types.
     *
     * \param[in] arguments The arguments to the functor.
     *
     * \return The functor result.
     */
    template<typename... Arguments>
    auto operator()( Arguments &&... arguments ) const
    {
        return call( std::forward<Arguments>( arguments )... );
    }
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_FUNCTOR_H
