# IP Facilities
IP facilities are defined in the
[`include/picolibrary/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/ip.h)/[`source/picolibrary/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/ip.cc)
header/source file pair.

## Table of Contents
1. [Address](#address)
1. [Port](#port)
1. [Endpoint](#endpoint)
1. [Network Stack](#network-stack)

## Address
The `::picolibrary::IP::Address` class is used to store an IP address.
- To get the version of a stored address, use the `::picolibrary::IP::Address::version()`
  member function.
- To check if the version of a stored address is unspecified, use the
  `::picolibrary::IP::Address::is_unspecified()` member function.
- To check if a stored address is an IPv4 address, use the
  `::picolibrary::IP::Address::is_ipv4()` member function.
- To check if a stored address is an address that is used to represent any address, use
  the `::picolibrary::IP::Address::is_any()` member function.
- To check if a stored address is a loopback address, use the
  `::picolibrary::IP::Address::is_loopback()` member function.
- To check if a stored address is a multicast address, use the
  `::picolibrary::IP::Address::is_multicast()` member function.
- To get a stored IPv4 address, use the `::picolibrary::IP::Address::ipv4()` member
  function.
- Direct comparison is supported.

`::picolibrary::IP::Address` automated tests are defined in the
[`test/automated/picolibrary/ip/address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/address/main.cc)
source file.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::IP::Address`.
The `::picolibrary::Output_Formatter<::picolibrary::IP::Address>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::IP::Address>` automated tests are defined
in the
[`test/automated/picolibrary/ip/address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/address/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::IP::Address` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip.h)/[`source/picolibrary/testing/automated/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip.cc)
header/source file pair.

## Port
The `::picolibrary::IP::Port` class is used to store a protocol port.
- To get the minimum valid port, use the `::picolibrary::IP::Port::min()` static member
  function.
- To get the maximum valid port, use the `::picolibrary::IP::Port::max()` static member
  function.
- To get the port that is used to represent any port (0), use the
  `::picolibrary::IP::Port::any()` static member function.
- To check if a port is the port that is used to represent any port (0), use the
  `::picolibrary::IP::Port::is_any()` member function.
- To get a port in its unsigned integer representation, use the
  `::picolibrary::IP::Port::as_unsigned_integer()` member function.
- Direct comparison is supported.

`::picolibrary::IP::Port` automated tests are defined in the
[`test/automated/picolibrary/ip/port/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/port/main.cc)
source file.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::IP::Port`.
The `::picolibrary::Output_Formatter<::picolibrary::IP::Port>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::IP::Port>` automated tests are defined in
the
[`test/automated/picolibrary/ip/port/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/port/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::IP::Port` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip.h)/[`source/picolibrary/testing/automated/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip.cc)
header/source file pair.

## Endpoint
The `::picolibrary::IP::Endpoint` class is used to store a protocol endpoint (IP address,
protocol port pair).
- To get an endpoint's address, use the `::picolibrary::IP::Endpoint::address()` member
  function.
- To get an endpoint's port, use the `::picolibrary::IP::Endpoint::port()` member
  function.
- Direct comparison is supported.

`::picolibrary::IP::Endpoint` automated tests are defined in the
[`test/automated/picolibrary/ip/endpoint/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/endpoint/main.cc)
source file.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::IP::Endpoint`.
The `::picolibrary::Output_Formatter<::picolibrary::IP::Endpoint>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::IP::Endpoint>` automated tests are defined
in the
[`test/automated/picolibrary/ip/endpoint/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/ip/endpoint/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::IP::Endpoint` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip.h)/[`source/picolibrary/testing/automated/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip.cc)
header/source file pair.

## Network Stack
The `::picolibrary::IP::Network_Stack_Concept` concept class defines the expected
interface of an IP network stack.
The `::picolibrary::IP::Network_Stack_Concept` concept class is defined in the
[`include/picolibrary/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/ip/network_stack.h)/[`source/picolibrary/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/ip/network_stack.cc)
header/source file pair.
- To construct a TCP client socket, use a network stack implementation's
  `make_tcp_client()` member function.
- To construct a TCP server socket, use a network stack implementation's
  `make_tcp_server()` member function.

The `::picolibrary::Testing::Automated::IP::Mock_Network_Stack` mock IP network stack
class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration
option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip/network_stack.h)/[`source/picolibrary/testing/automated/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip/network_stack.cc)
header/source file pair.
