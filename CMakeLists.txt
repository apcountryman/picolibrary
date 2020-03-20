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

# File: CMakeLists.txt
# Description: picolibrary core CMake rules.

cmake_minimum_required( VERSION 3.12.4 )
project(
    picolibrary
    LANGUAGES CXX
)

# load additional CMake modules
list(
    APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/apcountryman-cmake-utilities"
)
include( git-utilities )

# capture project version information
execute_git_command(
    PICOLIBRARY_VERSION
    COMMAND describe --always --dirty --broken
)

# configure Doxygen
find_package( Doxygen )
if( DEFINED Doxygen_FOUND )
    set(
        PICOLIBRARY_DOCUMENTATION_EXCLUDE_PATTERNS
        " \
        */.git/* \
        */README.md \
        */build/* \
        */cmake/modules/* \
        "
    )

    set( DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/documentation/Doxyfile" )
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/documentation/Doxyfile.in"
        "${DOXYFILE}"
        @ONLY
    )
    add_custom_target(
        picolibrary-documentation
        ALL
        COMMAND           "${DOXYGEN_EXECUTABLE}" "${DOXYFILE}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/documentation"
    )
endif( DEFINED Doxygen_FOUND )
