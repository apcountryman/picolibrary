# TCP Over IP Facilities
TCP over IP facilities are defined in the
[`include/picolibrary/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/ip/tcp.h)/[`source/picolibrary/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/ip/tcp.cc)
header/source file pair.

## Table of Contents
1. [Port](#port)
1. [Endpoint](#endpoint)
1. [Client Socket](#client-socket)
1. [Acceptor Socket](#acceptor-socket)
1. [Server Connection Handler Socket](#server-connection-handler-socket)

## Port
The `::picolibrary::IP::TCP::Port` type alias is used to store a TCP port.
See the [`::picolibrary::IP::Port` documentation](ip.md#port) for more information.

## Endpoint
The `::picolibrary::IP::TCP::Endpoint` type alias is used to store a TCP endpoint (IP
address, TCP port pair).
See the [`::picolibrary::IP::Endpoint` documentation](ip.md#endpoint) for more
information.

## Client Socket
The `::picolibrary::IP::TCP::Client_Concept` concept class defines the expected interface
of a TCP over IP client socket.
- To bind a client socket to a local endpoint, use a client socket implementation's
  `bind()` member function.
- To connect a client socket to a remote endpoint, use a client socket implementation's
  `connect()` member function.
- To check if a client socket is connected to a remote endpoint, use a client socket
  implementation's `is_connected()` member function.
- To get a connection's remote endpoint, use a client socket implementation's
  `remote_endpoint()` member function.
- To get a connection's local endpoint, use a client socket implementation's
  `local_endpoint()` member function.
- To get the amount of data that has yet to be transmitted to a remote endpoint, use a
  client socket implementation's `outstanding()` member function.
- To transmit data to a remote endpoint, use a client socket implementation's `transmit()`
  member function.
- To get the amount of data that is immediately available to be received from a remote
  endpoint, use a client socket implementation's `available()` member function.
- To receive data from a remote endpoint, use a client socket implementation's `receive()`
  member function.
- To disable further data transmission and reception, use a client socket implementation's
  `shutdown()` member function.
- To close a client socket, use a client socket implementation's `close()` member
  function.

The `::picolibrary::Testing::Automated::IP::TCP::Mock_Client` mock TCP over IP client
socket class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip/tcp.h)/[`source/picolibrary/testing/automated/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip/tcp.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::IP::TCP::echo_client()` interactive test helper
is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option
is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/ip/tcp.h)/[`source/picolibrary/testing/interactive/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/ip/tcp.cc)
header/source file pair.

## Acceptor Socket
The `::picolibrary::IP::TCP::Acceptor_Concept` concept class defines the expected
interface of a TCP over IP acceptor socket.
- To bind an acceptor socket to a local endpoint, use an acceptor socket implementation's
  `bind()` member function.
- To listen for incoming connection requests, use an acceptor socket implementation's
  `listen()` member function.
- To check if an acceptor socket is listening for incoming connection requests, use an
  acceptor socket implementation's `is_listening()` member function.
- To get the local endpoint on which an acceptor socket is listening for incoming
  connection requests, use an acceptor socket implementation's `local_endpoint()` member
  function.
- To accept an incoming connection request, use an acceptor socket implementation's
  `accept()` member function.
- To close an acceptor socket, use an acceptor socket implementation's `close()` member
  function.

The `::picolibrary::Testing::Automated::IP::TCP::Mock_Acceptor` mock TCP over IP acceptor
socket class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip/tcp.h)/[`source/picolibrary/testing/automated/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip/tcp.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::IP::TCP::echo_server()` interactive test helper
is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option
is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/ip/tcp.h)/[`source/picolibrary/testing/interactive/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/ip/tcp.cc)
header/source file pair.

## Server Connection Handler Socket
The `::picolibrary::IP::TCP::Server_Connection_Handler_Concept` concept class defines the
expected interface of a TCP over IP server connection handler socket.
- To check if a server connection handler socket is connected to a remote endpoint, use a
  server connection handler socket implementation's `is_connected()` member function.
- To get a connection's remote endpoint, use a server connection handler socket
  implementation's `remote_endpoint()` member function.
- To get a connection's local endpoint, use a server connection handler socket
  implementation's `local_endpoint()` member function.
- To get the amount of data that has yet to be transmitted to a remote endpoint, use a
  server connection handler socket implementation's `outstanding()` member function.
- To transmit data to a remote endpoint, use a server connection handler socket
  implementation's `transmit()` member function.
- To get the amount of data that is immediately available to be received from a remote
  endpoint, use a server connection handler socket implementation's `available()` member
  function.
- To receive data from a remote endpoint, use a server connection handler socket
  implementation's `receive()` member function.
- To disable further data transmission and reception, use a server connection handler
  socket implementation's `shutdown()` member function.
- To close a server connection handler socket, use a server connection handler socket
  implementation's `close()` member function.

The `::picolibrary::Testing::Automated::IP::TCP::Mock_Server_connection_Handler` mock TCP
over IP server connection handler socket class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/ip/tcp.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/ip/tcp.h)/[`source/picolibrary/testing/automated/ip/tcp.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/ip/tcp.cc)
header/source file pair.
