# UDP Over IP Facilities
UDP over IP facilities are defined in the
[`include/picolibrary/ip/udp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/ip/udp.h)/[`source/picolibrary/ip/udp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/ip/udp.cc)
header/source file pair.

## Table of Contents
1. [Port](#port)
1. [Endpoint](#endpoint)
1. [Socket](#socket)

## Port
The `::picolibrary::IP::UDP::Port` type alias is used to store a UDP port.
See the [`::picolibrary::IP::Port` documentation](ip.md#port) for more information.

## Endpoint
The `::picolibrary::IP::UDP::Endpoint` type alias is used to store a UDP endpoint (IP
address, UDP port pair).
See the [`::picolibrary::IP::Endpoint` documentation](ip.md#endpoint) for more
information.

## Reception Result
The `::picolibrary::IP::UDP::Reception_Result` struct is used to store a UDP reception
result.

## Socket
The `::picolibrary::IP::UDP::Socket_Concept` concept class defines the expected interface
of a UDP over IP socket.
- To bind a socket to a local endpoint, use a socket implementation's `bind()` member
  function.
- To get a socket's local endpoint, use a socket implementation's `local_endpoint()`
  member function.
- To transmit a datagram to a remote endpoint, use a socket implementation's `transmit()`
  member function.
- To receive a datagram from a remote endpoint, use a socket implementation's `receive()`
  member function.
- To close a socket, use a socket implementation's `close()` member function.

The `::picolibrary::Testing::Automated::IP::UDP::Mock_Socket` mock UDP over IP socket
class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration
option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/ip/udp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip/udp.h)/[`source/picolibrary/testing/automated/ip/udp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip/udp.cc)
header/source file pair.
