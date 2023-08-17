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

# Description: picolibrary::Reliable_Output_Stream automated tests CMake rules.

# picolibrary::Reliable_Output_Stream automated tests
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_executable(
        test-automated-picolibrary-reliable_output_stream
        main.cc
    )
    target_link_libraries(
        test-automated-picolibrary-reliable_output_stream
        picolibrary
        picolibrary-testing-automated-fatal_error
    )
    add_test(
        NAME    test-automated-picolibrary-reliable_output_stream
        COMMAND test-automated-picolibrary-reliable_output_stream --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
