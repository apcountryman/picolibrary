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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/bit_manipulation.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"
#include "picolibrary/result.h"
#include "picolibrary/rom.h"

namespace picolibrary {

class Output_Stream;

class Reliable_Output_Stream;

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
    Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Formatter const & expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Write the formatted value to the stream.
     *
     * \param[in] stream The stream to write the formatted value to.
     * \param[in] value The value to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, T const & value ) const noexcept
        -> Result<std::size_t, Error_Code>;

    /**
     * \brief Write the formatted value to the stream.
     *
     * \param[in] stream The stream to write the formatted value to.
     * \param[in] value The value to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, T const & value ) const noexcept -> std::size_t;
};

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
    virtual auto put( char character ) noexcept -> Result<void, Error_Code> = 0;

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
    virtual auto put( char const * begin, char const * end ) noexcept -> Result<void, Error_Code>
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
    virtual auto put( char const * string ) noexcept -> Result<void, Error_Code>
    {
        while ( auto const character = *string++ ) {
            auto result = put( character );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // while

        return {};
    }

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
    /**
     * \brief Write a null-terminated ROM string to the put area of the buffer.
     *
     * \param[in] string The null-terminated ROM string to write to the put area of the
     *            buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( ROM::String string ) noexcept -> Result<void, Error_Code>
    {
        while ( auto const character = *string++ ) {
            auto result = put( character );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }     // while

        return {};
    }
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

    /**
     * \brief Write an unsigned byte to the put area of the buffer.
     *
     * \param[in] value The unsigned byte to write to the put area of the buffer.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<void, Error_Code> = 0;

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
        -> Result<void, Error_Code>
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
    virtual auto put( std::int8_t value ) noexcept -> Result<void, Error_Code> = 0;

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
        -> Result<void, Error_Code>
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
    virtual auto flush() noexcept -> Result<void, Error_Code> = 0;

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
    constexpr auto is_nominal() const noexcept -> bool
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
    constexpr auto buffer() noexcept -> Stream_Buffer *
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
    auto put( char character ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
    auto put( char const * begin, char const * end ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
    auto put( char const * string ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( string );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
    /**
     * \brief Write a null-terminated ROM string to the stream.
     *
     * \pre picolibrary::Stream::is_nominal()
     *
     * \param[in] string The null-terminated ROM string to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto put( ROM::String string ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        auto result = buffer()->put( string );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return {};
    }
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

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
    auto put( std::uint8_t value ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
    auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
    auto put( std::int8_t value ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
    auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
     * \param[in] values The values to format. If a value is followed by an output
     *            formatter, the output formatter will be used to format the value and
     *            write the formatted value to the stream. If a value is not followed by
     *            an output formatter, a default constructed output formatter will be used
     *            to format the value and write the formatted value to the stream.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    template<typename... Types>
    auto print( Types &&... values ) noexcept -> Result<std::size_t, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        return print_implementation( std::size_t{ 0 }, std::forward<Types>( values )... );
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
    auto flush() noexcept -> Result<void, Error_Code>
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

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
     * \param[in] n The number of characters that have been written to the stream.
     *
     * \return The number of characters written to the stream.
     */
    auto print_implementation( std::size_t n ) noexcept -> Result<std::size_t, Error_Code>
    {
        return n;
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to print.
     * \tparam Types The types to print.
     *
     * \param[in] n The number of characters that have been written to the stream.
     * \param[in] value The value to format.
     * \param[in] formatter The output formatter to use to format the value and write the
     *            formatted value to the stream.
     * \param[in] values The values to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    template<typename Type, typename... Types>
    auto print_implementation( std::size_t n, Type && value, Output_Formatter<std::decay_t<Type>> formatter, Types &&... values ) noexcept
        -> Result<std::size_t, Error_Code>
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        auto result = formatter.print( *this, value );
        if ( result.is_error() ) {
            report_fatal_error();

            return result.error();
        } // if

        return print_implementation( n + result.value(), std::forward<Types>( values )... );
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to print.
     * \tparam Types The types to print.
     *
     * \param[in] n The number of characters that have been written to the stream.
     * \param[in] value The value to format.
     * \param[in] values The values to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    template<typename Type, typename... Types>
    auto print_implementation( std::size_t n, Type && value, Types &&... values ) noexcept
        -> Result<std::size_t, Error_Code>
    {
        return print_implementation(
            n,
            std::forward<Type>( value ),
            Output_Formatter<std::decay_t<Type>>{},
            std::forward<Types>( values )... );
    }
};

/**
 * \brief Reliable I/O stream device access buffer.
 */
class Reliable_Stream_Buffer {
  public:
    /**
     * \brief Write a character to the put area of the buffer.
     *
     * \param[in] character The character to write to the put area of the buffer.
     */
    virtual void put( char character ) noexcept = 0;

    /**
     * \brief Write a block of characters to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of characters to write to the put area
     *            of the buffer.
     * \param[in] end The end of the block of characters to write to the put area of the
     *            buffer.
     */
    virtual void put( char const * begin, char const * end ) noexcept
    {
        ::picolibrary::for_each(
            begin, end, [ this ]( auto character ) noexcept { put( character ); } );
    }

    /**
     * \brief Write a null-terminated string to the put area of the buffer.
     *
     * \param[in] string The null-terminated string to write to the put area of the
     *            buffer.
     */
    virtual void put( char const * string ) noexcept
    {
        while ( auto const character = *string++ ) { put( character ); } // while
    }

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
    /**
     * \brief Write a null-terminated ROM string to the put area of the buffer.
     *
     * \param[in] string The null-terminated ROM string to write to the put area of the
     *            buffer.
     */
    virtual void put( ROM::String string ) noexcept
    {
        while ( auto const character = *string++ ) { put( character ); } // while
    }
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

    /**
     * \brief Write an unsigned byte to the put area of the buffer.
     *
     * \param[in] value The unsigned byte to write to the put area of the buffer.
     */
    virtual void put( std::uint8_t value ) noexcept = 0;

    /**
     * \brief Write a block of unsigned bytes to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of unsigned bytes to write to the put
     *            area of the buffer.
     * \param[in] end The end of the block of unsigned bytes to write to the put area of
     *            the buffer.
     */
    virtual void put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        ::picolibrary::for_each(
            begin, end, [ this ]( auto value ) noexcept { put( value ); } );
    }

    /**
     * \brief Write a signed byte to the put area of the buffer.
     *
     * \param[in] value The signed byte to write to the put area of the buffer.
     */
    virtual void put( std::int8_t value ) noexcept = 0;

    /**
     * \brief Write a block of signed bytes to the put area of the buffer.
     *
     * \param[in] begin The beginning of the block of signed bytes to write to the put
     *            area of the buffer.
     * \param[in] end The end of the block of signed bytes to write to the put area of the
     *            buffer.
     */
    virtual void put( std::int8_t const * begin, std::int8_t const * end ) noexcept
    {
        ::picolibrary::for_each(
            begin, end, [ this ]( auto value ) noexcept { put( value ); } );
    }

    /**
     * \brief Write any data that is buffered in the put area of the buffer to the device.
     */
    virtual void flush() noexcept = 0;

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Reliable_Stream_Buffer() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reliable_Stream_Buffer( Reliable_Stream_Buffer && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Reliable_Stream_Buffer( Reliable_Stream_Buffer const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Reliable_Stream_Buffer() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator         =( Reliable_Stream_Buffer && expression ) noexcept
        -> Reliable_Stream_Buffer & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reliable_Stream_Buffer const & expression ) noexcept
        -> Reliable_Stream_Buffer & = default;
};

/**
 * \brief Reliable I/O stream core (base class).
 *
 * This class performs the following I/O stream functions:
 * - Stores the I/O stream's state information (end-of-file reached, I/O error present)
 * - Associates the I/O stream with an I/O stream device access buffer
 */
class Reliable_Stream {
  public:
    /**
     * \brief Check if the stream is nominal (no errors present and end-of-file has not
     *        been reached).
     *
     * \return true if the stream is nominal.
     * \return false if the stream is not nominal.
     */
    constexpr auto is_nominal() const noexcept -> bool
    {
        return not m_state;
    }

    /**
     * \brief Check if errors are present (I/O error present).
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
    constexpr Reliable_Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reliable_Stream( Reliable_Stream && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Reliable_Stream( Reliable_Stream const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Reliable_Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reliable_Stream && expression ) noexcept -> Reliable_Stream & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator  =( Reliable_Stream const & expression ) noexcept
        -> Reliable_Stream & = default;

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
     * \brief Get the I/O stream device access buffer associated with the I/O stream.
     *
     * \brief The I/O stream device access buffer associated with the I/O stream.
     */
    constexpr auto buffer() noexcept -> Reliable_Stream_Buffer *
    {
        return m_buffer;
    }

    /**
     * \brief Associate the I/O stream with an I/O stream device access buffer.
     *
     * \param[in] buffer The I/O stream device access buffer to associate the I/O stream
     *            with.
     */
    constexpr void set_buffer( Reliable_Stream_Buffer * buffer ) noexcept
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
    };

    /**
     * \brief State flag bit masks.
     */
    struct Mask {
        static constexpr auto END_OF_FILE_REACHED = mask<State>( 1, Bit::END_OF_FILE_REACHED ); ///< End-of-file reached.
        static constexpr auto IO_ERROR_PRESENT = mask<State>( 1, Bit::IO_ERROR_PRESENT ); ///< I/O error present.

        static constexpr auto ERROR_PRESENT = IO_ERROR_PRESENT; ///< Error present.
    };

    /**
     * \brief The I/O stream's state flags.
     */
    State m_state{};

    /**
     * \brief The I/O stream device access buffer associated with the I/O stream.
     */
    Reliable_Stream_Buffer * m_buffer{};
};

/**
 * \brief Reliable output stream.
 */
class Reliable_Output_Stream : public Reliable_Stream {
  public:
    /**
     * \brief Write a character to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] character The character to write to the stream.
     */
    void put( char character ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( character );
    }

    /**
     * \brief Write a block of characters to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of characters to write to the stream.
     * \param[in] end The end of the block of characters to write to the stream.
     */
    void put( char const * begin, char const * end ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( begin, end );
    }

    /**
     * \brief Write a null-terminated string to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \oaram[in] string The null-terminated string to write to the stream.
     */
    void put( char const * string ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( string );
    }

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
    /**
     * \brief Write a null-terminated ROM string to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \oaram[in] string The null-terminated ROM string to write to the stream.
     */
    void put( ROM::String string ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( string );
    }
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

    /**
     * \brief Write an unsigned byte to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] value The unsigned byte to write to the stream.
     */
    void put( std::uint8_t value ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( value );
    }

    /**
     * \brief Write a block of unsigned bytes to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of unsigned bytes to write to the
     *            stream.
     * \param[in] end The end of the block of unsigned bytes to write to the stream.
     */
    void put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( begin, end );
    }

    /**
     * \brief Write a signed byte to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] value The signed byte to write to the stream.
     */
    void put( std::int8_t value ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( value );
    }

    /**
     * \brief Write a block of signed bytes to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \param[in] begin The beginning of the block of signed bytes to write to the stream.
     * \param[in] end The end of the block of signed bytes to write to the stream.
     */
    void put( std::int8_t const * begin, std::int8_t const * end ) noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->put( begin, end );
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     *
     * \tparam Types The types to print.
     *
     * \param[in] values The values to format. If a value is followed by an output
     *            formatter, the output formatter will be used to format the value and
     *            write the formatted value to the stream. If a value is not followed by
     *            an output formatter, a default constructed output formatter will be used
     *            to format the value and write the formatted value to the stream.
     *
     * \return The number of characters written to the stream.
     */
    template<typename... Types>
    auto print( Types &&... values ) noexcept -> std::size_t
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        return print_implementation( std::size_t{ 0 }, std::forward<Types>( values )... );
    }

    /**
     * \brief Write any output that has been buffered to the device associated with the
     *        stream.
     *
     * \pre picolibrary::Reliable_Stream::is_nominal()
     */
    void flush() noexcept
    {
        PICOLIBRARY_EXPECT( is_nominal(), Generic_Error::IO_STREAM_DEGRADED );

        buffer()->flush();
    }

  protected:
    /**
     * \brief Constructor.
     */
    constexpr Reliable_Output_Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reliable_Output_Stream( Reliable_Output_Stream && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Reliable_Output_Stream( Reliable_Output_Stream const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Reliable_Output_Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator         =( Reliable_Output_Stream && expression ) noexcept
        -> Reliable_Output_Stream & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reliable_Output_Stream const & expression ) noexcept
        -> Reliable_Output_Stream & = default;

  private:
    /**
     * \brief Write formatted output to the stream.
     *
     * \param[in] n The number of characters that have been written to the stream.
     *
     * \return The number of characters written to the stream.
     */
    auto print_implementation( std::size_t n ) noexcept -> std::size_t
    {
        return n;
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to print.
     * \tparam Types The types to print.
     *
     * \param[in] n The number of characters that have been written to the stream.
     * \param[in] value The value to format.
     * \param[in] formatter The output formatter to use to format the value and write the
     *            formatted value to the stream.
     * \param[in] values The values to format.
     *
     * \return The number of characters written to the stream.
     */
    template<typename Type, typename... Types>
    auto print_implementation( std::size_t n, Type && value, Output_Formatter<std::decay_t<Type>> formatter, Types &&... values ) noexcept
        -> std::size_t
    {
        return print_implementation(
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
            n + formatter.print( *this, value ),
            std::forward<Types>( values )... );
    }

    /**
     * \brief Write formatted output to the stream.
     *
     * \tparam Type The type to print.
     * \tparam Types The types to print.
     *
     * \param[in] n The number of characters that have been written to the stream.
     * \param[in] value The value to format.
     * \param[in] values The values to format.
     *
     * \return The number of characters written to the stream.
     */
    template<typename Type, typename... Types>
    auto print_implementation( std::size_t n, Type && value, Types &&... values ) noexcept
        -> std::size_t
    {
        return print_implementation(
            n,
            std::forward<Type>( value ),
            Output_Formatter<std::decay_t<Type>>{},
            std::forward<Types>( values )... );
    }
};

/**
 * \brief Character output formatter.
 */
template<>
class Output_Formatter<char> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted character to the stream.
     *
     * \param[in] stream The stream to write the formatted character to.
     * \param[in] character The character to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, char character ) const noexcept -> Result<std::size_t, Error_Code>
    {
        auto result = stream.put( character );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return std::size_t{ 1 };
    }

    /**
     * \brief Write the formatted character to the stream.
     *
     * \param[in] stream The stream to write the formatted character to.
     * \param[in] character The character to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, char character ) const noexcept -> std::size_t
    {
        stream.put( character );

        return std::size_t{ 1 };
    }
};

/**
 * \brief Null-terminated string output formatter.
 */
template<>
class Output_Formatter<char const *> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted null-terminated string to the stream.
     *
     * \param[in] stream The stream to write the formatted null-terminated string to.
     * \param[in] string The null-terminated string to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, char const * string ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        auto result = stream.put( string );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return std::strlen( string );
    }

    /**
     * \brief Write the formatted null-terminated string to the stream.
     *
     * \param[in] stream The stream to write the formatted null-terminated string to.
     * \param[in] string The null-terminated string to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, char const * string ) const noexcept -> std::size_t
    {
        stream.put( string );

        return std::strlen( string );
    }
};

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
/**
 * \brief Null-terminated ROM string output formatter.
 */
template<>
class Output_Formatter<ROM::String> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted null-terminated ROM string to the stream.
     *
     * \param[in] stream The stream to write the formatted null-terminated ROM string to.
     * \param[in] string The null-terminated ROM string to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, ROM::String string ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        auto result = stream.put( string );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return ROM::strlen( string );
    }

    /**
     * \brief Write the formatted null-terminated ROM string to the stream.
     *
     * \param[in] stream The stream to write the formatted null-terminated ROM string to.
     * \param[in] string The null-terminated ROM string to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, ROM::String string ) const noexcept -> std::size_t
    {
        stream.put( string );

        return ROM::strlen( string );
    }
};
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

/**
 * \brief picolibrary::Error_Code output formatter.
 */
template<>
class Output_Formatter<Error_Code> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Output_Formatter( Output_Formatter && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Output_Formatter( Output_Formatter const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Output_Formatter const & expression ) noexcept
        -> Output_Formatter & = default;

    /**
     * \brief Write the formatted picolibrary::Error_Code to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Error_Code to.
     * \param[in] error The picolibrary::Error_Code to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Error_Code const & error ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        return stream.print(
            error.category().name(), PICOLIBRARY_ROM_STRING( "::" ), error.description() );
    }

    /**
     * \brief Write the formatted picolibrary::Error_Code to the stream.
     *
     * \param[in] stream The stream to write the formatted picolibrary::Error_Code to.
     * \param[in] error The picolibrary::Error_Code to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Error_Code const & error ) const noexcept -> std::size_t
    {
        return stream.print(
            error.category().name(), PICOLIBRARY_ROM_STRING( "::" ), error.description() );
    }
};

/**
 * \brief Error code enum output formatter.
 *
 * \tparam Enum The error code enum type to print.
 */
template<typename Enum>
class Output_Formatter<Enum, std::enable_if_t<is_error_code_enum_v<Enum>>> :
    public Output_Formatter<Error_Code> {
};

} // namespace picolibrary

#endif // PICOLIBRARY_STREAM_H
