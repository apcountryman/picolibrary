# picolibrary
#
# Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# File: source/CMakeLists.txt
# Description: picolibrary CMake rules.

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/picolibrary/version.cc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
    @ONLY
)

set(
    PICOLIBRARY_SOURCE_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
    "picolibrary.cc"
    "picolibrary/adc.cc"
    "picolibrary/algorithm.cc"
    "picolibrary/asynchronous_serial.cc"
    "picolibrary/asynchronous_serial/stream.cc"
    "picolibrary/bit_manipulation.cc"
    "picolibrary/crc.cc"
    "picolibrary/error.cc"
    "picolibrary/fixed_size_array.cc"
    "picolibrary/format.cc"
    "picolibrary/gpio.cc"
    "picolibrary/i2c.cc"
    "picolibrary/iterator.cc"
    "picolibrary/microchip.cc"
    "picolibrary/microchip/mcp3008.cc"
    "picolibrary/result.cc"
    "picolibrary/spi.cc"
    "picolibrary/stream.cc"
    "picolibrary/utility.cc"
)
set( PICOLIBRARY_LINK_LIBRARIES )

if( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} OR ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing.cc"
    )
endif( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} OR ${PICOLIBRARY_ENABLE_UNIT_TESTING} )

if( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/interactive.cc"
        "picolibrary/testing/interactive/adc.cc"
        "picolibrary/testing/interactive/asynchronous_serial.cc"
        "picolibrary/testing/interactive/asynchronous_serial/stream.cc"
        "picolibrary/testing/interactive/gpio.cc"
        "picolibrary/testing/interactive/microchip.cc"
        "picolibrary/testing/interactive/microchip/mcp3008.cc"
        "picolibrary/testing/interactive/spi.cc"
    )
endif( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )

if( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/unit.cc"
        "picolibrary/testing/unit/adc.cc"
        "picolibrary/testing/unit/asynchronous_serial.cc"
        "picolibrary/testing/unit/crc.cc"
        "picolibrary/testing/unit/error.cc"
        "picolibrary/testing/unit/gpio.cc"
        "picolibrary/testing/unit/microchip.cc"
        "picolibrary/testing/unit/microchip/mcp3008.cc"
        "picolibrary/testing/unit/random.cc"
        "picolibrary/testing/unit/spi.cc"
        "picolibrary/testing/unit/stream.cc"
    )
    list(
        APPEND PICOLIBRARY_LINK_LIBRARIES
        gmock
        gtest
    )
endif( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )

add_library(
    picolibrary STATIC
    ${PICOLIBRARY_SOURCE_FILES}
)
target_include_directories(
    picolibrary
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_compile_definitions(
    picolibrary
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION}>,PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION,>"
)
target_link_libraries(
    picolibrary
    ${PICOLIBRARY_LINK_LIBRARIES}
)
