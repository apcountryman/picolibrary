# picolibrary
#
# Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
# contributors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
# file except in compliance with the License. You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under
# the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the specific language governing
# permissions and limitations under the License.

# Description: picolibrary automated tests CMake rules.

# picolibrary::Adafruit automated tests
add_subdirectory( adafruit )

# picolibrary::ADC automated tests
add_subdirectory( adc )

# picolibrary algorithm automated tests
add_subdirectory( algorithm )

# picolibrary::Asynchronous_Serial automated tests
add_subdirectory( asynchronous_serial )

# picolibrary bit manipulation automated tests
add_subdirectory( bit_manipulation )

# picolibrary::CRC automated tests
add_subdirectory( crc )

# picolibrary::Error_Code automated tests
add_subdirectory( error_code )

# picolibrary::Event automated tests
add_subdirectory( event )

# picolibrary::Format automated tests
add_subdirectory( format )

# picolibrary::Generic_Error automated tests
add_subdirectory( generic_error )

# picolibrary::Generic_Error_Category automated tests
add_subdirectory( generic_error_category )

# picolibrary::GPIO automated tests
add_subdirectory( gpio )

# picolibrary::HSM automated tests
add_subdirectory( hsm )

# picolibrary::I2C automated tests
add_subdirectory( i2c )

# picolibrary::Indicator automated tests
add_subdirectory( indicator )

# picolibrary::Interrupt automated tests
add_subdirectory( interrupt )

# picolibrary::IP automated tests
add_subdirectory( ip )

# picolibrary::IPv4 automated tests
add_subdirectory( ipv4 )

# picolibrary::MAC_Address automated tests
add_subdirectory( mac_address )

# picolibrary::Microchip automated tests
add_subdirectory( microchip )

# picolibrary::Output_Stream automated tests
add_subdirectory( output_stream )

# picolibrary::Reliable_Output_Stream automated tests
add_subdirectory( reliable_output_stream )

# picolibrary::Reliable_Stream automated tests
add_subdirectory( reliable_stream )

# picolibrary::Reliable_Stream_Buffer automated tests
add_subdirectory( reliable_stream_buffer )

# picolibrary::SPI automated tests
add_subdirectory( spi )

# picolibrary::State_Machine automated tests
add_subdirectory( state_machine )

# picolibrary::Stream automated tests
add_subdirectory( stream )

# picolibrary::Stream_Buffer automated tests
add_subdirectory( stream_buffer )

# picolibrary::Texas_Instruments automated tests
add_subdirectory( texas_instruments )

# picolibrary::WIZnet automated tests
add_subdirectory( wiznet )
