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

# Description: picolibrary::WIZnet::W5500::IP::TCP::Server automated tests CMake rules.

# picolibrary::WIZnet::W5500::IP::TCP::Server automated tests
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_executable(
        test-automated-picolibrary-wiznet-w5500-ip-tcp-server
        main.cc
    )
    target_link_libraries(
        test-automated-picolibrary-wiznet-w5500-ip-tcp-server
        picolibrary
        picolibrary-testing-automated-fatal_error
    )
    add_test(
        NAME    test-automated-picolibrary-wiznet-w5500-ip-tcp-server
        COMMAND test-automated-picolibrary-wiznet-w5500-ip-tcp-server --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
