# Bit Manipulation Utilities
Bit manipulation utilities are defined in the
[`include/picolibrary/bit_manipulation.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/bit_manipulation.h)/[`source/picolibrary/bit_manipulation.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/bit_manipulation.cc)
header/source file pair.
Bit manipulation utility automated tests are defined in the
[`test/automated/picolibrary/bit_manipulation/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/bit_manipulation/main.cc)
source file.

## `highest_bit_set()`
To lookup an unsigned integer's highest bit set, use the
`::picolibrary::highest_bit_set()` utility.

## `mask()`
To create a bit mask, use the `::picolibrary::mask()` utility.

## `reflect()`
To reflect the bits in an unsigned integer, use the `::picolibrary::reflect()` utility.

## `is_power_of_two()`
To check if an unsigned integer is a power of two, use the
`::picolibrary::is_power_of_two()` utility.
