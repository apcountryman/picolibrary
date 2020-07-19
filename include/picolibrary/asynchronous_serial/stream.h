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

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

namespace picolibrary::Asynchronous_Serial {

/**
 * \brief Unbuffered asynchronous serial output stream device access buffer.
 *
 * \tparam Transmitter The type of asynchronous serial transmitter the device access
 *         buffer interfaces with.
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
     * \param[in] transmitter The transmitter to associate with the device access buffer.
     */
    constexpr Unbuffered_Output_Stream_Buffer( Transmitter & transmitter ) noexcept :
        m_transmitter{ &transmitter }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Unbuffered_Output_Stream_Buffer( Unbuffered_Output_Stream_Buffer && source ) noexcept :
        m_transmitter{ source.m_transmitter }
    {
        source.m_transmitter = nullptr;
    }

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
     * \brief Get the transmitter associated with the device access buffer.
     *
     * \return The transmitter associated with the device access buffer.
     */
    constexpr auto transmitter() const noexcept -> Transmitter const *
    {
        return m_transmitter;
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( char )
     */
    virtual auto put( char character ) noexcept -> Result<Void, Error_Code> override final
    {
        static_cast<void>( character );

        return {};
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( std::uint8_t )
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        static_cast<void>( value );

        return {};
    }

    /**
     * \copydoc picolibrary::Stream_Buffer::put( std::int8_t )
     */
    virtual auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        static_cast<void>( value );

        return {};
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
     * \brief The transmitter associated with the device access buffer.
     */
    Transmitter * m_transmitter{};
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
