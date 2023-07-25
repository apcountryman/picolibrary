# IP Facilities
WIZnet W5500 IP facilities are defined in the
[`include/picolibrary/wiznet/w5500/ip.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500/ip.h)/[`source/picolibrary/wiznet/w5500/ip.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500/ip.cc)
header/source file pair.

## Table of Contents
1. [Port Allocator](#port-allocator)
1. [Network Stack](#network-stack)

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

## Network Stack
The `::picolibrary::WIZnet::W5500::IP::Network_Stack` template class implements an IP
network stack interface for interacting with a WIZnet W5500.
The `::picolibrary::WIZnet::W5500::IP::Network_Stack` template class is defined in the
[`include/picolibrary/wiznet/w5500/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500/ip/network_stack.h)/[`source/picolibrary/wiznet/w5500/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500/ip/network_stack.cc)
header/source file pair.
See the [`::picolibrary::IP::Network_Stack_Concept`
docuentation](../../../network/ip.md#network-stack) for more information.
- To initialize the network stack, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::initialize()` member function.
- To get the fatal error that occurs if an operation fails due to the W5500 being
  nonresponsive, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()` member
  function.
- To check if the W5500 is responsive, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive()` member
  function.
- To get the PHY mode, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode()` member function.
- To get the link status, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_status()` member function.
- To get the link mode, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode()` member function.
- To get the link speed, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed()` member function.
- To configure ping blocking, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking()` member
  function.
- To get the ping blocking configuration, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()` member
  function.
- To get the ARP forcing configuration, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration()` member
  function.
- To get the retransmission retry time (RTR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::retransmission_retry_time()` member
  function.
- To get the retransmission retry count (RCR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::retransmission_retry_count()` member
  function.
- To get the MAC address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::mac_address()` member function.
- To get the IPv4 address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_address()` member function.
- To get the IPv4 gateway address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_gateway_address()` member
  function.
- To get the IPv4 subnet mask, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_subnet_mask()` member function.
- To get the interrupt assert wait time (INTLEVEL register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_assert_wait_time()` member
  function.
- To enable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::enable_interrupts()` member function.
- To disable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts()` member
  functions.
- To get a mask identifying the interrupts that are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::enabled_interrupts()` member function.
- To get the interrupt context (IR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_context()` member function.
- To clear interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::clear_interrupts()` member function.
- To enable socket interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::enable_socket_interrupts()` member
  function.
- To disable socket interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::disable_socket_interrupts()` member
  function.
- To check if socket interrupts are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()`
  member function.
- To get the socket interrupt context (SIR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupt_context()` member
  function.
- To get the endpoint that has been reported to be unreachable, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_endpoint()` member
  function.
- To get the socket buffer size, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::socket_buffer_size()` member function.
- To get the number of sockets the network stack is configured to support, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::sockets()` member function.
- To get the number of sockets that are available for allocation, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::sockets_available_for_allocation()`
  member function.

`::picolibrary::WIZnet::W5500::IP::Network_Stack` automated tests are defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/network_stack/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/network_stack/main.cc)
source file.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack` mock WIZnet
W5500 IP network stack class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.cc)
header/source file pair.
