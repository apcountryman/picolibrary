# Cyclic Redundancy Check (CRC) Facilities
CRC facilities are defined in the
[`include/picolibrary/crc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/crc.h)/[`source/picolibrary/crc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/crc.cc)
header/source file pair.

## Table of Contents
1. [Calculators](#calculators)
1. [Calculator Utilities](#calculator-utilities)

## Calculators
The `::picolibrary::CRC::Calculation_Parameters` struct captures a CRC calculation's
parameters.

The `::picolibrary::CRC::Calculator_Concept` defines the expected interface of a CRC
calculator implementation.
- To calculate the remainder for a message, use a CRC calculator implementation's
  `calculate()` member function.

picolibrary provides the following CRC calculator implementations:
- `::picolibrary::CRC::Bitwise_Calculator`
- `::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator`
- `::picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator`
- `::picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator`
- `::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator`

An automated test that verifies that all provide CRC calculator implementations produce
the same result is defined in the
[`test/automated/picolibrary/crc/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/main.cc)
source file.

The `::picolibrary::CRC::Bitwise_Calculator` CRC calculator implementation processes
messages one bit at a time, and requires a message augment to push the entirety of a
message through the calculation.
While this results in lower performance than table driven implementations, memory use is
lower due to the lack of a lookup table.
`::picolibrary::CRC::Bitwise_Calculator` automated tests are defined in the
[`test/automated/picolibrary/crc/bitwise_calculator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/bitwise_calculator/main.cc)
source file.

The `::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator` CRC calculator
implementation processes messages one byte at a time, and requires a message augment to
push the entirety of a message through the calculation.
While this results in higher memory use than bitwise implementations and table driven
implementations that process messages one nibble at a time, performance is higher due to
the message processing loop requiring fewer iterations to process a message.
The `::picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator` CRC calculator
implementation is strictly superior to the
`::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator` implementation since
it does not have to process a message augment in addition to the message itself.
`::picolibrary::CRC::Augmented_Byte_Indexed_Lookup_Table_Calculator` automated tests are
defined in the
[`test/automated/picolibrary/crc/augmented_byte_indexed_lookup_table_calculator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/augmented_byte_indexed_lookup_table_calculator/main.cc)
source file.

The `::picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator` CRC calculator
implementation processes messages one byte at a time, and does not require a message
augment to push the entirety of a message through the calculation.
While this results in higher memory use than bitwise implementations and table driven
implementations that process messages one nibble at a time, performance is higher due to
the message processing loop requiring fewer iterations to process a message.
`::picolibrary::CRC::Direct_Byte_Indexed_Lookup_Table_Calculator` automated tests are
defined in the
[`test/automated/picolibrary/crc/direct_byte_indexed_lookup_table_calculator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/direct_byte_indexed_lookup_table_calculator/main.cc)
source file.

The `::picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator` CRC calculator
implementation processes messages one nibble at a time, and requires a message augment to
push the entirety of a message through the calculation.
This achieves a balance between implementations that are optimized for performance (table
driven implementations that process messages one or more bytes at a time), and those that
are optimized for memory use (bitwise implementations).
The `::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator` CRC calculator
implementation is strictly superior to the
`::picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator` implementation
since it does not have to process a message augment in addition to the message itself.
`::picolibrary::CRC::Augmented_Nibble_Indexed_Lookup_Table_Calculator` automated tests are
defined in the
[`test/automated/picolibrary/crc/augmented_nibble_indexed_lookup_table_calculator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/augmented_nibble_indexed_lookup_table_calculator/main.cc)
source file.

The `::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator` CRC calculator
implementation processes messages one nibble at a time, and does not require a message
augment to push the entirety of a message through the calculation.
This achieves a balance between implementations that are optimized for performance (table
driven implementations that process messages one or more bytes at a time), and those that
are optimized for memory use (bitwise implementations).
`::picolibrary::CRC::Direct_Nibble_Indexed_Lookup_Table_Calculator` automated tests are
defined in the
[`test/automated/picolibrary/crc/direct_nibble_indexed_lookup_table_calculator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/crc/direct_nibble_indexed_lookup_table_calculator/main.cc)
source file.

Automated test fixtures for CRC calculator implementations (`::uint8Register`,
`::uint16Register`, and `uint32Register`) are available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration is `ON`.
The automated test fixtures are defined in the
[`include/picolibrary/testing/automated/crc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/crc.h)/[`source/picolibrary/testing/automated/crc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/crc.cc)
header/source file pair.

## Calculator Utilities
To get the input processor for a CRC calculation, use the
`::picolibrary::CRC::input_processor()` function.

To get the output processor for a CRC calculation, use the
`::picolibrary::CRC::output_processor()` function.

To get a message augment array for a CRC calculation, use the
`::picolibrary::CRC::Message_Augment` type alias.

To generate a byte indexed lookup table for a CRC calculation, use the
`::picolibrary::generate_byte_indexed_lookup_table()` function.

To get the number of bits in a nibble, use the `::picolibrary::CRC::NIBBLE_DIGITS`
constant.

To get the largest value a nibble can hold, use the `::picolibrary::CRC::NIBBLE_MAX`
constant.

To get the pair of nibbles that make up a byte, use the
`::picolibrary::get_byte_nibbles()` function.

To generate a nibble indexed lookup table for a CRC calculation, use the
`::picolibrary::generate_nibble_indexed_lookup_table()` function.
