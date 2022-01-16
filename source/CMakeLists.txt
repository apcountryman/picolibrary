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

# File: source/CMakeLists.txt
# Description: picolibrary CMake rules.

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/picolibrary/version.cc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
    @ONLY
)

set(
    PICOLIBRARY_SOURCE_FILES
    "picolibrary.cc"
    "picolibrary/error.cc"
    "picolibrary/precondition.cc"
    "picolibrary/result.cc"
    "picolibrary/void.cc"
)
set( PICOLIBRARY_LINK_LIBRARIES )

if( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} OR ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing.cc"
    )
endif( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} OR ${PICOLIBRARY_ENABLE_UNIT_TESTING} )

if( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/interactive.cc"
    )
endif( ${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING} )

if( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    list(
        APPEND PICOLIBRARY_SOURCE_FILES
        "picolibrary/testing/unit.cc"
        "picolibrary/testing/unit/error.cc"
        "picolibrary/testing/unit/mock_handle.cc"
        "picolibrary/testing/unit/random.cc"
    )
    list(
        APPEND PICOLIBRARY_LINK_LIBRARIES
        gmock
        gtest
    )
endif( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )

add_library(
    picolibrary STATIC
    ${PICOLIBRARY_SOURCE_FILES}
)
target_include_directories(
    picolibrary
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_compile_definitions(
    picolibrary
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_ENABLE_INTERACTIVE_TESTING}>,PICOLIBRARY_ENABLE_INTERACTIVE_TESTING,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_ENABLE_UNIT_TESTING}>,PICOLIBRARY_ENABLE_UNIT_TESTING,>"
    PUBLIC "$<IF:$<BOOL:${PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION}>,PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION,>"
)
target_link_libraries(
    picolibrary
    ${PICOLIBRARY_LINK_LIBRARIES}
)

add_library(
    picolibrary-fatal_error STATIC
    "picolibrary/fatal_error.cc"
)
target_include_directories(
    picolibrary-fatal_error
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

add_library(
    picolibrary-version STATIC
    "${CMAKE_CURRENT_BINARY_DIR}/picolibrary/version.cc"
)
target_include_directories(
    picolibrary-version
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

if( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
    add_library(
        picolibrary-testing-unit-fatal_error STATIC
        "picolibrary/testing/unit/fatal_error.cc"
    )
    target_include_directories(
        picolibrary-testing-unit-fatal_error
        PUBLIC "${PROJECT_SOURCE_DIR}/include"
    )
endif( ${PICOLIBRARY_ENABLE_UNIT_TESTING} )
