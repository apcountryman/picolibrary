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

# File: test/unit/picolibrary/gpio/active_low_input_pin/CMakeLists.txt
# Description: picolibrary::GPIO::Active_Low_Input_Pin unit tests CMake rules.

# build the picolibrary::GPIO::Active_Low_Input_Pin unit tests
if( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    add_executable(
        test-unit-picolibrary-gpio-active_low_input_pin
        main.cc
    )
    target_link_libraries(
        test-unit-picolibrary-gpio-active_low_input_pin
        picolibrary
        picolibrary-fatal_error
    )
    add_test(
        NAME    test-unit-picolibrary-gpio-active_low_input_pin
        COMMAND test-unit-picolibrary-gpio-active_low_input_pin --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
