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

# File: test/unit/picolibrary/microchip/mcp23s08/CMakeLists.txt
# Description: picolibrary::Microchip::MCP23S08 unit tests CMake rules.

# build the picolibrary::Microchip::MCP23S08::Address_Numeric unit tests
add_subdirectory( address_numeric )

# build the picolibrary::Microchip::MCP23S08::Address_Transmitted unit tests
add_subdirectory( address_transmitted )

# build the picolibrary::Microchip::MCP23S08::Communication_Controller unit tests
add_subdirectory( communication_controller )
