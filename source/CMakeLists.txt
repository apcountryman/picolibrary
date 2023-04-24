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

# Description: picolibrary CMake rules.

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/picolibrary/version.cc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
    @ONLY
)

set(
    PICOLIBRARY_SOURCE_FILES
    "picolibrary.cc"
    "picolibrary/adafruit.cc"
    "picolibrary/adafruit/pid781.cc"
    "picolibrary/adc.cc"
    "picolibrary/algorithm.cc"
    "picolibrary/array.cc"
    "picolibrary/asynchronous_serial.cc"
    "picolibrary/bit_manipulation.cc"
    "picolibrary/circular_buffer.cc"
    "picolibrary/crc.cc"
    "picolibrary/error.cc"
    "picolibrary/event.cc"
    "picolibrary/fixed_capacity_vector.cc"
    "picolibrary/format.cc"
    "picolibrary/gpio.cc"
    "picolibrary/hsm.cc"
    "picolibrary/i2c.cc"
    "picolibrary/indicator.cc"
    "picolibrary/interrupt.cc"
    "picolibrary/ip.cc"
    "picolibrary/ip/network_stack.cc"
    "picolibrary/ip/tcp.cc"
    "picolibrary/ipv4.cc"
    "picolibrary/mac_address.cc"
    "picolibrary/microchip.cc"
    "picolibrary/microchip/mcp23008.cc"
    "picolibrary/microchip/mcp23s08.cc"
    "picolibrary/microchip/mcp23x08.cc"
    "picolibrary/microchip/mcp3008.cc"
    "picolibrary/postcondition.cc"
    "picolibrary/precondition.cc"
    "picolibrary/result.cc"
    "picolibrary/rgb.cc"
    "picolibrary/rom.cc"
    "picolibrary/spi.cc"
    "picolibrary/state_machine.cc"
    "picolibrary/stream.cc"
    "picolibrary/utility.cc"
    "picolibrary/wiznet.cc"
    "picolibrary/wiznet/w5500.cc"
    "picolibrary/wiznet/w5500/ip.cc"
    "picolibrary/wiznet/w5500/ip/network_stack.cc"
    "picolibrary/wiznet/w5500/ip/tcp.cc"
)
set( PICOLIBRARY_LINK_LIBRARIES )

if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} OR ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing.cc"
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} OR ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )

if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/automated.cc"
        "picolibrary/testing/automated/adafruit.cc"
        "picolibrary/testing/automated/adafruit/pid781.cc"
        "picolibrary/testing/automated/adc.cc"
        "picolibrary/testing/automated/asynchronous_serial.cc"
        "picolibrary/testing/automated/crc.cc"
        "picolibrary/testing/automated/error.cc"
        "picolibrary/testing/automated/event.cc"
        "picolibrary/testing/automated/gpio.cc"
        "picolibrary/testing/automated/i2c.cc"
        "picolibrary/testing/automated/indicator.cc"
        "picolibrary/testing/automated/interrupt.cc"
        "picolibrary/testing/automated/ip.cc"
        "picolibrary/testing/automated/ip/network_stack.cc"
        "picolibrary/testing/automated/ip/tcp.cc"
        "picolibrary/testing/automated/ipv4.cc"
        "picolibrary/testing/automated/mac_address.cc"
        "picolibrary/testing/automated/microchip.cc"
        "picolibrary/testing/automated/microchip/mcp23008.cc"
        "picolibrary/testing/automated/microchip/mcp23s08.cc"
        "picolibrary/testing/automated/microchip/mcp23x08.cc"
        "picolibrary/testing/automated/microchip/mcp3008.cc"
        "picolibrary/testing/automated/mock_handle.cc"
        "picolibrary/testing/automated/random.cc"
        "picolibrary/testing/automated/rgb.cc"
        "picolibrary/testing/automated/spi.cc"
        "picolibrary/testing/automated/stream.cc"
        "picolibrary/testing/automated/wiznet.cc"
        "picolibrary/testing/automated/wiznet/w5500.cc"
        "picolibrary/testing/automated/wiznet/w5500/ip.cc"
        "picolibrary/testing/automated/wiznet/w5500/ip/network_stack.cc"
        "picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc"
    )
    list(
        APPEND PICOLIBRARY_LINK_LIBRARIES
        gmock
        gtest
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )

if( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/interactive.cc"
        "picolibrary/testing/interactive/adafruit.cc"
        "picolibrary/testing/interactive/adc.cc"
        "picolibrary/testing/interactive/asynchronous_serial.cc"
        "picolibrary/testing/interactive/gpio.cc"
        "picolibrary/testing/interactive/i2c.cc"
        "picolibrary/testing/interactive/ip.cc"
        "picolibrary/testing/interactive/ip/tcp.cc"
        "picolibrary/testing/interactive/microchip.cc"
        "picolibrary/testing/interactive/microchip/mcp23008.cc"
        "picolibrary/testing/interactive/microchip/mcp23s08.cc"
        "picolibrary/testing/interactive/microchip/mcp3008.cc"
        "picolibrary/testing/interactive/spi.cc"
        "picolibrary/testing/interactive/stream.cc"
        "picolibrary/testing/interactive/wiznet.cc"
        "picolibrary/testing/interactive/wiznet/w5500.cc"
        "picolibrary/testing/interactive/wiznet/w5500/ip.cc"
        "picolibrary/testing/interactive/wiznet/w5500/ip/network_stack.cc"
        "picolibrary/testing/interactive/wiznet/w5500/ip/tcp.cc"
    )
endif( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )

add_library(
    picolibrary STATIC
    ${PICOLIBRARY_SOURCE_FILES}
)
target_include_directories(
    picolibrary
    PUBLIC "${PICOLIBRARY_HIL_INCLUDE_DIR}"
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_compile_definitions(
    picolibrary
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_ENABLE_AUTOMATED_TESTING}>,PICOLIBRARY_ENABLE_AUTOMATED_TESTING,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING}>,PICOLIBRARY_ENABLE_INTERACTIVE_TESTING,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION}>,PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION}>,PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION}>,PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION,>"
)
target_link_libraries(
    picolibrary
    ${PICOLIBRARY_LINK_LIBRARIES}
)

add_library(
    picolibrary-fatal_error STATIC
    "picolibrary/fatal_error.cc"
)
target_include_directories(
    picolibrary-fatal_error
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(
    picolibrary-fatal_error
    picolibrary
)

add_library(
    picolibrary-version STATIC
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
)
target_include_directories(
    picolibrary-version
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(
    picolibrary-version
    picolibrary
)

if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_library(
        picolibrary-testing-automated-fatal_error STATIC
        "picolibrary/testing/automated/fatal_error.cc"
    )
    target_include_directories(
        picolibrary-testing-automated-fatal_error
        PUBLIC "${PROJECT_SOURCE_DIR}/include"
    )
    target_link_libraries(
        picolibrary-testing-automated-fatal_error
        picolibrary
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
