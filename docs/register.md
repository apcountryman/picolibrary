# Register Facilities
Register facilities are defined in the
[`include/picolibrary/register.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/register.h)/[`source/picolibrary/register.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/register.cc)
header/source file pair.

## Table of Contents
1. [Register](#register)
1. [Reserved Register](#reserved-register)

## Register
The `::picolibrary::Register` class is used to declare a microcontroller register that is
not reserved.
Register access is restricted to the following operations:
- Conversion to the underlying register type (reading the register)
- Assignment
- Bitwise AND assignment
- Bitwise OR assignment
- Bitwise XOR assignment

## Reserved Register
The `::picolibrary::Reserved_Register` class is used to declare a microcontroller reserved
register.
Reserved registers cannot be accessed.
