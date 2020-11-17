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

#include <cstdint>
#include <type_traits>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/utility.h"

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
        return for_each<Discard_Functor>(
            begin, end, [this]( auto character ) noexcept { return put( character ); } );
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
            if ( result.is_error() ) { return result; } // if
        }                                               // while

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
        return for_each<Discard_Functor>(
            begin, end, [this]( auto value ) noexcept { return put( value ); } );
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
        return for_each<Discard_Functor>(
            begin, end, [this]( auto value ) noexcept { return put( value ); } );
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
 * \brief I/O stream core (base class)
 *
 * This class handles the following for an I/O stream:
 * - Stores the I/O stream's state information (end-of-file reached, I/O error present,
 *   fatal error present)
 * - Associates the I/O stream with an I/O stream device access buffer
 */
class Stream {
  public:
    /**
     * \brief Destructor.
     */
    ~Stream() noexcept = default;

    /**
     * \brief Check if the stream is nominal (no errors present and end-of-file has not
     *        been reached.
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
     * \brief Check if no errors are present (I/O error and fatal error not present).
     *
     * \return true if no errors are present.
     * \return false if errors are present.
     */
    constexpr explicit operator bool() const noexcept
    {
        return not error_present();
    }

    /**
     * \copydoc picolibrary::Stream::error_present()
     */
    constexpr auto operator!() const noexcept
    {
        return error_present();
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
     * \return The I/O stream device access buffer associated with the I/O stream.
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
        static constexpr auto END_OF_FILE_REACHED = State{ 0b1 << Bit::END_OF_FILE_REACHED }; ///< End-of-file reached.
        static constexpr auto IO_ERROR_PRESENT = State{ 0b1 << Bit::IO_ERROR_PRESENT }; ///< I/O error present.
        static constexpr auto FATAL_ERROR_PRESENT = State{ 0b1 << Bit::FATAL_ERROR_PRESENT }; ///< Fatal error present.

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
 * \tparam T The type to be printed.
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

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter && ) = delete;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the value to be formatted.
     *
     * \param[in] format The format specification for the value to be formatted. This will
     *            not include the opening '{'.
     *
     * \return A pointer to the end of the format specification ('}') if the format
     *         specification is valid.
     * \return picolibrary::Generic_Error::INVALID_FORMAT if the format specification
     *         is not valid.
     */
    auto parse( char const * format ) noexcept -> Result<char const *, Error_Code>;

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
     * \brief Destructor.
     */
    ~Output_Stream() noexcept = default;

    /**
     * \brief Write a character to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] character The character to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char character ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( character );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write a block of characters to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] begin The beginning of the block of characters to write to the stream.
     * \param[in] end The end of the block of characters to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char const * begin, char const * end ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write a null-terminated string to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] string The null-terminated string to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( char const * string ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( string );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write an unsigned byte to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] value The unsigned byte to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( value );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write a block of unsigned bytes to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
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
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write a signed byte to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] value The signed byte to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( value );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write a block of signed bytes to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \param[in] begin The beginning of the block of signed bytes to write to the stream.
     * \param[in] end The end of the block of signed bytes to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        auto result = buffer()->put( begin, end );
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \pre Neither an I/O error nor a fatal error is present. If either an I/O error or a
     *      fatal error is present, picolibrary::Generic_Error::IO_STREAM_DEGRADED will be
     *      returned.
     *
     * \tparam Types The types to write.
     *
     * \param[in] format The format string specifying the format to use for each value to
     *            be written. Format string syntax is based on the Python format string
     *            syntax. Named and positional arguments are not supported. The format
     *            specification for each value to be written is delimited by '{' and '}'.
     *            Use "{{" to write a literal '{'. Use "}}" to write a literal '}'. The
     *            format specification syntax for a particular type is defined by the
     *            specialization of picolibrary::Output_Formatter that is applicable to
     *            the type.
     * \param[in] values The values to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::INVALID_FORMAT if the format string is not
     *         valid.
     * \return An error code if the write failed.
     */
    template<typename... Types>
    auto print( char const * format, Types &&... values ) noexcept -> Result<Void, Error_Code>
    {
        if ( error_present() ) { return Generic_Error::IO_STREAM_DEGRADED; } // if

        return print_implementation( format, std::forward<Types>( values )... );
    }

    /**
     * \brief Write any output that has been buffered to the device associated with the
     *        stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto flush() noexcept
    {
        auto result = buffer()->flush();
        if ( result.is_error() ) { report_fatal_error(); } // if
        return result;
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
     *            be written.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::INVALID_FORMAT if the format string is not
     *         valid.
     * \return An error code if the write failed.
     */
    auto print_implementation( char const * format ) noexcept -> Result<Void, Error_Code>
    {
        for ( ; *format != '\0'; ++format ) {
            if ( *format == '{' or *format == '}' ) {
                if ( *( format + 1 ) != *format ) {
                    report_io_error();
                    return Generic_Error::INVALID_FORMAT;
                } // if

                ++format;
            } // if

            auto result = put( *format );
            if ( result.is_error() ) { return result; } // if
        }                                               // for

        return {};
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to write.
     * \tparam Types The types to write.
     *
     * \param[in] format The format string specifying the format to use for each value to
     *            be written.
     * \param[in] value The value to write.
     * \param[in] values The values to write.
     *
     * \return Nothing if the write succeeded.
     * \return picolibrary::Generic_Error::INVALID_FORMAT if the format string is not
     *         valid.
     * \return An error code if the write failed.
     */
    template<typename Type, typename... Types>
    auto print_implementation( char const * format, Type && value, Types &&... values ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        for ( ; *format != '\0'; ++format ) {
            if ( *format == '{' ) {
                ++format;

                if ( *format != '{' ) {
                    auto formatter = Output_Formatter<std::decay_t<Type>>{};

                    {
                        auto result = formatter.parse( format );
                        if ( result.is_error() ) {
                            report_io_error();
                            return result.error();
                        } // if

                        format = result.value();

                        if ( *format != '}' ) {
                            report_io_error();
                            return Generic_Error::INVALID_FORMAT;
                        } // if

                        ++format;
                    }

                    {
                        auto result = formatter.print( *this, value );
                        if ( result.is_error() ) { return result; } // if
                    }

                    return print_implementation( format, std::forward<Types>( values )... );
                } // if
            } else if ( *format == '}' ) {
                ++format;

                if ( *format != '}' ) {
                    report_io_error();
                    return Generic_Error::INVALID_FORMAT;
                } // if
            }     // else if

            auto result = put( *format );
            if ( result.is_error() ) { return result; } // if
        }                                               // for

        report_io_error();
        return Generic_Error::INVALID_FORMAT;
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

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter && ) = delete;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the string to be formatted.
     *
     * \param[in] format The format specification for the string to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the string to the stream.
     *
     * \param[in] stream The stream to write the string to.
     * \param[in] string The string to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, char const * string ) noexcept
    {
        return stream.put( string );
    }
};

/**
 * \brief picolibrary::Error_Code output formatter.
 *
 * picolibrary::Error_Code only supports the default format specification ("{}").
 */
template<>
class Output_Formatter<Error_Code> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter && ) = delete;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the picolibrary::Error_Code to be
     *        formatted.
     *
     * \param[in] format The format specification for the picolibrary::Error_Code to be
     *            formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the picolibrary::Error_Code to the stream.
     *
     * \param[in] stream The stream to write the picolibrary::Error_Code to.
     * \param[in] error The picolibrary::Error_Code to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Error_Code error ) noexcept
    {
        return stream.print( "{}::{}", error.category().name(), error.description() );
    }
};

/**
 * \brief Error code enum output formatter.
 *
 * \tparam Enum The error code enum.
 */
template<typename Enum>
class Output_Formatter<Enum, std::enable_if_t<is_error_code_enum_v<Enum>>>
    : public Output_Formatter<Error_Code> {
};

} // namespace picolibrary

#endif // PICOLIBRARY_STREAM_H
