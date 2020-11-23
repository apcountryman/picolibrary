# picolibrary
#
# Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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

# File: test/unit/picolibrary/CMakeLists.txt
# Description: picolibrary unit tests CMake rules.

# build the picolibrary::Algorithm unit tests
add_subdirectory( algorithm )

# build the picolibrary::Asynchronous_Serial unit tests
add_subdirectory( asynchronous_serial )

# build the picolibrary::Bit_Manupulation unit tests
add_subdirectory( bit_manipulation )

# build the picolibrary::CRC unit tests
add_subdirectory( crc )

# build the picolibrary::Error_Code unit tests
add_subdirectory( error_code )

# build the picolibrary::Format unit tests
add_subdirectory( format )

# build the picolibrary::Generic_Error unit tests
add_subdirectory( generic_error )

# build the picolibrary::Generic_Error_Category unit tests
add_subdirectory( generic_error_category )

# build the picolibrary::GPIO unit tests
add_subdirectory( gpio )

# build the picolibrary::Output_Stream unit tests
add_subdirectory( output_stream )

# build the picolibrary::SPI unit tests
add_subdirectory( spi )

# build the picolibrary::Stream unit tests
add_subdirectory( stream )

# build the picolibrary::Stream_Buffer unit tests
add_subdirectory( stream_buffer )
