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

# Description: picolibrary::Microchip::MCP3008 automated tests CMake rules.

# picolibrary::Microchip::MCP3008::Blocking_Single_Sample_Converter automated tests
add_subdirectory( blocking_single_sample_converter )

# picolibrary::Microchip::MCP3008::Driver automated tests
add_subdirectory( driver )
