# UDP over IP Facilities
WIZnet W5500 UDP over IP facilities are defined in the
[`include/picolibrary/wiznet/w5500/ip/udp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500/ip/udp.h)/[`source/picolibrary/wiznet/w5500/ip/udp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500/ip/udp.cc)
header/source file pair.

## Table of Contents
1. [Socket](#socket)

## Socket
The `::picolibrary::WIZnet::W5500::IP::UDP::Socket` template class implements a UDP over
IP socket interface for interacting with a WIZnet W5500 socket.
See the [`::picolibrary::IP::UDP::Socket_Concept`
documentation](../../../network/udp_over_ip.md#socket) for more information.
- To get a socket's state, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::state()` member function.
- To get a socket's hardware socket ID, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_id()` member function.
- To get a socket's socket interrupt mask (mask used when checking the network stack's
  socket interrupt context), use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::socket_interrupt_mask()` member
  function.
- To configure socket broadcast blocking, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::configure_broadcast_blocking()` member function.
- To get the socket broadcast blocking configuration, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::broadcast_blocking_configuration()` member
  function.
- To configure socket unicast blocking, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::configure_unicast_blocking()` member function.
- To get the socket unicast blocking configuration, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::unicast_blocking_configuration()` member
  function.
- To configure a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::configure_time_to_live()` member
  function.
- To get a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::time_to_live()` member function.
- To enable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::enable_interrupts()` member function.
- To disable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::disable_interrupts()` member functions.
- To get a mask identifying the interrupts that are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::enabled_interrupts()` member function.
- To get the interrupt context (SN_IR register value), use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::interrupt_context()` member function.
- To clear interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::clear_interrupts()` member function.
- To check if data transmission is in progress, use the
  `::picolibrary::WIZnet::W5500::IP::UDP::Socket::is_transmitting()` member function.

`::picolibrary::WIZnet::W5500::IP::UDP::Socket` automated tests are defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/udp/socket/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/udp/socket/main.cc)
source file.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::UDP::Mock_Socket` mock WIZnet
W5500 UDP over IP socket class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/udp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/udp.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/udp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/udp.cc)
header/source file pair.
