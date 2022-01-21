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
 * \brief picolibrary stream interface.
 */

#ifndef PICOLIBRARY_STREAM_H
#define PICOLIBRARY_STREAM_H

#include <cstdint>
#include <type_traits>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief I/O stream device access buffer.
 */
class Stream_Buffer {
  public:
    /**
     * \brief Write a character to the put area of the buffer.
     *
     * \param[in] character The character to write to the put area of the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( char character ) noexcept -> Result<Void, Error_Code> = 0;

    /**
     * \brief Write a block of characters to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of characters to write to the put area
     *            of the buffer.
     * \param[in] end The end of the block of characters to write to the put area of the
     *            buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( char const * begin, char const * end ) noexcept -> Result<Void, Error_Code>
    {
        return ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            begin, end, [ this ]( auto character ) noexcept { return put( character ); } );
    }

    /**
     * \brief Write a null-terminated string to the put area of the buffer.
     *
     * \param[in] string The null-terminated string to write to the put area of the
     *            buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( char const * string ) noexcept -> Result<Void, Error_Code>
    {
        while ( auto const character = *string++ ) {
            auto result = put( character );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // while

        return {};
    }

    /**
     * \brief Write an unsigned byte to the put area of the buffer.
     *
     * \param[in] value The unsigned byte to write to the put area of the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code> = 0;

    /**
     * \brief Write a block of unsigned bytes to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of unsigned bytes to write to the put
     *            area of the buffer.
     * \param[in] end The end of the block of unsigned bytes to write to the put area of
     *            the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code>
    {
        return ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            begin, end, [ this ]( auto value ) noexcept { return put( value ); } );
    }

    /**
     * \brief Write a signed byte to the put area of the buffer.
     *
     * \param[in] value The signed byte to write to the put area of the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code> = 0;

    /**
     * \brief Write a block of signed bytes to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of signed bytes to write to the put
     *            area of the buffer.
     * \param[in] end The end of the block of signed bytes to write to the put area of the
     *            buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept
        -> Result<Void, Error_Code>
    {
        return ::picolibrary::for_each<Functor_Can_Fail_Discard_Functor>(
            begin, end, [ this ]( auto value ) noexcept { return put( value ); } );
    }

    /**
     * \brief Write any data that is buffered in the put area of the buffer to the device.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto flush() noexcept -> Result<Void, Error_Code> = 0;

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
     * \brief Destructor.
     */
    ~Stream_Buffer() noexcept = default;

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

/**
 * \brief I/O stream core (base class).
 *
 * This class performs the following I/O stream functions:
 * - Stores the I/O stream's state information (end-of-file reached, I/O error preset,
 *   fatal error present)
 * - Associates the I/O stream with an I/O stream device access buffer
 */
class Stream {
  public:
    /**
     * \brief Check if the stream is nominal (no errors present and end-of-file has not
     *        been reached).
     *
     * \return true if the stream is nominal.
     * \return false if the stream is not nominal.
     */
    constexpr auto is_nominal() const noexcept
    {
        return not m_state;
    }

    /**
     * \brief Check if errors are present (I/O error and/or fatal error present).
     *
     * \return true if errors are present.
     * \return false if no errors are present.
     */
    constexpr auto error_present() const noexcept -> bool
    {
        return m_state & Mask::ERROR_PRESENT;
    }

    /**
     * \brief Check if end-of-file has been reached.
     *
     * \return true if end-of-file has been reached.
     * \return false if end-of-file has not been reached.
     */
    constexpr auto end_of_file_reached() const noexcept -> bool
    {
        return m_state & Mask::END_OF_FILE_REACHED;
    }

    /**
     * \brief Check if an I/O error is present.
     *
     * \return true if an I/O error is present.
     * \return false if an I/O error is not present.
     */
    constexpr auto io_error_present() const noexcept -> bool
    {
        return m_state & Mask::IO_ERROR_PRESENT;
    }

    /**
     * \brief Report an I/O error.
     */
    constexpr void report_io_error() noexcept
    {
        m_state |= Mask::IO_ERROR_PRESENT;
    }

    /**
     * \brief Clear an I/O error.
     */
    constexpr void clear_io_error() noexcept
    {
        m_state &= ~Mask::IO_ERROR_PRESENT;
    }

    /**
     * \brief Check if a fatal error is present.
     *
     * \return true if a fatal error is present.
     * \return false if a fatal error is not present.
     */
    constexpr auto fatal_error_present() const noexcept -> bool
    {
        return m_state & Mask::FATAL_ERROR_PRESENT;
    }

    /**
     * \brief Check if the I/O stream is associated with an I/O stream device access
     *        buffer.
     *
     * \return true if the I/O stream is associated with an I/O stream device access
     *         buffer.
     * \return false if the I/O stream is not associated with an I/O stream device access
     *         buffer.
     */
    constexpr auto buffer_is_set() const noexcept -> bool
    {
        return m_buffer;
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Stream( Stream && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Stream( Stream const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Stream && expression ) noexcept -> Stream & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Stream const & expression ) noexcept -> Stream & = default;

    /**
     * \brief Report that end-of-file has been reached.
     */
    constexpr void report_end_of_file_reached() noexcept
    {
        m_state |= Mask::END_OF_FILE_REACHED;
    }

    /**
     * \brief Clear end-of-file reached report.
     */
    constexpr void clear_end_of_file_reached_report() noexcept
    {
        m_state &= ~Mask::END_OF_FILE_REACHED;
    }

    /**
     * \brief Report a fatal error.
     */
    constexpr void report_fatal_error() noexcept
    {
        m_state |= Mask::FATAL_ERROR_PRESENT;
    }

    /**
     * \brief Clear a fatal error.
     */
    constexpr void clear_fatal_error() noexcept
    {
        m_state &= ~Mask::FATAL_ERROR_PRESENT;
    }

    /**
     * \brief Get the I/O stream device access buffer associated with the I/O stream.
     *
     * \brief The I/O stream device access buffer associated with the I/O stream.
     */
    constexpr auto buffer() noexcept
    {
        return m_buffer;
    }

    /**
     * \brief Associate the I/O stream with an I/O stream device access buffer.
     *
     * \param[in] buffer The I/O stream device access buffer to associate the I/O stream
     *            with.
     */
    constexpr void set_buffer( Stream_Buffer * buffer ) noexcept
    {
        m_buffer = buffer;
    }

  private:
    /**
     * \brief State flags.
     */
    using State = std::uint_fast8_t;

    /**
     * \brief State flag bit positions.
     */
    enum Bit : std::uint_fast8_t {
        END_OF_FILE_REACHED, ///< End-of-file reached.
        IO_ERROR_PRESENT,    ///< I/O error present.
        FATAL_ERROR_PRESENT, ///< Fatal error present.
    };

    /**
     * \brief State flag bit masks.
     */
    struct Mask {
        static constexpr auto END_OF_FILE_REACHED = mask<State>( 1, Bit::END_OF_FILE_REACHED ); ///< End-of-file reached.
        static constexpr auto IO_ERROR_PRESENT = mask<State>( 1, Bit::IO_ERROR_PRESENT ); ///< I/O error present.
        static constexpr auto FATAL_ERROR_PRESENT = mask<State>( 1, Bit::FATAL_ERROR_PRESENT ); ///< Fatal error present.

        static constexpr auto ERROR_PRESENT = State{ IO_ERROR_PRESENT | FATAL_ERROR_PRESENT }; ///< Error present.
    };

    /**
     * \brief The I/O stream's state flags.
     */
    State m_state{};

    /**
     * \brief The I/O stream device access buffer associated with the I/O stream.
     */
    Stream_Buffer * m_buffer{};
};

class Output_Stream;

/**
 * \brief Output formatter.
 *
 * \tparam T The type to print.
 *
 * \attention This class must be fully or partially specialized for each type that will
 *            support formatted output.
 */
template<typename T, typename = void>
class Output_Formatter {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the value to be formatted.
     *
     * \param[in] format The format specification for the value to be formatted.
     *
     * \pre format is a valid format specification for the value to be formatted.
     *
     * \return A pointer to the end of the format specification ('}').
     */
    auto parse( char const * format ) noexcept -> char const *;

    /**
     * \brief Write the formatted value to the stream.
     *
     * \param[in] stream The stream to write the formatted value to.
     * \param[in] value The value to format.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, T const & value ) noexcept -> Result<Void, Error_Code>;
};

/**
 * \brief Output stream.
 */
class Output_Stream : public Stream {
  public:
    /**
     * \brief Write a character to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] character The character to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char character ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( character );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write a block of characters to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of characters to write to the stream.
     * \param[in] end The end of the block of characters to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char const * begin, char const * end ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write a null-terminated string to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] string The null-terminated string to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char const * string ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( string );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write an unsigned byte to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] value The unsigned byte to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( value );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write a block of unsigned bytes to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of unsigned bytes to write to the
     *            stream.
     * \param[in] end The end of the block of unsigned bytes to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write a signed byte to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] value The signed byte to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( value );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write a block of signed bytes to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of signed bytes to write to the stream.
     * \param[in] end The end of the block of signed bytes to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \tparam Types The types to print.
     *
     * \param[in] format The format string specifying the format to use for each value to
     *            be formatted. Format string syntax is based on the Python format string
     *            syntax. Named and positional arguments are not supported. The format
     *            specification for each value to be formatted is delimited by '{' and
     *            '}'. Use "{{" to write a literal '{'. Use "}}" to write a literal '}'.
     *            The format specification syntax for a particular type is defined by the
     *            specialization of picolibrary::Output_Formatter that is applicable to
     *            that type.
     * \param[in] values The values to format.
     *
     * \pre All format specifications found in format are valid.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    template<typename... Types>
    auto print( char const * format, Types &&... values ) noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        return print_implementation( format, std::forward<Types>( values )... );
    }

    /**
     * \brief Write any output that has been buffered to the device associated with the
     *        stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto flush() noexcept -> Result<Void, Error_Code>
    {
        expect( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->flush();
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Output_Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Stream( Output_Stream && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Stream( Output_Stream const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Stream && expression ) noexcept -> Output_Stream & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Stream const & expression ) noexcept -> Output_Stream & = default;

  private:
    /**
     * \brief Write formatted output to the stream.
     *
     * \param[in] format The format string specifying the format to use for each value to
     *            be formatted.
     *
     * \pre All format specifications found in format are valid.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print_implementation( char const * format ) noexcept -> Result<Void, Error_Code>
    {
        for ( ; *format; ++format ) {
            if ( *format == '{' or *format == '}' ) {
                expect( *( format + 1 ) == *format, Generic_Error::INVALID_FORMAT );

                ++format;
            } // if

            auto result = put( *format );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for

        return {};
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to print.
     * \tparam Types The types to print.
     *
     * \param[in] format The format string specifying the format to use for each value to
     *            be formatted.
     * \param[in] value The value to format.
     * \param[in] values The values to format.
     *
     * \pre All format specifications found in format are valid.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    template<typename Type, typename... Types>
    auto print_implementation( char const * format, Type && value, Types &&... values ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        for ( ; *format; ++format ) {
            if ( *format == '{' ) {
                ++format;

                if ( *format != '{' ) {
                    auto formatter = Output_Formatter<std::decay_t<Type>>{};

                    format = formatter.parse( format );
                    expect( *format == '}', Generic_Error::INVALID_FORMAT );

                    ++format;

                    auto result = formatter.print( *this, value );
                    if ( result.is_error() ) {
                        report_fatal_error();

                        return result.error();
                    } // if

                    return print_implementation( format, std::forward<Types>( values )... );
                } // if
            } else if ( *format == '}' ) {
                ++format;

                expect( *format == '}', Generic_Error::INVALID_FORMAT );
            } // else if

            auto result = put( *format );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // for

        expect( false, Generic_Error::INVALID_FORMAT );

        return {}; // unreachable
    }
};

/**
 * \brief Character output formatter.
 *
 * Characters only support the default format specification ("{}").
 */
template<>
class Output_Formatter<char> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the character to be formatted.
     *
     * \param[in] format The format specification for the character to be formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted character to the stream.
     *
     * \param[in] stream The stream to write the formatted character to.
     * \param[in] character The character to format.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, char character ) noexcept
    {
        return stream.put( character );
    }
};

/**
 * \brief Null-terminated string output formatter.
 *
 * Null-terminated strings only support the default format specification ("{}").
 */
template<>
class Output_Formatter<char const *> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the null-terminated string to be
     *        formatted.
     *
     * \param[in] format The format specification for the null-terminated string to be
     *            formatted.
     *
     * \return format
     */
    constexpr auto parse( char const * format ) noexcept
    {
        return format;
    }

    /**
     * \brief Write the formatted null-terminated string to the stream.
     *
     * \param[in] stream The stream to write the formatted null-terminated string to.
     * \param[in] string The null-terminated string to format.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, char const * string ) noexcept
    {
        return stream.put( string );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_STREAM_H
