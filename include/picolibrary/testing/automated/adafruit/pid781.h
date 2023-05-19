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
 * \brief picolibrary::Testing::Automated::Adafruit::PID781 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H
#define PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H

#include <ostream>

#include "picolibrary/adafruit/pid781.h"

namespace picolibrary::Adafruit::PID781 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::Adafruit::PID781::Bit_Rate to.
 * \param[in] stream The picolibrary::Adafruit::PID781::Bit_Rate to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Bit_Rate bit_rate ) -> std::ostream &
{
    switch ( bit_rate ) {
            // clang-format off

        case Bit_Rate::_1200:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_1200";
        case Bit_Rate::_2400:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_2400";
        case Bit_Rate::_4800:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_4800";
        case Bit_Rate::_9600:   return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_9600";
        case Bit_Rate::_19200:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_19200";
        case Bit_Rate::_28800:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_28800";
        case Bit_Rate::_38400:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_38400";
        case Bit_Rate::_57600:  return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_57600";
        case Bit_Rate::_115200: return stream << "::picolibrary::Adafruit::PID781::Bit_Rate::_115200";

            // clang-format on
    } // switch

    return stream << "UNKNOWN";
}

} // namespace picolibrary::Adafruit::PID781

/**
 * \brief Adafruit PID781 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Adafruit::PID781 {
} // namespace picolibrary::Testing::Automated::Adafruit::PID781

#endif // PICOLIBRARY_TESTING_AUTOMATED_ADAFRUIT_PID781_H
