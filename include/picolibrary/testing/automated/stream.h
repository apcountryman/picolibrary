/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Automated stream interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_STREAM_H
#define PICOLIBRARY_TESTING_AUTOMATED_STREAM_H

#include <cstdint>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"

namespace picolibrary::Testing::Automated {

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

    MOCK_METHOD( (Result<void>), put, (char), ( noexcept, override ) );
    MOCK_METHOD( (Result<void>), put, ( std::string ) );

    auto put( char const * begin, char const * end ) noexcept -> Result<void> override
    {
        return put( std::string{ begin, end } );
    }

    auto put( char const * string ) noexcept -> Result<void> override
    {
        return put( std::string{ string } );
    }

    MOCK_METHOD( (Result<void>), put, ( std::uint8_t ), ( noexcept, override ) );
    MOCK_METHOD( (Result<void>), put, (std::vector<std::uint8_t>));

    auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept -> Result<void> override
    {
        return put( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<void>), put, ( std::int8_t ), ( noexcept, override ) );
    MOCK_METHOD( (Result<void>), put, (std::vector<std::int8_t>));

    auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept -> Result<void> override
    {
        return put( std::vector<std::int8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<void>), flush, (), ( noexcept, override ) );
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

    auto buffer() noexcept -> Mock_Stream_Buffer &
    {
        return m_buffer;
    }

  private:
    Mock_Stream_Buffer m_buffer{};
};

/**
 * \brief Automated testing string stream device access buffer.
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
    auto string() const noexcept -> std::string const &
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
    auto put( char character ) noexcept -> Result<void> override final
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
    auto put( std::uint8_t value ) noexcept -> Result<void> override final
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
    auto put( std::int8_t value ) noexcept -> Result<void> override final
    {
        m_string.push_back( value );

        return {};
    }

    /**
     * \brief Do nothing.
     *
     * \return Nothing.
     */
    auto flush() noexcept -> Result<void> override final
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
 * \brief Automated testing output string stream.
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
    auto string() const noexcept -> std::string const &
    {
        return m_buffer.string();
    }

  private:
    /**
     * \brief The stream's device access buffer.
     */
    String_Stream_Buffer m_buffer{};
};

/**
 * \brief Automated testing vector stream device access buffer.
 *
 * \tparam T The vector element type.
 */
template<typename T>
class Vector_Stream_Buffer final : public Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    Vector_Stream_Buffer() = default;

    Vector_Stream_Buffer( Vector_Stream_Buffer && ) = delete;

    Vector_Stream_Buffer( Vector_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Vector_Stream_Buffer() noexcept = default;

    auto operator=( Vector_Stream_Buffer && ) = delete;

    auto operator=( Vector_Stream_Buffer const & ) = delete;

    /**
     * \brief Get the vector abstracted by the device access buffer.
     *
     * \return The vector abstracted by the device access buffer.
     */
    auto vector() const noexcept -> std::vector<T> const &
    {
        return m_vector;
    }

    /**
     * \brief Write a character to the vector.
     *
     * \param[in] character The character to write to the vector.
     *
     * \return Nothing.
     */
    auto put( char character ) noexcept -> Result<void> override final
    {
        m_vector.push_back( character );

        return {};
    }

    /**
     * \brief Write an unsigned byte to the vector.
     *
     * \param[in] value The unsigned byte to write to the vector.
     *
     * \return Nothing.
     */
    auto put( std::uint8_t value ) noexcept -> Result<void> override final
    {
        m_vector.push_back( value );

        return {};
    }

    /**
     * \brief Write a signed byte to the vector.
     *
     * \param[in] value The signed byte to write to the vector.
     *
     * \return Nothing.
     */
    auto put( std::int8_t value ) noexcept -> Result<void> override final
    {
        m_vector.push_back( value );

        return {};
    }

    /**
     * \brief Do nothing.
     *
     * \return Nothing.
     */
    auto flush() noexcept -> Result<void> override final
    {
        return {};
    }

  private:
    /**
     * \brief The vector abstracted by the device access buffer.
     */
    std::vector<T> m_vector{};
};

/**
 * \brief Automated testing output vector stream.
 *
 * \tparam T The vector element type.
 */
template<typename T>
class Output_Vector_Stream : public Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Output_Vector_Stream()
    {
        set_buffer( &m_buffer );
    }

    Output_Vector_Stream( Output_Vector_Stream && ) = delete;

    Output_Vector_Stream( Output_Vector_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Vector_Stream() noexcept = default;

    auto operator=( Output_Vector_Stream && ) = delete;

    auto operator=( Output_Vector_Stream const & ) = delete;

    /**
     * \brief Get the vector abstracted by the stream.
     *
     * \return The vector abstracted by the stream.
     */
    auto vector() const noexcept -> std::vector<T> const &
    {
        return m_buffer.vector();
    }

  private:
    /**
     * \brief The stream's device access buffer.
     */
    Vector_Stream_Buffer<T> m_buffer{};
};

/**
 * \brief Mock reliable I/O stream device access buffer.
 */
class Mock_Reliable_Stream_Buffer : public Reliable_Stream_Buffer {
  public:
    Mock_Reliable_Stream_Buffer() = default;

    Mock_Reliable_Stream_Buffer( Mock_Reliable_Stream_Buffer && ) = delete;

    Mock_Reliable_Stream_Buffer( Mock_Reliable_Stream_Buffer const & ) = delete;

    ~Mock_Reliable_Stream_Buffer() noexcept = default;

    auto operator=( Mock_Reliable_Stream_Buffer && ) = delete;

    auto operator=( Mock_Reliable_Stream_Buffer const & ) = delete;

    MOCK_METHOD( void, put, (char), ( noexcept, override ) );
    MOCK_METHOD( void, put, ( std::string ) );

    void put( char const * begin, char const * end ) noexcept override
    {
        put( std::string{ begin, end } );
    }

    void put( char const * string ) noexcept override
    {
        put( std::string{ string } );
    }

    MOCK_METHOD( void, put, ( std::uint8_t ), ( noexcept, override ) );
    MOCK_METHOD( void, put, (std::vector<std::uint8_t>));

    void put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept override
    {
        put( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( void, put, ( std::int8_t ), ( noexcept, override ) );
    MOCK_METHOD( void, put, (std::vector<std::int8_t>));

    void put( std::int8_t const * begin, std::int8_t const * end ) noexcept override
    {
        put( std::vector<std::int8_t>{ begin, end } );
    }

    MOCK_METHOD( void, flush, (), ( noexcept, override ) );
};

/**
 * \brief Mock reliable output stream.
 */
class Mock_Reliable_Output_Stream : public Reliable_Output_Stream {
  public:
    Mock_Reliable_Output_Stream()
    {
        set_buffer( &m_buffer );
    }

    Mock_Reliable_Output_Stream( Mock_Reliable_Output_Stream && ) = delete;

    Mock_Reliable_Output_Stream( Mock_Reliable_Output_Stream const & ) = delete;

    ~Mock_Reliable_Output_Stream() noexcept = default;

    auto operator=( Mock_Reliable_Output_Stream && ) = delete;

    auto operator=( Mock_Reliable_Output_Stream const & ) = delete;

    using ::picolibrary::Reliable_Stream::clear_end_of_file_reached_report;
    using ::picolibrary::Reliable_Stream::report_end_of_file_reached;

    auto buffer() noexcept -> Mock_Reliable_Stream_Buffer &
    {
        return m_buffer;
    }

  private:
    Mock_Reliable_Stream_Buffer m_buffer{};
};

/**
 * \brief Automated testing reliable string stream device access buffer.
 */
class Reliable_String_Stream_Buffer final : public Reliable_Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    Reliable_String_Stream_Buffer() = default;

    Reliable_String_Stream_Buffer( Reliable_String_Stream_Buffer && ) = delete;

    Reliable_String_Stream_Buffer( Reliable_String_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_String_Stream_Buffer() noexcept = default;

    auto operator=( Reliable_String_Stream_Buffer && ) = delete;

    auto operator=( Reliable_String_Stream_Buffer const & ) = delete;

    /**
     * \brief Get the string abstracted by the device access buffer.
     *
     * \return The string abstracted by the device access buffer.
     */
    auto string() const noexcept -> std::string const &
    {
        return m_string;
    }

    /**
     * \brief Write a character to the string.
     *
     * \param[in] character The character to write to the string.
     */
    void put( char character ) noexcept override final
    {
        m_string.push_back( character );
    }

    /**
     * \brief Write an unsigned byte to the string.
     *
     * \param[in] value The unsigned byte to write to the string.
     */
    void put( std::uint8_t value ) noexcept override final
    {
        m_string.push_back( value );
    }

    /**
     * \brief Write a signed byte to the string.
     *
     * \param[in] value The signed byte to write to the string.
     */
    void put( std::int8_t value ) noexcept override final
    {
        m_string.push_back( value );
    }

    /**
     * \brief Do nothing.
     */
    void flush() noexcept override final
    {
    }

  private:
    /**
     * \brief The string abstracted by the device access buffer.
     */
    std::string m_string{};
};

/**
 * \brief Automated testing reliable output string stream.
 */
class Reliable_Output_String_Stream : public Reliable_Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Reliable_Output_String_Stream()
    {
        set_buffer( &m_buffer );
    }

    Reliable_Output_String_Stream( Reliable_Output_String_Stream && ) = delete;

    Reliable_Output_String_Stream( Reliable_Output_String_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Output_String_Stream() noexcept = default;

    auto operator=( Reliable_Output_String_Stream && ) = delete;

    auto operator=( Reliable_Output_String_Stream const & ) = delete;

    /**
     * \brief Get the string abstracted by the stream.
     *
     * \return The string abstracted by the stream.
     */
    auto string() const noexcept -> std::string const &
    {
        return m_buffer.string();
    }

  private:
    /**
     * \brief The stream's device access buffer.
     */
    Reliable_String_Stream_Buffer m_buffer{};
};

/**
 * \brief Automated testing reliable vector stream device access buffer.
 *
 * \tparam T The vector element type.
 */
template<typename T>
class Reliable_Vector_Stream_Buffer final : public Reliable_Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    Reliable_Vector_Stream_Buffer() = default;

    Reliable_Vector_Stream_Buffer( Reliable_Vector_Stream_Buffer && ) = delete;

    Reliable_Vector_Stream_Buffer( Reliable_Vector_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Vector_Stream_Buffer() noexcept = default;

    auto operator=( Reliable_Vector_Stream_Buffer && ) = delete;

    auto operator=( Reliable_Vector_Stream_Buffer const & ) = delete;

    /**
     * \brief Get the vector abstracted by the device access buffer.
     *
     * \return The vector abstracted by the device access buffer.
     */
    auto vector() const noexcept -> std::vector<T> const &
    {
        return m_vector;
    }

    /**
     * \brief Write a character to the vector.
     *
     * \param[in] character The character to write to the vector.
     */
    void put( char character ) noexcept override final
    {
        m_vector.push_back( character );
    }

    /**
     * \brief Write an unsigned byte to the vector.
     *
     * \param[in] value The unsigned byte to write to the vector.
     */
    void put( std::uint8_t value ) noexcept override final
    {
        m_vector.push_back( value );
    }

    /**
     * \brief Write a signed byte to the vector.
     *
     * \param[in] value The signed byte to write to the vector.
     */
    void put( std::int8_t value ) noexcept override final
    {
        m_vector.push_back( value );
    }

    /**
     * \brief Do nothing.
     */
    void flush() noexcept override final
    {
    }

  private:
    /**
     * \brief The vector abstracted by the device access buffer.
     */
    std::vector<T> m_vector{};
};

/**
 * \brief Automated testing reliable output vector stream.
 *
 * \tparam T The vector element type.
 */
template<typename T>
class Reliable_Output_Vector_Stream : public Reliable_Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Reliable_Output_Vector_Stream()
    {
        set_buffer( &m_buffer );
    }

    Reliable_Output_Vector_Stream( Reliable_Output_Vector_Stream && ) = delete;

    Reliable_Output_Vector_Stream( Reliable_Output_Vector_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Output_Vector_Stream() noexcept = default;

    auto operator=( Reliable_Output_Vector_Stream && ) = delete;

    auto operator=( Reliable_Output_Vector_Stream const & ) = delete;

    /**
     * \brief Get the vector abstracted by the stream.
     *
     * \return The vector abstracted by the stream.
     */
    auto vector() const noexcept -> std::vector<T> const &
    {
        return m_buffer.vector();
    }

  private:
    /**
     * \brief The stream's device access buffer.
     */
    Reliable_Vector_Stream_Buffer<T> m_buffer{};
};

} // namespace picolibrary::Testing::Automated

#endif // PICOLIBRARY_TESTING_AUTOMATED_STREAM_H
