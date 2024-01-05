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

# Description: picolibrary release configuration.

# install prefix is not applicable
mark_as_advanced( CMAKE_INSTALL_PREFIX )

# build configuration
set( CMAKE_BUILD_TYPE                            "Release" CACHE INTERNAL "" )
set( CMAKE_EXPORT_COMPILE_COMMANDS               ON        CACHE INTERNAL "" )
set( PICOLIBRARY_HIL_INCLUDE_DIR                 ""        CACHE INTERNAL "" )
set( PICOLIBRARY_USE_STATIC_ANALYSIS_BUILD_FLAGS OFF       CACHE INTERNAL "" )

# assertion failure location information suppression configuration
set( PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION OFF CACHE INTERNAL "" )

# human readable error information suppression configuration
set( PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION OFF CACHE INTERNAL "" )

# human readable event information suppression configuration
set( PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION OFF CACHE INTERNAL "" )

# automated testing configuration
set( PICOLIBRARY_ENABLE_AUTOMATED_TESTING OFF CACHE INTERNAL "" )

# interactive testing configuration
set( PICOLIBRARY_ENABLE_INTERACTIVE_TESTING OFF CACHE INTERNAL "" )
