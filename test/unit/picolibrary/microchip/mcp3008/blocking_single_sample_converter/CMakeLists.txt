# picolibrary
#
# Copyright 2021 Andrew Countryman <apcountryman@gmail.com>
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

# File: test/unit/picolibrary/microchip/mcp3008/blocking_single_sample_converter/CMakeLists.txt
# Description: picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter unit
#       tests CMake rules.

# build the picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter unit tests
if( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    add_executable(
        test-unit-picolibrary-microchip-mcp3008-blocking_single_sample_converter
        main.cc
    )
    target_link_libraries(
        test-unit-picolibrary-microchip-mcp3008-blocking_single_sample_converter
        picolibrary
    )
    add_test(
        NAME    test-unit-picolibrary-microchip-mcp3008-blocking_single_sample_converter
        COMMAND test-unit-picolibrary-microchip-mcp3008-blocking_single_sample_converter --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
