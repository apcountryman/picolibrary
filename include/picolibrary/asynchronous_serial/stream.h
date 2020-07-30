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
 * \brief picolibrary::Asynchronous_Serial::Stream interface.
 */

#ifndef PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H

#include <cstdint>
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

namespace picolibrary::Asynchronous_Serial {

/**
 * \brief Unbuffered asynchronous serial output stream device access buffer.
 *
 * \tparam Transmitter The type of asynchronous serial transmitter that is abstracted by
 *         the device access buffer.
 */
template<typename Transmitter>
class Unbuffered_Output_Stream_Buffer : public Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Unbuffered_Output_Stream_Buffer() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the device access buffer.
     */
    constexpr Unbuffered_Output_Stream_Buffer( Transmitter transmitter ) noexcept :
        m_transmitter{ std::move( transmitter ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Unbuffered_Output_Stream_Buffer( Unbuffered_Output_Stream_Buffer && source ) noexcept = default;

    /**
     * \todo #29
     */
    Unbuffered_Output_Stream_Buffer( Unbuffered_Output_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Unbuffered_Output_Stream_Buffer() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Unbuffered_Output_Stream_Buffer && expression ) noexcept
        -> Unbuffered_Output_Stream_Buffer & = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Unbuffered_Output_Stream_Buffer const & ) = delete;

    /**
     * \brief Initialize the transmitter's hardware.
     *
     * \return Nothing if initializing the transmitter's hardware succeeded.
     * \return An error code if initializing the transmitter's hardware failed.
     */
    auto initialize() noexcept
    {
        return m_transmitter.initialize();
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( char )
     */
    virtual auto put( char character ) noexcept -> Result<Void, Error_Code> override final
    {
        return m_transmitter.transmit( character );
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( std::uint8_t )
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        return m_transmitter.transmit( value );
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( std::int8_t )
     */
    virtual auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        return m_transmitter.transmit( value );
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::flush()
     */
    virtual auto flush() noexcept -> Result<Void, Error_Code> override final
    {
        return {};
    }

  private:
    /**
     * \brief The asynchronous serial transmitter abstracted by the device access buffer.
     */
    Transmitter m_transmitter{};
};

/**
 * \brief Unbuffered asynchronous serial output stream.
 *
 * \tparam Transmitter The type of asynchronous serial transmitter that is abstracted by
 *         the stream.
 */
template<typename Transmitter>
class Unbuffered_Output_Stream : public Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Unbuffered_Output_Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream.
     */
    constexpr Unbuffered_Output_Stream( Transmitter transmitter ) noexcept :
        m_buffer{ std::move( transmitter ) }
    {
        set_buffer( &m_buffer );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Unbuffered_Output_Stream( Unbuffered_Output_Stream && source ) noexcept :
        m_buffer{ std::move( source.m_buffer ) }
    {
        if ( source.buffer_is_set() ) {
            set_buffer( &m_buffer );

            source.set_buffer( nullptr );
        } // if
    }

    /**
     * \todo #29
     */
    Unbuffered_Output_Stream( Unbuffered_Output_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Unbuffered_Output_Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Unbuffered_Output_Stream && expression ) noexcept
    {
        m_buffer = std::move( expression.m_buffer );

        if ( expression.buffer_is_set() ) {
            set_buffer( &m_buffer );

            expression.set_buffer( nullptr );
        } else {
            set_buffer( nullptr );
        } // else

        return *this;
    }

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Unbuffered_Output_Stream const & ) = delete;

  private:
    /**
     * \brief The stream's device access buffer.
     */
    Unbuffered_Output_Stream_Buffer<Transmitter> m_buffer{};
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
