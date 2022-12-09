# picolibrary
#
# Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# Description: picolibrary core CMake rules.

cmake_minimum_required( VERSION 3.12.4 )
project(
    picolibrary
    LANGUAGES CXX
)

# general project configuration
option(
    PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    "picolibrary: enable automated testing"
    OFF
)
option(
    PICOLIBRARY_ENABLE_INTERACTIVE_TESTING
    "picolibrary: enable interactive testing"
    OFF
)
option(
    PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
    "picolibrary: suppress assertion failure location information"
    OFF
)
option(
    PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION
    "picolibrary: suppress human readable error information"
    OFF
)
option(
    PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
    "picolibrary: suppress human readable event information"
    OFF
)

set(
    PICOLIBRARY_HIL_INCLUDE_DIR
    "" CACHE STRING
    "picolibrary: HIL include directory"
)

if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION} )
    message( FATAL_ERROR "picolibrary automated tests require human readable error information" )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION} )

if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION} )
    message( FATAL_EVENT "picolibrary automated tests require human readable event information" )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} AND ${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION} )

# load additional CMake modules
list(
    APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/apcountryman-cmake-utilities"
)

# configure compilation and linking
option(
    PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS
    "picolibrary: use parent project's build flags"
    ON
)
if( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS} )
    set( CMAKE_CXX_STANDARD 17 )
    add_compile_options(
        -Werror -Wall -Wextra
        -Wcast-qual
        -Wfloat-equal
        -Wmissing-field-initializers
        -Wmissing-include-dirs
        -Wold-style-cast
        -Wpointer-arith
        -Wshadow
        -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=picolibrary
    )

    option(
        PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS
        "picolibrary: use static analysis build flags"
        OFF
    )
    if( NOT ${PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS} )
        add_compile_options(
            -Wcast-align=strict
            -Wduplicated-cond
            -Wextra-semi
            -Wimplicit-fallthrough=5
            -Wlogical-op
            -Wplacement-new=2
            -Wunsafe-loop-optimizations
        )
    endif( NOT ${PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS} )

    if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
        add_compile_options(
            -fsanitize=address
            -fno-sanitize-recover=all
        )
        add_link_options(
            -fsanitize=address
            -fno-sanitize-recover=all
        )
    endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
endif( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS} )

# capture project version information
include( git-utilities )
execute_git_command(
    PICOLIBRARY_VERSION
    COMMAND describe --match=none --always --dirty --broken
)

# configure Google Test
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    option(
        PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST
        "picolibrary: use parent project's Google Test"
        ON
    )
    if( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST} )
        set( BUILD_GMOCK   ON  CACHE INTERNAL "" )
        set( INSTALL_GTEST OFF CACHE INTERNAL "" )

        set( gtest_build_samples         OFF CACHE INTERNAL "" )
        set( gtest_build_tests           OFF CACHE INTERNAL "" )
        set( gtest_disable_pthreads      OFF CACHE INTERNAL "" )
        set( gtest_force_shared_crt      OFF CACHE INTERNAL "" )
        set( gtest_hide_internal_symbols OFF CACHE INTERNAL "" )

        set( gmock_build_tests OFF CACHE INTERNAL "" )

        add_subdirectory( googletest )
    endif( NOT ${PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST} )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )

# enable automated testing
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    enable_testing()
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )

# build the library
add_subdirectory( source )

# build the library's tests
add_subdirectory( test )
