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

# Description: picolibrary::Microchip::MCP23X08 automated tests CMake rules.

# picolibrary::Microchip::MCP23X08::Caching_Driver automated tests
add_subdirectory( caching_driver )

# picolibrary::Microchip::MCP23X08::Internally_Pulled_Up_Input_Pin automated tests
add_subdirectory( internally_pulled_up_input_pin )

# picolibrary::Microchip::MCP23X08::Open_Drain_IO_Pin automated tests
add_subdirectory( open_drain_io_pin )

# picolibrary::Microchip::MCP23X08::Pin automated tests
add_subdirectory( pin )

# picolibrary::Microchip::MCP23X08::Push_Pull_IO_Pin automated tests
add_subdirectory( push_pull_io_pin )
