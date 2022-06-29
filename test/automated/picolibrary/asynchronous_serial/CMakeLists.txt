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

# Description: picolibrary::Asynchronous_Serial automated tests CMake rules.

# build the picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream automated
# tests
add_subdirectory( reliable_unbuffered_output_stream )

# build the picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer
# automated tests
add_subdirectory( reliable_unbuffered_output_stream_buffer )

# build the picolibrary::Asynchronous_Serial::Transmitter automated tests
add_subdirectory( transmitter )

# build the picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream automated tests
add_subdirectory( unbuffered_output_stream )

# build the picolibrary::Asynchronous_Serial::Unbuffered_Output_Stream_Buffer automated
# tests
add_subdirectory( unbuffered_output_stream_buffer )
