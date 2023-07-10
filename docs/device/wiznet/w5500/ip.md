# IP Facilities
WIZnet W5500 IP facilities are defined in the
[`include/picolibrary/wiznet/w5500/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500/ip.h)/[`source/picolibrary/wiznet/w5500/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500/ip.cc)
header/source file pair.

## Table of Contents
1. [Port Allocator](#port-allocator)

## Port Allocator
The `::picolibrary::WIZnet::W5500::IP::Port_Allocator_Concept` concept class defines the
expected interface of a WIZnet W5500 IP protocol port allocator.
- To allocate a port, use a port allocator implementation's `allocate()` member function.
- To deallocate a previously allocated port, use a port allocator implementation's
  `deallocate()` member function.

The `::picolibrary::WIZnet::W5500::IP::Unsupported_Protocol_Port_Allocator` WIZnet W5500
IP protocol port allocator is used to catch attempts to allocate ports for protocols that
are not used.

The
`::picolibrary::WIZnet::W5500::IP::TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation`
WIZnet W5500 IP protocol port allocator is used to allocate TCP or UDP over IP ports.
Ephemeral port allocation is not supported.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator` mock
WIZnet W5500 IP protocol port allocator class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip.cc)
header/source file pair.
