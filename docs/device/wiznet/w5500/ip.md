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
WIZnet W5500 IP protocol port allocator is available if the
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
- To get the fatal error that occurs if an operation fails due to a W5500 being
  nonresponsive, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()` member
  function.
- To check if a W5500 is responsive, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive()` member
  function.
- To configure the PHY, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy()` member function.
- To get the PHY mode, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode()` member function.
- To get the link status, us the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_status()` member function.
- To get the link mode, us the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode()` member function.
- To get the link speed, us the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed()` member function.
- To configure ping blocking, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking()` member
  function.
- To get the ping blocking configuration, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()` member
  function.
- To configure ARP forcing, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing()` member
  function.
- To get the ARP forcing configuration, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration()` member
  function.
- To configure retransmission (RTR and RCR register values), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_retransmission()` member
  function.
- To get the retry time (RTR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::retry_time()` member function.
- To get the retry count (RCR register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::retry_count()` member function.
- To configure the socket buffer size, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()` member
  function.
- To get the socket buffer size, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::socket_buffer_size()` member function.
- To get the number of sockets a network stack is configured to support, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::sockets()` member function.
- To configure a W5500's MAC address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_mac_address()` member
  function.
- To get a W5500's MAC address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::mac_address()` member function.
- To configure a W5500's IPv4 address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_address()` member
  function.
- To get a W5500's IPv4 address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_address()` member function.
- To configure a W5500's IPv4 gateway address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_gateway_address()`
  member function.
- To get a W5500's IPv4 gateway address, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_gateway_address()` member
  function.
- To configure a W5500's IPv4 subnet mask, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_subnet_mask()` member
  function.
- To get a W5500's IPv4 subnet mask, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_subnet_mask()` member function.
- To configure a W5500's interrupt assert wait time (INTLEVEL register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::configure_interrupt_assert_wait_time()`
  member function.
- To get a W5500's interrupt assert wait time (INTLEVEL register value), use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_assert_wait_time() member
  function.
- To enable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::enable_interrupts()` member function.
  Interrupt masks are defined in the `::picolibrary::WIZnet::W5500::Interrupt` structure.
  The `::picolibrary::WIZnet::W5500::Interrupt` structure is defined in the
  [`include/picolibrary/wiznet/w5500.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500.h)/[`source/picolibrary/wiznet/w5500.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500.cc)
  header/source file pair.
- To disable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts()` member
  functions.
  Interrupt masks are defined in the `::picolibrary::WIZnet::W5500::Interrupt` structure.
  The `::picolibrary::WIZnet::W5500::Interrupt` structure is defined in the
  [`include/picolibrary/wiznet/w5500.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500.h)/[`source/picolibrary/wiznet/w5500.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500.cc)
  header/source file pair.
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
- To get the IPv4 address that has been reported to be unreachable, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_ipv4_address()` member
  function.
- To get the port that has been reported to be unreachable, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_port()` member function.
- To get the number of sockets that are available for allocation, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::sockets_available_for_allocation()`
  member function.
- To allocate a socket, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::allocate_socket()` member functions.
- To deallocate a socket, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::deallocate_socket()` member function.
- To allocate sockets, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::allocate_sockets()` member functions.
- To deallocate sockets, use the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::deallocate_sockets()` member function.
- To access the TCP over IP port allocator, us the
  `::picolibrary::WIZnet::W5500::IP::Network_Stack::tcp_port_allocator()` member function.

`::picolibrary::WIZnet::W5500::IP::Network_Stack` automated tests are defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/network_stack/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/network_stack/main.cc)
source file.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Network_Stack` mock WIZnet
W5500 IP network stack is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/network_stack.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::WIZnet::W5500::IP::ping()` interactive test
helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration
option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/wiznet/w5500/ip/network_stack.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/wiznet/w5500/ip/network_stack.h)/[`source/picolibrary/testing/interactive/wiznet/w5500/ip/network_stack.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/wiznet/w5500/ip/network_stack.cc)
header/source file pair.
