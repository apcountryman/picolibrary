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
     * \brief Write the contents of the put area of the buffer to the device.
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
     * \return true if an error is present.
     * \return false if an error is not present.
     */
    constexpr auto error_present() const noexcept
    {
        return false;
    }

    /**
     * \brief Check if errors are not present (I/O error and fatal error not present).
     *
     * \return true if an error is not present.
     * \return false if an error is present.
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
    constexpr auto end_of_file_reached() const noexcept
    {
        return m_state;
    }

    /**
     * \brief Check if an I/O error is present.
     *
     * \return true if an I/O error is present.
     * \return false if an I/O error is not present.
     */
    constexpr auto io_error_present() const noexcept
    {
        return false;
    }

    /**
     * \brief Check if a fatal error is present.
     *
     * \return true if a fatal error is present.
     * \return false if a fatal error is not present.
     */
    constexpr auto fatal_error_present() const noexcept
    {
        return false;
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
     * \brief Get the I/O stream device access buffer associated with the I/O stream.
     *
     * \return The I/O stream device access buffer associated with the I/O stream.
     */
    constexpr auto buffer() noexcept -> Stream_Buffer *
    {
        return nullptr;
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
    };

    /**
     * \brief State flag bit masks.
     */
    struct Mask {
        static constexpr auto END_OF_FILE_REACHED = State{ 0b1 << Bit::END_OF_FILE_REACHED }; ///< End-of-file reached.
    };

    /**
     * \brief The stream's state flags.
     */
    State m_state{};
};

} // namespace picolibrary

#endif // PICOLIBRARY_STREAM_H
