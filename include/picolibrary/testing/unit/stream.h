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
 * \brief Mock output stream.
 */
class Mock_Output_Stream : public Output_Stream {
  public:
    Mock_Output_Stream()
    {
        set_buffer( &m_buffer );
    }

    Mock_Output_Stream( Mock_Output_Stream && ) = delete;

    Mock_Output_Stream( Mock_Output_Stream const & ) = delete;

    ~Mock_Output_Stream() noexcept = default;

    auto operator=( Mock_Output_Stream && ) = delete;

    auto operator=( Mock_Output_Stream const & ) = delete;

    using ::picolibrary::Stream::clear_end_of_file_reached_report;
    using ::picolibrary::Stream::report_end_of_file_reached;

    using ::picolibrary::Stream::clear_fatal_error;
    using ::picolibrary::Stream::report_fatal_error;

    auto & buffer() noexcept
    {
        return m_buffer;
    }

  private:
    Mock_Stream_Buffer m_buffer{};
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

/**
 * \brief Unit testing output string stream.
 */
class Output_String_Stream : public Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Output_String_Stream()
    {
        set_buffer( &m_buffer );
    }

    Output_String_Stream( Output_String_Stream && ) = delete;

    Output_String_Stream( Output_String_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_String_Stream() noexcept = default;

    auto operator=( Output_String_Stream && ) = delete;

    auto operator=( Output_String_Stream const & ) = delete;

    /**
     * \brief Get the string abstracted by the stream.
     *
     * \return The string abstracted by the stream.
     */
    auto const & string() const noexcept
    {
        return m_buffer.string();
    }

  private:
    /**
     * \brief The stream's device access buffer.
     */
    String_Stream_Buffer m_buffer{};
};

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_STREAM_H
