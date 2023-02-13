# Asynchronous Serial Facilities
Asynchronous serial facilities are defined in the
[`include/picolibrary/asynchronous_serial.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/asynchronous_serial.h)/[`source/picolibrary/asynchronous_serial.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/asynchronous_serial.cc)
header/source file pair.

## Table of Contents
1. [Transmitter](#transmitter)
1. [Streams](#streams)
    1. [Reliable Output Streams](#reliable-output-streams)

## Transmitter
The `::picolibrary::Asynchronous_Serial::Basic_Transmitter_Concept` concept class defines
the expected interface of an asynchronous serial basic transmitter.
- To initialize a basic transmitter's hardware, use a basic transmitter implementation's
  `initialize()` member function.
- To transmit data, use a basic transmitter implementation's `transmit()` member function.

The `::picolibrary::Testing::Automated::Asynchronous_Serial::Mock_Basic_Transmitter` mock
asynchronous serial basic transmitter class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/asynchronous_serial.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/asynchronous_serial.h)/[`source/picolibrary/testing/automated/asynchronous_serial.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/asynchronous_serial.cc)
header/source file pair.

The `::picolibrary::Asynchronous_Serial::Transmitter_Concept` concept class defines the
expected interface of an asynchronous serial transmitter.
- To initialize a transmitter's hardware, use a transmitter implementation's
  `initialize()` member function.
- To transmit data, use a transmitter implementation's `transmit()` member functions.

To add asynchronous serial transmitter functionality to an asynchronous serial basic
transmitter implementation, use the `::picolibrary::Asynchronous_Serial::Transmitter`
template class.
`::picolibrary::Asynchronous_Serial::Transmitter` automated tests are defined in the
[`test/automated/picolibrary/asynchronous_serial/transmitter/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/asynchronous_serial/transmitter/main.cc)
source file.

The `::picolibrary::Testing::Automated::Asynchronous_Serial::Mock_Transmitter` mock
asynchronous serial transmitter class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/asynchronous_serial.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/asynchronous_serial.h)/[`source/picolibrary/testing/automated/asynchronous_serial.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/asynchronous_serial.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::Asynchronous_Serial::hello_world()` interactive
test helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project
configuration option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/asynchronous_serial.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/asynchronous_serial.h)/[`source/picolibrary/testing/interactive/asynchronous_serial.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/asynchronous_serial.cc)
header/source file pair.

## Streams

### Reliable Output Streams
The `::picolibrary::Asynchronous_Serial::Reliable_Output_Stream_Concept` concept class
defines the expected interface of an asynchronous serial reliable output stream.

picolibrary provides the following asynchronous serial reliable output stream
implementations:
- `::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream`

The `::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream` asynchronous
serial reliable output stream implementation (and associated
`::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer` I/O stream
device access buffer implementation) transmit data without buffering it.
`::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream` automated tests
are defined in the
[`test/automated/picolibrary/asynchronous_serial/reliable_unbuffered_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/asynchronous_serial/reliable_unbuffered_output_stream/main.cc)
source file.
`::picolibrary::Asynchronous_Serial::Reliable_Unbuffered_Output_Stream_Buffer` automated
tests are defined in the
[`test/automated/picolibrary/asynchronous_serial/reliable_unbuffered_output_stream_buffer/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/asynchronous_serial/reliable_unbuffered_output_stream_buffer/main.cc)
source file.

The `::picolibrary::Testing::Automated::Asynchronous_Serial::hello_world()` interactive
test helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project
configuration option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/asynchronous_serial.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/asynchronous_serial.h)/[`source/picolibrary/testing/interactive/asynchronous_serial.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/asynchronous_serial.cc)
header/source file pair.
