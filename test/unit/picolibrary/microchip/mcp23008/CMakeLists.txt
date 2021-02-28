# picolibrary
#
# Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# File: test/unit/picolibrary/microchip/mcp23008/CMakeLists.txt
# Description: picolibrary::Microchip::MCP23008 unit tests CMake rules.

# build the picolibrary::Microchip::MCP23008::Driver unit tests
add_subdirectory( driver )

# build the picolibrary::Microchip::MCP23008::Internally_Pulled_Up_Input_Pin unit tests
add_subdirectory( internally_pulled_up_input_pin )

# build the picolibrary::Microchip::MCP23008::Register_Cache unit tests
add_subdirectory( register_cache )
