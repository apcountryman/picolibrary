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

# Description: picolibrary::IP automated tests CMake rules.

# picolibrary::IP::Address automated tests
add_subdirectory( address )

# picolibrary::IP::Endpoint automated tests
add_subdirectory( endpoint )

# picolibrary::IP::Port automated tests
add_subdirectory( port )
