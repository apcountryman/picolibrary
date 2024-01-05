/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Interactive::Adafruit::PID781 interface.
 */

#ifndef PICOLIBRARY_TESTING_INTERACTIVE_ADAFRUIT_PID781_H
#define PICOLIBRARY_TESTING_INTERACTIVE_ADAFRUIT_PID781_H

#include <utility>

#include "picolibrary/adafruit/pid781.h"
#include "picolibrary/asynchronous_serial.h"
#include "picolibrary/rom.h"

/**
 * \brief Adafruit PID781 interactive testing facilities.
 */
namespace picolibrary::Testing::Interactive::Adafruit::PID781 {

/**
 * \brief Driver hello world interactive test helper.
 *
 * \tparam Transmitter The type of asynchronous serial transmitter used to communicate
 *         with the PID781.
 *
 * \param[in] transmitter The asynchronous serial transmitter to use to communicate with
 *            the PID781.
 */
template<typename Transmitter>
// NOLINTNEXTLINE(readability-function-size)
void hello_world( Transmitter transmitter ) noexcept
{
    // #lizard forgives the length

    transmitter.initialize();

    auto pid781 = ::picolibrary::Adafruit::PID781::Driver<Asynchronous_Serial::Reliable_Unbuffered_Output_Stream<Transmitter>>{
        std::move( transmitter )
    };

    auto const smiley_face = ::picolibrary::Adafruit::PID781::Custom_Character_ID::_0;

    pid781.set_and_save_backlight_color( { 51, 0, 111 } );
    pid781.set_and_save_backlight_brightness( 255 );
    pid781.set_and_save_lcd_contrast( 200 );
    pid781.illuminate_backlight();
    pid781.enable_auto_scrolling();
    pid781.enable_cursor_blink();
    pid781.create_custom_character(
        smiley_face,
        {
            0b00000,
            0b01010,
            0b01010,
            0b00000,
            0b00000,
            0b10001,
            0b01110,
            0b00000,
        } );

    pid781.clear_screen();

    pid781.home_cursor();
    pid781.put( PICOLIBRARY_ROM_STRING( "Adafruit::PID781" ) );
    pid781.set_cursor_position( { 2, 2 } );
    pid781.print( PICOLIBRARY_ROM_STRING( "Hello, world!" ), smiley_face );
    pid781.flush();
}

} // namespace picolibrary::Testing::Interactive::Adafruit::PID781

#endif // PICOLIBRARY_TESTING_INTERACTIVE_ADAFRUIT_PID781_H
