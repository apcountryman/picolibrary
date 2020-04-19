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

# project configuration
option( PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION "picolibrary: suppress human readable error information" OFF )
option( PICOLIBRARY_TESTING_INTERACTIVE_ENABLED               "picolibrary: enable interactive testing"                OFF )
option( PICOLIBRARY_TESTING_UNIT_ENABLED                      "picolibrary: enable unit testing"                       OFF )
option( PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS            "picolibrary: use parent project's build flags"          ON  )
option( PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST            "picolibrary: use parent project's Google Test"          ON  )

if( ${PICOLIBRARY_TESTING_UNIT_ENABLED} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION} )
    message( FATAL_ERROR "picolibrary unit tests require human readable error information" )
endif( ${PICOLIBRARY_TESTING_UNIT_ENABLED} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION} )

# load additional CMake modules
list(
    APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/apcountryman-cmake-utilities"
)

# configure compilation and linking
if( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS} )
    set(
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -std=c++17 \
        -Werror -Wall -Wextra \
        -Wcast-qual \
        -Wfloat-equal \
        -Wmissing-field-initializers \
        -Wmissing-include-dirs \
        -Wold-style-cast \
        -Wpointer-arith \
        -Wshadow \
        "
    )

    if( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )
        set(
            CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} \
            -fsanitize=address \
            -fno-sanitize-recover=all \
            "
        )
    endif( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )
endif( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS} )

# capture project version information
include( git-utilities )
execute_git_command(
    PICOLIBRARY_VERSION
    COMMAND describe --always --dirty --broken
)

# configure Google Test
if( ${PICOLIBRARY_TESTING_UNIT_ENABLED} AND NOT ${PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST} )
    set( BUILD_GMOCK   ON  CACHE BOOL "" FORCE )
    set( INSTALL_GTEST OFF CACHE BOOL "" FORCE )
    mark_as_advanced(
        BUILD_GMOCK
        INSTALL_GTEST
    )

    set( gtest_build_samples         OFF CACHE BOOL "" FORCE )
    set( gtest_build_tests           OFF CACHE BOOL "" FORCE )
    set( gtest_disable_pthreads      OFF CACHE BOOL "" FORCE )
    set( gtest_force_shared_crt      OFF CACHE BOOL "" FORCE )
    set( gtest_hide_internal_symbols OFF CACHE BOOL "" FORCE )
    mark_as_advanced(
        gtest_build_samples
        gtest_build_tests
        gtest_disable_pthreads
        gtest_force_shared_crt
        gtest_hide_internal_symbols
    )

    set( gmock_build_tests OFF CACHE BOOL "" FORCE )
    mark_as_advanced( gmock_build_tests )

    add_subdirectory( googletest )
endif( ${PICOLIBRARY_TESTING_UNIT_ENABLED} AND NOT ${PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST} )

# configure Doxygen
find_package( Doxygen )
if( ${DOXYGEN_FOUND} )
    set(
        PICOLIBRARY_DOCUMENTATION_EXCLUDE_PATTERNS
        " \
        */.git/* \
        */README.md \
        */build/* \
        */cmake/modules/* \
        */googletest/* \
        */lizard/* \
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
endif( ${DOXYGEN_FOUND} )

# enable unit testing
if( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )
    enable_testing()
endif( ${PICOLIBRARY_TESTING_UNIT_ENABLED} )

# build the library
add_subdirectory( source )

# build the library's tests
add_subdirectory( test )
