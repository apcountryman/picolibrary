# TCP over IP Facilities
WIZnet W5500 TCP over IP facilities are defined in the
[`include/picolibrary/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/wiznet/w5500/ip/tcp.cc)
header/source file pair.

## Table of Contents
1. [Client Socket](#client-socket)
1. [Acceptor Socket](#acceptor-socket)
1. [Server Connection Handler Socket](#server-connection-handler-socket)

## Client Socket
The `::picolibrary::WIZnet::W5500::IP::TCP::Client` template class implements a TCP over
IP client socket interface for interacting with a WIZnet W5500 socket.
See the [`::picolibrary::IP::TCP::Client_Concept`
documentation](../../../network/tcp_over_ip.md#client-socket) for more information.
- To get a socket's state, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::state()` member function.
- To get a socket's hardware socket ID, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::socket_id()` member function.
- To get a socket's socket interrupt mask (mask used when checking the network stack's
  socket interrupt context), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::socket_interrupt_mask()` member
  function.
- To configure a socket's no delayed ACK usage, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::configure_no_delayed_ack_usage()` member
  function.
- To get a socket's no delayed ACK usage configuration, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::no_delayed_ack_usage_configuration()`
  member function.
- To configure a socket's maximum segment size, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::configure_maximum_segment_size()` member
  function.
- To get a socket's maximum segment size, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::maximum_segment_size()` member function.
- To configure a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::configure_time_to_live()` member
  function.
- To get a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::time_to_live()` member function.
- To configure a socket's keepalive packet transmission period (SN_KPALVTR register
  value), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::configure_keepalive_period()` member
  function.
- To get a socket's keepalive packet transmission period (SN_KPALVTR register value), use
  the `::picolibrary::WIZnet::W5500::IP::TCP::Client::keepalive_period()` member function.
- To enable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::enable_interrupts()` member function.
- To disable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::disable_interrupts()` member functions.
- To get a mask identifying the interrupts that are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::enabled_interrupts()` member function.
- To get the interrupt context (SN_IR register value), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::interrupt_context()` member function.
- To clear interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::clear_interrupts()` member function.
- To check if data transmission is in progress, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::is_transmitting()` member function.
- To manually transmit a keepalive packet, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Client::transmit_keepalive()` member function.

`::picolibrary::WIZnet::W5500::IP::TCP::Client` automated tests are defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/tcp/client/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/tcp/client/main.cc)
source file.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP::Mock_Client` mock WIZnet
W5500 TCP over IP client socket class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::WIZnet::W5500::IP::TCP::echo_client()`
interactive test helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING`
project configuration option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.cc)
header/source file pair.

## Acceptor Socket
The `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor` template class implements a TCP over
IP acceptor socket interface for interacting with a WIZnet W5500 socket.
See the [`::picolibrary::IP::TCP::Acceptor_Concept`
documentation](../../../network/tcp_over_ip.md#acceptor-socket) for more information.
- To get a socket's state, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::state()` member function.
- To get a socket's hardware socket IDs, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::socket_ids()` member function.
- To get a socket's socket interrupt mask (mask used when checking the network stack's
  socket interrupt context), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::socket_interrupt_mask()` member
  function.
- To configure a socket's no delayed ACK usage, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_no_delayed_ack_usage()`
  member function.
- To get a socket's no delayed ACK usage configuration, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::no_delayed_ack_usage_configuration()`
  member function.
- To configure a socket's maximum segment size, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_maximum_segment_size()`
  member function.
- To get a socket's maximum segment size, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::maximum_segment_size()` member
  function.
- To configure a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_time_to_live()` member
  function.
- To get a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::time_to_live()` member function.
- To configure a socket's keepalive packet transmission period (SN_KPALVTR register
  value), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::configure_keepalive_period()` member
  function.
- To get a socket's keepalive packet transmission period (SN_KPALVTR register value), use
  the `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::keepalive_period()` member
  function.
- To enable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enable_interrupts()` member function.
- To disable interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::disable_interrupts()` member
  functions.
- To get a mask identifying the interrupts that are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::enabled_interrupts()` member function.
- To get the interrupt context (SN_IR register values ORed together), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::interrupt_context()` member function.
- To clear interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Acceptor::clear_interrupts()` member function.

`::picolibrary::WIZnet::W5500::IP::TCP::Acceptor` automated tests are defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/tcp/acceptor/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/tcp/acceptor/main.cc)
source file.

The `::picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP::Mock_Acceptor` mock WIZnet
W5500 TCP over IP acceptor socket class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::WIZnet::W5500::IP::TCP::echo_server()`
interactive test helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING`
project configuration option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/wiznet/w5500/ip/tcp.cc)
header/source file pair.

## Server Connection Handler Socket
The `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler` template class
implements a TCP over IP server connection handler socket interface for interacting with a
WIZnet W5500 socket.
See the [`::picolibrary::IP::TCP::Server_Connection_Handler_Concept`
documentation](../../../network/tcp_over_ip.md#server-connection-handler-socket) for more information.
- To get a socket's state, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::state()` member
  function.
- To get a socket's hardware socket ID, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_id()` member
  function.
- To get a socket's socket interrupt mask (mask used when checking the network stack's
  socket interrupt context), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::socket_interrupt_mask()`
  member function.
- To get a socket's no delayed ACK usage configuration, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::no_delayed_ack_usage_configuration()`
  member function.
- To get a socket's maximum segment size, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::maximum_segment_size()`
  member function.
- To get a socket's IPv4 packet time to live field value, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::time_to_live()`
  member function.
- To get a socket's keepalive packet transmission period (SN_KPALVTR register value), use
  the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::keepalive_period()`
  member function.
- To get a mask identifying the interrupts that are enabled, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::enabled_interrupts()`
  member function.
- To get the interrupt context (SN_IR register value), use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::interrupt_context()`
  member function.
- To clear interrupts, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::clear_interrupts()`
  member function.
- To check if data transmission is in progress, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::is_transmitting()`
  member function.
- To manually transmit a keepalive packet, use the
  `::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler::transmit_keepalive()`
  member function.

`::picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler` automated tests are
defined in the
[`test/automated/picolibrary/wiznet/w5500/ip/tcp/server_connection_handler/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/wiznet/w5500/ip/tcp/server_connection_handler/main.cc)
source file.

The
`::picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP::Mock_Server_Connection_Handler`
mock WIZnet W5500 TCP over IP server connection handler socket class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/wiznet/w5500/ip/tcp.h)/[`source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/wiznet/w5500/ip/tcp.cc)
header/source file pair.
