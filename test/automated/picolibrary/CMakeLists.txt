# picolibrary
#
# Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# build the picolibrary::ADC automated tests
add_subdirectory( adc )

# build the picolibrary algorithm automated tests
add_subdirectory( algorithm )

# build the picolibrary::Asynchronous_Serial automated tests
add_subdirectory( asynchronous_serial )

# build the picolibrary bit manipulation automated tests
add_subdirectory( bit_manipulation )

# build the picolibrary::CRC automated tests
add_subdirectory( crc )

# build the picolibrary::Error_Code automated tests
add_subdirectory( error_code )

# build the picolibrary::Event automated tests
add_subdirectory( event )

# build the picolibrary::Format automated tests
add_subdirectory( format )

# build the picolibrary::Generic_Error automated tests
add_subdirectory( generic_error )

# build the picolibrary::Generic_Error_Category automated tests
add_subdirectory( generic_error_category )

# build the picolibrary::GPIO automated tests
add_subdirectory( gpio )

# build the picolibrary::I2C automated tests
add_subdirectory( i2c )

# build the picolibrary::Indicator automated tests
add_subdirectory( indicator )

# build the picolibrary::Interrupt automated tests
add_subdirectory( interrupt )

# build the picolibrary::IPv4 automated tests
add_subdirectory( ipv4 )

# build the picolibrary::MAC_Address automated tests
add_subdirectory( mac_address )

# build the picolibrary::Microchip automated tests
add_subdirectory( microchip )

# build the picolibrary::Output_Stream automated tests
add_subdirectory( output_stream )

# build the picolibrary::Reliable_Stream automated tests
add_subdirectory( reliable_stream )

# build the picolibrary::Reliable_Stream_Buffer automated tests
add_subdirectory( reliable_stream_buffer )

# build the picolibrary::SPI automated tests
add_subdirectory( spi )

# build the picolibrary::Stream automated tests
add_subdirectory( stream )

# build the picolibrary::Stream_Buffer automated tests
add_subdirectory( stream_buffer )

# build the picolibrary::WIZnet automated tests
add_subdirectory( wiznet )
