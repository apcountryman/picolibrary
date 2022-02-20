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
 * \brief picolibrary::Asynchronous_Serial stream interface.
 */

#ifndef PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
#define PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H

#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/void.h"

namespace picolibrary::Asynchronous_Serial {

/**
 * \brief Output stream concept.
 *
 * \tparam Transmitter The type of transmitter to abstract with the stream.
 */
template<typename Transmitter>
class Output_Stream_Concept : public Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Output_Stream_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream.
     */
    Output_Stream_Concept( Transmitter transmitter ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Output_Stream_Concept( Output_Stream_Concept && source ) noexcept;

    Output_Stream_Concept( Output_Stream_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Stream_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Output_Stream_Concept && expression ) noexcept -> Output_Stream_Concept &;

    auto operator=( Output_Stream_Concept const & ) = delete;
};

/**
 * \brief Unbuffered output stream device access buffer.
 *
 * \tparam Transmitter The type of transmitter to abstract with the stream device access
 *         buffer.
 */
template<typename Transmitter>
class Unbuffered_Output_Stream_Buffer final : public Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Unbuffered_Output_Stream_Buffer() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream device access
     *            buffer.
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
    auto operator=( Unbuffered_Output_Stream_Buffer && expression ) noexcept
        -> Unbuffered_Output_Stream_Buffer & = default;

    auto operator=( Unbuffered_Output_Stream_Buffer const & ) = delete;

    /**
     * \brief Transmit a character.
     *
     * \param[in] character The character to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( char character ) noexcept -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit( character );

        return {};
    }

    /**
     * \brief Transmit a block of characters.
     *
     * \param[in] begin The beginning of the block of characters to transmit.
     * \param[in] end The end of the block of characters to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( char const * begin, char const * end ) noexcept
        -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit(
            reinterpret_cast<std::uint8_t const *>( begin ),
            reinterpret_cast<std::uint8_t const *>( end ) );

        return {};
    }

    /**
     * \brief Transmit a null-terminated string.
     *
     * \param[in] string The null-terminated string to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( char const * string ) noexcept -> Result<Void, Error_Code> override final
    {
        while ( auto const character = *string++ ) {
            m_transmitter.transmit( character );
        } // while

        return {};
    }

    /**
     * \brief Transmit an unsigned byte.
     *
     * \param[in] value The unsigned byte to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( std::uint8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit( value );

        return {};
    }

    /**
     * \brief Transmit a block of unsigned bytes.
     *
     * \param[in] begin The beginning of the block of unsigned bytes to transmit.
     * \param[in] end The end of the block of unsigned bytes to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit( begin, end );

        return {};
    }

    /**
     * \brief Transmit a signed byte.
     *
     * \param[in] value The signed byte to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( std::int8_t value ) noexcept -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit( value );

        return {};
    }

    /**
     * \brief Transmit a block of signed bytes.
     *
     * \param[in] begin The beginning of the block of signed bytes to transmit.
     * \param[in] end The end of the block of signed bytes to transmit.
     *
     * \return Nothing.
     */
    virtual auto put( std::int8_t const * begin, std::int8_t const * end ) noexcept
        -> Result<Void, Error_Code> override final
    {
        m_transmitter.transmit(
            reinterpret_cast<std::uint8_t const *>( begin ),
            reinterpret_cast<std::uint8_t const *>( end ) );

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
     * \brief The transmitter abstracted by the stream device access buffer.
     */
    Transmitter m_transmitter{};
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
