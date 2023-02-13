# Internet Protocol version 4 (IPv4) Facilities
IPv4 facilities are defined in the
[`include/picolibrary/ipv4.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/ipv4.h)/[`source/picolibrary/ipv4.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/ipv4.cc)
header/source file pair.

## Table of Contents
1. [Address](#address)

## Address
The `::picolibrary::IPv4::Address` class is used to store an IPv4 address.
- To get the minimum valid address, use the `::picolibrary::IPv4::Address::min()` static
  member function.
- To get the maximum valid address, use the `::picolibrary::IPv4::Address::max()` static
  member function.
- To get the address that is used to represent any address (0.0.0.0), use the
  `::picolibrary::IPv4::Address::any()` static member function.
- To get the typical loopback address (127.0.0.1), use the
  `::picolibrary::IPv4::Address::loopback()` static member function.
- To get the local network broadcast address, use the
  `::picolibrary::IPv4::Address::broadcast()` static member function.
- To check if an address is the address that is used to represent any address (0.0.0.0),
  use the `::picolibrary::IPv4::Address::is_any()` member function.
- To check if an address is a loopback address (127.0.0.0-127.255.255.255), use the
  `::picolibrary::IPv4::Address::is_loopback()` member function.
- To check if an address is the local network broadcast address (255.255.255.255), use the
  `::picolibrary::IPv4::Address::is_broadcast()` member function.
- To check if an address is a multicast address (224.0.0.0-239.255.255.255), use the
  `::picolibrary::IPv4::Address::is_multicast()` member function.
- To get an address in its byte array representation, use the
  `::picolibrary::IPv4::Address::as_byte_array()` member function.
- To get an address in its unsigned integer representation, use the
  `::picolibrary::IPv4::Address::as_unsigned_integer()` member function.
- Direct comparison is supported.

`::picolibrary::IPv4::Address` automated tests are defined in the
[`test/automated/picolibrary/ipv4/address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ipv4/address/main.cc)
source file.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::IPv4::Address`.
The `::picolibrary::Output_Formatter<::picolibrary::IPv4::Address>` specialization does
not support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::IPv4::Address>` automated tests are
defined in the
[`test/automated/picolibrary/ipv4/address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ipv4/address/main.cc)
source file.

A `::picolibrary::Testing::Automated::random()` specialization for
`::picolibrary::IPv4::Address` is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The specialization is defined in the
[`include/picolibrary/testing/automated/ipv4.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ipv4.h)/[`source/picolibrary/testing/automated/ipv4.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ipv4.cc)
header/source file pair.
