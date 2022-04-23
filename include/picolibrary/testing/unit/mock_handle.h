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
 * \brief picolibrary::Testing::Unit::Mock_Handle interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MOCK_HANDLE_H
#define PICOLIBRARY_TESTING_UNIT_MOCK_HANDLE_H

namespace picolibrary::Testing::Unit {

/**
 * \brief Movable mock handle.
 *
 * \tparam Mock The type of mock the handle is a handle to.
 */
template<typename Mock>
class Mock_Handle {
  public:
    Mock_Handle( Mock_Handle const & ) = delete;

    auto operator=( Mock_Handle const & ) = delete;

    /**
     * \brief Get the mock.
     *
     * \return The mock.
     */
    auto mock() noexcept -> Mock &
    {
        return *m_mock;
    }

    /**
     * \brief Get the mock.
     *
     * \return The mock.
     */
    auto mock() const noexcept -> Mock const &
    {
        return *m_mock;
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Mock_Handle() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] mock The mock.
     */
    constexpr Mock_Handle( Mock & mock ) noexcept : m_mock{ &mock }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Mock_Handle( Mock_Handle && source ) noexcept : m_mock{ source.m_mock }
    {
        source.m_mock = nullptr;
    }

    /**
     * \brief Destructor.
     */
    ~Mock_Handle() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Mock_Handle && expression ) noexcept -> Mock_Handle &
    {
        if ( &expression != this ) {
            m_mock = expression.m_mock;

            expression.m_mock = nullptr;
        } // if

        return *this;
    }

  private:
    /**
     * \brief The mock.
     */
    Mock * m_mock{};
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_MOCK_HANDLE_H
