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
 * \brief picolibrary::Testing::Unit stream interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_STREAM_H
#define PICOLIBRARY_TESTING_UNIT_STREAM_H

#include <cstdint>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Mock I/O stream device access buffer.
 */
class Mock_Stream_Buffer : public Stream_Buffer {
  public:
    Mock_Stream_Buffer() = default;

    Mock_Stream_Buffer( Mock_Stream_Buffer && ) = delete;

    Mock_Stream_Buffer( Mock_Stream_Buffer const & ) = delete;

    ~Mock_Stream_Buffer() noexcept = default;

    auto operator=( Mock_Stream_Buffer && ) = delete;

    auto operator=( Mock_Stream_Buffer const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), put, (char), ( noexcept, override ) );
    MOCK_METHOD( (Result<Void, Error_Code>), put, ( std::string ) );

    virtual auto put( char const * begin, char const * end ) noexcept -> Result<Void, Error_Code> override
    {
        return put( std::string{ begin, end } );
    }

    virtual auto put( char const * string ) noexcept -> Result<Void, Error_Code> override
    {
        return put( std::string{ string } );
    }

    MOCK_METHOD( (Result<Void, Error_Code>), put, ( std::uint8_t ), ( noexcept, override ) );
    MOCK_METHOD( (Result<Void, Error_Code>), put, (std::vector<std::uint8_t>));

    virtual auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code> override
    {
        return put( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<Void, Error_Code>), put, ( std::int8_t ), ( noexcept, override ) );
    MOCK_METHOD( (Result<Void, Error_Code>), put, (std::vector<std::int8_t>));

    virtual auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept
        -> Result<Void, Error_Code> override
    {
        return put( std::vector<std::int8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<Void, Error_Code>), flush, (), ( noexcept, override ) );
};

/**
 * \brief Unit testing string stream device access buffer.
 */
class String_Stream_Buffer final : public Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    String_Stream_Buffer() = default;

    String_Stream_Buffer( String_Stream_Buffer && ) = delete;

    String_Stream_Buffer( String_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~String_Stream_Buffer() noexcept = default;

    auto operator=( String_Stream_Buffer && ) = delete;

    auto operator=( String_Stream_Buffer const & ) = delete;

    /**
     * \brief Get the string abstracted by the device access buffer.
     *
     * \return The string abstracted by the device access buffer.
     */
    auto const & string() const noexcept
    {
        return m_string;
    }

    /**
     * \brief Write a character to the string.
     *
     * \param[in] character The character to write to the string.
     *
     * \return Nothing.
     */
    virtual auto put( char character ) noexcept -> Result<Void, Error_Code> override final
    {
        m_string.push_back( character );

        return {};
    }

    /**
     * \brief Write an unsigned byte to the string.
     *
     * \param[in] value The unsigned byte to write to the string.
     *
     * \return Nothing.
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        m_string.push_back( value );

        return {};
    }

    /**
     * \brief Write a signed byte to the string.
     *
     * \param[in] value The signed byte to write to the string.
     *
     * \return Nothing.
     */
    virtual auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        m_string.push_back( value );

        return {};
    }

    /**
     * \brief Do nothing.
     *
     * \return Nothing.
     */
    virtual auto flush() noexcept -> Result<Void, Error_Code> override final
    {
        return {};
    }

  private:
    /**
     * \brief The string abstracted by the device access buffer.
     */
    std::string m_string{};
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_STREAM_H
