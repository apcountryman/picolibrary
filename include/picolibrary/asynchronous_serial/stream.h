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

#include "picolibrary/stream.h"

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
 * \brief Reliable output stream concept.
 *
 * \tparam Transmitter The type of transmitter to abstract with the stream.
 */
template<typename Transmitter>
class Reliable_Output_Stream_Concept : public Reliable_Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    Reliable_Output_Stream_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream.
     */
    Reliable_Output_Stream_Concept( Transmitter transmitter ) noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Reliable_Output_Stream_Concept( Reliable_Output_Stream_Concept && source ) noexcept;

    Reliable_Output_Stream_Concept( Reliable_Output_Stream_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Output_Stream_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Reliable_Output_Stream_Concept && expression ) noexcept
        -> Reliable_Output_Stream_Concept &;

    auto operator=( Reliable_Output_Stream_Concept const & ) = delete;
};

/**
 * \brief Reliable output stream device access buffer.
 *
 * \tpamara Transmitter The type of transmitter to abstract with the stream device access
 *          buffer.
 */
template<typename Transmitter>
class Reliable_Unbuffered_Output_Stream_Buffer final : public Reliable_Stream_Buffer {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Reliable_Unbuffered_Output_Stream_Buffer() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream device access
     *            buffer.
     */
    constexpr Reliable_Unbuffered_Output_Stream_Buffer( Transmitter transmitter ) noexcept :
        m_transmitter{ std::move( transmitter ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reliable_Unbuffered_Output_Stream_Buffer(
        Reliable_Unbuffered_Output_Stream_Buffer && source ) noexcept = default;

    Reliable_Unbuffered_Output_Stream_Buffer( Reliable_Unbuffered_Output_Stream_Buffer const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Unbuffered_Output_Stream_Buffer() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reliable_Unbuffered_Output_Stream_Buffer && expression ) noexcept
        -> Reliable_Unbuffered_Output_Stream_Buffer & = default;

    auto operator=( Reliable_Unbuffered_Output_Stream_Buffer const & ) = delete;

    /**
     * \brief Transmit a character.
     *
     * \param[in] character The character to transmit.
     */
    void put( char character ) noexcept override final
    {
        m_transmitter.transmit( character );
    }

    /**
     * \brief Transmit a block of characters.
     *
     * \param[in] begin The beginning of the block of characters to transmit.
     * \param[in] end The end of the block of characters to transmit.
     */
    void put( char const * begin, char const * end ) noexcept override final
    {
        m_transmitter.transmit(
            reinterpret_cast<std::uint8_t const *>( begin ),
            reinterpret_cast<std::uint8_t const *>( end ) );
    }

    /**
     * \brief Transmit a null-terminated string.
     *
     * \param[in] string The null-terminated string to transmit.
     */
    void put( char const * string ) noexcept override final
    {
        while ( auto const character = *string++ ) {
            m_transmitter.transmit( character );
        } // while
    }

    /**
     * \brief Transmit an unsigned byte.
     *
     * \param[in] value The unsigned byte to transmit.
     */
    void put( std::uint8_t value ) noexcept override final
    {
        m_transmitter.transmit( value );
    }

    /**
     * \brief Transmit a block of unsigned bytes.
     *
     * \param[in] begin The beginning of the block of unsigned bytes to transmit.
     * \param[in] end The end of the block of unsigned bytes to transmit.
     */
    void put( std::uint8_t const * begin, std::uint8_t const * end ) noexcept override final
    {
        m_transmitter.transmit( begin, end );
    }

    /**
     * \brief Transmit a signed byte.
     *
     * \param[in] value The signed byte to transmit.
     */
    void put( std::int8_t value ) noexcept override final
    {
        m_transmitter.transmit( value );
    }

    /**
     * \brief Transmit a block of signed bytes.
     *
     * \param[in] begin The beginning of the block of signed bytes to transmit.
     * \param[in] end The end of the block of signed bytes to transmit.
     */
    void put( std::int8_t const * begin, std::int8_t const * end ) noexcept override final
    {
        m_transmitter.transmit(
            reinterpret_cast<std::uint8_t const *>( begin ),
            reinterpret_cast<std::uint8_t const *>( end ) );
    }

    /**
     * \brief Do nothing.
     */
    void flush() noexcept override final
    {
    }

  private:
    /**
     * \brief The transmitter abstracted by the stream device access buffer.
     */
    Transmitter m_transmitter{};
};

/**
 * \brief Reliable unbuffered output stream.
 *
 * \tparam Transmitter The type of transmitter to abstract with the stream.
 */
template<typename Transmitter>
class Reliable_Unbuffered_Output_Stream : public Reliable_Output_Stream {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Reliable_Unbuffered_Output_Stream() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] transmitter The transmitter to abstract with the stream.
     */
    constexpr Reliable_Unbuffered_Output_Stream( Transmitter transmitter ) noexcept :
        m_buffer{ std::move( transmitter ) }
    {
        set_buffer( &m_buffer );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reliable_Unbuffered_Output_Stream( Reliable_Unbuffered_Output_Stream && source ) noexcept
    {
        if ( source.buffer_is_set() ) {
            set_buffer( &m_buffer );

            source.set_buffer( nullptr );
        } // if
    }

    Reliable_Unbuffered_Output_Stream( Reliable_Unbuffered_Output_Stream const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Reliable_Unbuffered_Output_Stream() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reliable_Unbuffered_Output_Stream && expression ) noexcept
        -> Reliable_Unbuffered_Output_Stream &
    {
        if ( &expression != this ) {
            m_buffer = std::move( expression.m_buffer );

            if ( expression.buffer_is_set() ) {
                set_buffer( &m_buffer );

                expression.set_buffer( nullptr );
            } else {
                set_buffer( nullptr );
            } // else
        }     // if

        return *this;
    }

    auto operator=( Reliable_Unbuffered_Output_Stream const & ) = delete;

  private:
    /**
     * \brief The stream's device access buffer.
     */
    Reliable_Unbuffered_Output_Stream_Buffer<Transmitter> m_buffer{};
};

} // namespace picolibrary::Asynchronous_Serial

#endif // PICOLIBRARY_ASYNCHRONOUS_SERIAL_STREAM_H
