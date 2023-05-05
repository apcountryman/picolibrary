# picolibrary
#
# Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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

# Description: picolibrary::CRC automated tests CMake rules.

# build the picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator automated
# tests
add_subdirectory( augmented_byte_indexed_lookup_table_calculator )

# build the picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator automated
# tests
add_subdirectory( augmented_nibble_indexed_lookup_table_calculator )

# build the picolibrary::CRC::Bitwise_Calculator automated tests
add_subdirectory( bitwise_calculator )

# build the picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator automated tests
add_subdirectory( direct_byte_indexed_lookup_table_calculator )

# build the picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator automated
# tests
add_subdirectory( direct_nibble_indexed_lookup_table_calculator )

# build the picolibrary::CRC automated tests
if( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
    add_executable(
        test-automated-picolibrary-crc
        main.cc
    )
    target_link_libraries(
        test-automated-picolibrary-crc
        picolibrary
        picolibrary-testing-automated-fatal_error
    )
    add_test(
        NAME    test-automated-picolibrary-crc
        COMMAND test-automated-picolibrary-crc --gtest_color=yes
    )
endif( ${PICOLIBRARY_ENABLE_AUTOMATED_TESTING} )
