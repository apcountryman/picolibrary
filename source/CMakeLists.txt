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

# File: source/CMakeLists.txt
# Description: picolibrary CMake rules.

set(
    PICOLIBRARY_SOURCE_FILES
    picolibrary.cc
)
set( PICOLIBRARY_LINK_LIBRARIES )

add_library(
    picolibrary STATIC
    ${PICOLIBRARY_SOURCE_FILES}
)
target_include_directories(
    picolibrary
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(
    picolibrary
    ${PICOLIBRARY_LINK_LIBRARIES}
)
