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

# Description: picolibrary automated testing configuration.

# install prefix is not applicable
mark_as_advanced( CMAKE_INSTALL_PREFIX )

# build configuration
set( CMAKE_BUILD_TYPE                            "Debug" CACHE STRING "" FORCE )
set( CMAKE_EXPORT_COMPILE_COMMANDS               ON      CACHE BOOL   "" FORCE )
set( PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS  OFF     CACHE BOOL   "" FORCE )
set( PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS OFF     CACHE BOOL   "" FORCE )
mark_as_advanced(
    CMAKE_BUILD_TYPE
    CMAKE_EXPORT_COMPILE_COMMANDS
    PICOLIBRARY_USE_PARENT_PROJECT_BUILD_FLAGS
    PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS
)

# human readable error information suppression configuration
set( PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION OFF CACHE BOOL "" FORCE )

# human readable event information suppression configuration
set( PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION OFF CACHE BOOL "" FORCE )

# automated testing configuration
set( PICOLIBRARY_ENABLE_AUTOMATED_TESTING       ON  CACHE BOOL "" FORCE )
set( PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST OFF CACHE BOOL "" FORCE )
mark_as_advanced(
    PICOLIBRARY_ENABLE_AUTOMATED_TESTING
    PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST
)

# interactive testing configuration
set( PICOLIBRARY_ENABLE_INTERACTIVE_TESTING OFF CACHE BOOL "" FORCE )
mark_as_advanced( PICOLIBRARY_ENABLE_INTERACTIVE_TESTING )
