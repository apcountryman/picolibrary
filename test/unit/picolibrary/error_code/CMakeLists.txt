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

# File: test/unit/picolibrary/error_code/CMakeLists.txt
# Description: picolibrary::Error_Code unit tests CMake rules.

# build the picolibrary::Error_Code unit tests
if( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )
    add_executable(
        test-unit-picolibrary-error_code
        main.cc
    )
    target_link_libraries(
        test-unit-picolibrary-error_code
        picolibrary
    )
    add_test(
        NAME    test-unit-picolibrary-error_code
        COMMAND test-unit-picolibrary-error_code --gtest_color=yes
    )
endif( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )
