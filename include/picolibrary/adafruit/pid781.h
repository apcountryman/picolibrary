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
 * \brief picolibrary::Adafruit::PID781 interface.
 */

#ifndef PICOLIBRARY_ADAFRUIT_PID781_H
#define PICOLIBRARY_ADAFRUIT_PID781_H

#include <cstddef>
#include <cstdint>
#include <limits>

#include "picolibrary/array.h"
#include "picolibrary/gpio.h"
#include "picolibrary/result.h"
#include "picolibrary/rgb.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

/**
 * \brief Adafruit PID781 facilities.
 */
namespace picolibrary::Adafruit::PID781 {

/**
 * \brief Bit rate.
 */
enum class Bit_Rate : std::uint8_t {
    _1200   = 0x53, ///< 1200 bits/second.
    _2400   = 0x29, ///< 2400 bits/second.
    _4800   = 0xCF, ///< 4800 bits/second.
    _9600   = 0x67, ///< 9600 bits/second.
    _19200  = 0x33, ///< 19200 bits/second.
    _28800  = 0x22, ///< 28800 bits/second.
    _38400  = 0x19, ///< 38400 bits/second.
    _57600  = 0x10, ///< 57600 bits/second.
    _115200 = 0x08, ///< 115200 bits/second.
};

/**
 * \brief LCD size.
 */
enum class LCD_Size : std::uint16_t {
    _16X2 = ( 16 << std::numeric_limits<std::uint8_t>::digits ) | 2, ///< 16 columns, 2 rows.
    _20X4 = ( 20 << std::numeric_limits<std::uint8_t>::digits ) | 4, ///< 20 columns, 4 rows.
};

/**
 * \brief Get the number of columns an LCD has.
 *
 * \param[in] lcd_size The size of the LCD.
 *
 * \return The number of columns the LCD has.
 */
constexpr auto columns( LCD_Size lcd_size ) noexcept -> std::uint8_t
{
    return to_underlying( lcd_size ) >> std::numeric_limits<std::uint8_t>::digits;
}

/**
 * \brief Get the number of rows an LCD has.
 *
 * \param[in] lcd_size The size of the LCD.
 *
 * \return The number of rows the LCD has.
 */
constexpr auto rows( LCD_Size lcd_size ) noexcept -> std::uint8_t
{
    return to_underlying( lcd_size );
}

/**
 * \brief Cursor position.
 *
 * \attention Cursor positions are 1-indexed (e.g. (1,1) is the top left position).
 */
struct Cursor_Position {
    /**
     * \brief The column component of the cursor position.
     */
    std::uint8_t column;

    /**
     * \brief The row component of the cursor position.
     */
    std::uint8_t row;
};

/**
 * \brief Custom character bank.
 */
enum class Custom_Character_Bank : std::uint8_t {
    _0 = 0, ///< 0.
    _1 = 1, ///< 1.
    _2 = 2, ///< 2.
    _3 = 3, ///< 3.
    _4 = 4, ///< 4.
};

/**
 * \brief Custom character ID.
 */
enum class Custom_Character_ID : std::uint8_t {
    _0 = 0, ///< 0.
    _1 = 1, ///< 1.
    _2 = 2, ///< 2.
    _3 = 3, ///< 3.
    _4 = 4, ///< 4.
    _5 = 5, ///< 5.
    _6 = 6, ///< 6.
    _7 = 7, ///< 7.
};

/**
 * \brief Custom character pixel matrix.
 */
using Custom_Character_Pixel_Matrix = Array<std::uint8_t, 8>;

/**
 * \brief Pin.
 */
enum class Pin : std::uint8_t {
    PB0 = 1, ///< PB0.
    PC2 = 2, ///< PC2.
    PC4 = 3, ///< PC4.
    PC7 = 4, ///< PC7.
};

/**
 * \brief Driver.
 *
 * \tparam Reliable_Output_Stream The type of reliable output stream used to communicate
 *         with the PID781.
 */
template<typename Reliable_Output_Stream>
class Driver : public Reliable_Output_Stream {
  public:
    using Reliable_Output_Stream::Reliable_Output_Stream;

    /**
     * \brief Destructor.
     */
    ~Driver() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Driver && expression ) noexcept -> Driver & = default;

    auto operator=( Driver const & ) = delete;

    /**
     * \brief Set and save the bit rate.
     *
     * \param[in] bit_rate The desired bit rate.
     */
    void set_and_save_bit_rate( Bit_Rate bit_rate ) noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_BIT_RATE,
            to_underlying( bit_rate ),
        } );
    }

    /**
     * \brief Report the bit rate.
     */
    void report_bit_rate() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            REPORT_BIT_RATE,
        } );
    }

    /**
     * \brief Set and save the LCD size.
     *
     * \param[in] lcd_size The desired LCD size.
     */
    void set_and_save_lcd_size( LCD_Size lcd_size ) noexcept
    {
        put( Data<4>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_LCD_SIZE,
            columns( lcd_size ),
            rows( lcd_size ),
        } );
    }

    /**
     * \brief Set and save the backlight color.
     *
     * \param[in] color The desired backlight color.
     */
    void set_and_save_backlight_color( RGB::Color const & color )
    {
        put( Data<5>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_BACKLIGHT_COLOR,
            color.r,
            color.g,
            color.b,
        } );
    }

    /**
     * \brief Set and save the backlight brightness.
     *
     * \param[in] brightness The desired backlight brightness.
     */
    void set_and_save_backlight_brightness( std::uint8_t brightness )
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_BACKLIGHT_BRIGHTNESS,
            brightness,
        } );
    }

    /**
     * \brief Set and save the LCD contrast.
     *
     * \param[in] contrast The desired LCD contrast.
     */
    void set_and_save_lcd_contrast( std::uint8_t contrast )
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_LCD_CONTRAST,
            contrast,
        } );
    }

    /**
     * \brief Set and save the splash screen.
     *
     * \attention Unlike other functions, this function does not capture the data
     *            associated with the command. It is the user's responsibility to write
     *            the contents of the splash screen (columns * rows characters) to the
     *            stream after calling this function.
     */
    void set_and_save_splash_screen() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_SPLASH_SCREEN,
        } );
    }

    /**
     * \brief Illuminate the backlight.
     */
    void illuminate_backlight() noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            ILLUMINATE_BACKLIGHT,
            0x00,
        } );
    }

    /**
     * \brief Extinguish the backlight.
     */
    void extinguish_backlight() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            EXTINGUISH_BACKLIGHT,
        } );
    }

    /**
     * \brief Enable auto scrolling.
     */
    void enable_auto_scrolling() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            ENABLE_AUTO_SCROLLING,
        } );
    }

    /**
     * \brief Disable auto scrolling.
     */
    void disable_auto_scrolling() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            DISABLE_AUTO_SCROLLING,
        } );
    }

    /**
     * \brief Enable cursor underline.
     */
    void enable_cursor_underline() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            ENABLE_CURSOR_UNDERLINE,
        } );
    }

    /**
     * \brief Disable cursor underline.
     */
    void disable_cursor_underline() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            DISABLE_CURSOR_UNDERLINE,
        } );
    }

    /**
     * \brief Enable cursor blink.
     */
    void enable_cursor_blink() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            ENABLE_CURSOR_BLINK,
        } );
    }

    /**
     * \brief Disable cursor blink.
     */
    void disable_cursor_blink() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            DISABLE_CURSOR_BLINK,
        } );
    }

    /**
     * \brief Set the cursor position.
     *
     * \param[in] cursor_position The desired cursor position.
     */
    void set_cursor_position( Cursor_Position cursor_position ) noexcept
    {
        put( Data<4>{
            COMMAND_INTRODUCER,
            SET_CURSOR_POSITION,
            cursor_position.column,
            cursor_position.row,
        } );
    }

    /**
     * \brief Home the cursor.
     */
    void home_cursor() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            HOME_CURSOR,
        } );
    }

    /**
     * \brief Retreat the cursor.
     */
    void retreat_cursor() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            RETREAT_CURSOR,
        } );
    }

    /**
     * \brief Advance the cursor.
     */
    void advance_cursor() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            ADVANCE_CURSOR,
        } );
    }

    /**
     * \brief Clear the screen.
     */
    void clear_screen() noexcept
    {
        put( Data<2>{
            COMMAND_INTRODUCER,
            CLEAR_SCREEN,
        } );
    }

    /**
     * \brief Create a custom character.
     *
     * \param[in] id The custom character's ID.
     * \param[in] pixel_matrix The custom character's pixel matrix.
     */
    void create_custom_character( Custom_Character_ID id, Custom_Character_Pixel_Matrix const & pixel_matrix ) noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            CREATE_CUSTOM_CHARACTER,
            to_underlying( id ),
        } );
        put( pixel_matrix );
    }

    /**
     * \brief Create and save a custom character.
     *
     * \param[in] bank The custom character bank to save the custom character to.
     * \param[in] id The custom character's ID.
     * \param[in] pixel_matrix The custom character's pixel matrix.
     */
    void create_and_save_custom_character(
        Custom_Character_Bank                 bank,
        Custom_Character_ID                   id,
        Custom_Character_Pixel_Matrix const & pixel_matrix ) noexcept
    {
        put( Data<4>{
            COMMAND_INTRODUCER,
            CREATE_AND_SAVE_CUSTOM_CHARACTER,
            to_underlying( bank ),
            to_underlying( id ),
        } );
        put( pixel_matrix );
    }

    /**
     * \brief Load a custom character bank.
     *
     * \param[in] bank The custom character bank to load.
     */
    void load_custom_character_bank( Custom_Character_Bank bank ) noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            LOAD_CUSTOM_CHARACTER_BANK,
            to_underlying( bank ),
        } );
    }

    /**
     * \brief Set and save an output pin's initial state.
     *
     * \param[in] pin The pin whose initial state is to be set and saved.
     * \param[in] initial_pin_state The desired initial pin state.
     */
    void set_and_save_output_pin_initial_state( Pin pin, GPIO::Initial_Pin_State initial_pin_state ) noexcept
    {
        put( Data<4>{
            COMMAND_INTRODUCER,
            SET_AND_SAVE_OUTPUT_PIN_INITIAL_STATE,
            to_underlying( pin ),
            to_underlying( initial_pin_state ),
        } );
    }

    /**
     * \brief Transition an output pin to the high state.
     *
     * \param[in] pin The pin to transition to the high state.
     */
    void transition_output_pin_to_high( Pin pin ) noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            TRANSITION_OUTPUT_PIN_TO_HIGH,
            to_underlying( pin ),
        } );
    }

    /**
     * \brief Transition an output pin to the low state.
     *
     * \param[in] pin The pin to transition to the low state.
     */
    void transition_output_pin_to_low( Pin pin ) noexcept
    {
        put( Data<3>{
            COMMAND_INTRODUCER,
            TRANSITION_OUTPUT_PIN_TO_LOW,
            to_underlying( pin ),
        } );
    }

  private:
    /**
     * \brief Data.
     *
     * \tparam N The number of bytes the data is comprised of.
     */
    template<std::size_t N>
    using Data = Array<std::uint8_t, N>;

    /**
     * \brief Command introducer.
     */
    static constexpr auto COMMAND_INTRODUCER = std::uint8_t{ 0xFE };

    /**
     * \brief Commands.
     */
    enum Command : std::uint8_t {
        SET_AND_SAVE_BIT_RATE             = 0x39, ///< Set and save bit rate.
        REPORT_BIT_RATE                   = 0xD2, ///< Report bit rate.
        SET_AND_SAVE_LCD_SIZE             = 0xD1, ///< Set and save LCD size.
        SET_AND_SAVE_BACKLIGHT_COLOR      = 0xD0, ///< Set and save backlight color.
        SET_AND_SAVE_BACKLIGHT_BRIGHTNESS = 0x98, ///< Set and save backlight brightness.
        SET_AND_SAVE_LCD_CONTRAST         = 0x91, ///< Set and save LCD contrast.
        SET_AND_SAVE_SPLASH_SCREEN        = 0x40, ///< Set and save splash screen.
        ILLUMINATE_BACKLIGHT              = 0x42, ///< Illuminate backlight.
        EXTINGUISH_BACKLIGHT              = 0x46, ///< Extinguish backlight.
        ENABLE_AUTO_SCROLLING             = 0x51, ///< Enable auto scrolling.
        DISABLE_AUTO_SCROLLING            = 0x52, ///< Disable auto scrolling.
        ENABLE_CURSOR_UNDERLINE           = 0x4A, ///< Enable cursor underline.
        DISABLE_CURSOR_UNDERLINE          = 0x4B, ///< Disable cursor underline.
        ENABLE_CURSOR_BLINK               = 0x53, ///< Enable cursor blink.
        DISABLE_CURSOR_BLINK              = 0x54, ///< Disable cursor blink.
        SET_CURSOR_POSITION               = 0x47, ///< Set cursor position.
        HOME_CURSOR                       = 0x48, ///< Home cursor.
        RETREAT_CURSOR                    = 0x4C, ///< Retreat cursor.
        ADVANCE_CURSOR                    = 0x4D, ///< Advance cursor.
        CLEAR_SCREEN                      = 0x58, ///< Clear screen.
        CREATE_CUSTOM_CHARACTER           = 0x4E, ///< Create custom character.
        CREATE_AND_SAVE_CUSTOM_CHARACTER  = 0xC1, ///< Create and save custom character.
        LOAD_CUSTOM_CHARACTER_BANK        = 0xC0, ///< Load custom character bank.
        SET_AND_SAVE_OUTPUT_PIN_INITIAL_STATE = 0xC3, ///< Set and save output pin initial state.
        TRANSITION_OUTPUT_PIN_TO_HIGH = 0x57,         ///< Transition output pin to high.
        TRANSITION_OUTPUT_PIN_TO_LOW  = 0x56,         ///< Transition output pin to low.
    };

    /**
     * \brief Write data to the stream.
     *
     * \tparam N The number of bytes of data to write to the stream.
     *
     * \param[in] data The data to write to the stream.
     */
    template<std::size_t N>
    void put( Data<N> const & data ) noexcept
    {
        Reliable_Output_Stream::put( data.begin(), data.end() );
    }
};

} // namespace picolibrary::Adafruit::PID781

namespace picolibrary {

/**
 * \brief picolibrary::Adafruit::PID781::Custom_Character_ID output formatter.
 */
template<>
class Output_Formatter<Adafruit::PID781::Custom_Character_ID> {
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
     * \brief Write the formatted picolibrary::Adafruit::PID781::Custom_Character_ID to
     *        the stream.
     *
     * \param[in] stream The stream to write the formatted
     *            picolibrary::Adafruit::PID781::Custom_Character_ID to.
     * \param[in] custom_character_id The
     *            picolibrary::Adafruit::PID781::Custom_Character_ID to format.
     *
     * \return The number of characters written to the stream if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Adafruit::PID781::Custom_Character_ID custom_character_id ) const noexcept
        -> Result<std::size_t>
    {
        auto result = stream.put( to_underlying( custom_character_id ) );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return std::size_t{ 1 };
    }

    /**
     * \brief Write the formatted picolibrary::Adafruit::PID781::Custom_Character_ID to
     *        the stream.
     *
     * \param[in] stream The stream to write the formatted
     *            picolibrary::Adafruit::PID781::Custom_Character_ID to.
     * \param[in] custom_character_id The
     *            picolibrary::Adafruit::PID781::Custom_Character_ID to format.
     *
     * \return The number of characters written to the stream.
     */
    auto print( Reliable_Output_Stream & stream, Adafruit::PID781::Custom_Character_ID custom_character_id ) const noexcept
        -> std::size_t
    {
        stream.put( to_underlying( custom_character_id ) );

        return std::size_t{ 1 };
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_ADAFRUIT_PID781_H
