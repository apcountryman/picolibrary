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

# Description: picolibrary::SPI automated tests CMake rules.

# build the picolibrary::SPI::Controller automated tests
add_subdirectory( controller )

# build the picolibrary::SPI::Device automated tests
add_subdirectory( device )

# build the picolibrary::SPI::Device_Selection_Guard automated tests
add_subdirectory( device_selection_guard )

# build the picolibrary::SPI::GPIO_Output_Pin_Device_Selector automated tests
add_subdirectory( gpio_output_pin_device_selector )
