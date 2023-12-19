# Peripheral Facilities
Peripheral facilities are defined in the
[`include/picolibrary/peripheral.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/peripheral.h)/[`source/picolibrary/peripheral.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/peripheral.cc)
header/source file pair.

## Table of Contents
1. [Peripheral Instance](#peripheral-instance)

## Peripheral Instance
The `::picolibrary::Peripheral::Instance` template class is used to define microcontroller
peripheral instances.
- To access a peripheral instance, use the
  `::picolibrary::Peripheral::Instance::instance()` static member function.
