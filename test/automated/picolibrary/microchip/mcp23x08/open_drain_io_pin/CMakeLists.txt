# picolibrary
#
# Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# Description: picolibrary::Microchip::MCP23X08::Open_Drain_IO_Pin automated tests CMake
#       rules.

# picolibrary::Microchip::MCP23X08::Open_Drain_IO_Pin automated tests
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_executable(
        test-automated-picolibrary-microchip-mcp23x08-open_drain_io_pin
        main.cc
    )
    target_link_libraries(
        test-automated-picolibrary-microchip-mcp23x08-open_drain_io_pin
        picolibrary
        picolibrary-testing-automated-fatal_error
    )
    add_test(
        NAME    test-automated-picolibrary-microchip-mcp23x08-open_drain_io_pin
        COMMAND test-automated-picolibrary-microchip-mcp23x08-open_drain_io_pin --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
