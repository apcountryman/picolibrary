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

# Description: picolibrary::I2C automated tests CMake rules.

# picolibrary::I2C::Address_Numeric automated tests
add_subdirectory( address_numeric )

# picolibrary::I2C::Address_Transmitted automated tests
add_subdirectory( address_transmitted )

# picolibrary::I2C::Bus_Control_Guard automated tests
add_subdirectory( bus_control_guard )

# picolibrary::I2C::Controller automated tests
add_subdirectory( controller )

# picolibrary::I2C::Device automated tests
add_subdirectory( device )

# picolibrary::I2C automated tests
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_executable(
        test-automated-picolibrary-i2c
        main.cc
    )
    target_link_libraries(
        test-automated-picolibrary-i2c
        picolibrary
        picolibrary-testing-automated-fatal_error
    )
    add_test(
        NAME    test-automated-picolibrary-i2c
        COMMAND test-automated-picolibrary-i2c --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
