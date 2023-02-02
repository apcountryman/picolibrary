# Input/Output (I/O) Facilities

## Device Error Handling
Two different styles of I/O streams (normal and reliable) are supported that are
differentiated by how device errors are handled.
Normal I/O streams (`::picolibrary::Stream_Buffer`, `::picolibrary::Stream`,
`::picolibrary::Output_Stream`, and derived classes) enter an error state when a device
error occurs and report the device error to the caller.
Reliable I/O streams (`::picolibrary::Reliable_Stream_Buffer`,
`::picolibrary::Reliable_Stream`, `::picolibrary::Reliable_Output_Stream`, and derived
classes) cannot experience device errors, handle device errors internally, or call
`::picolibrary::trap_fatal_error()` (via a precondition or postcondition check) if a
device error occurs.

## I/O Stream Device Access Buffer
The `::picolibrary::Stream_Buffer` and `::picolibrary::Reliable_Stream_Buffer` I/O stream
device access buffer abstract classes are defined in the
[`include/picolibrary/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/stream.h)/[`source/picolibrary/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/stream.cc)
header/source file pair.
These classes define the standard interface that streams use to interact with devices.

`::picolibrary::Stream_Buffer` automated tests are defined in the
[`test/automated/picolibrary/stream_buffer/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/stream_buffer/main.cc)
source file.
`::picolibrary::Reliable_Stream_Buffer` automated tests are defined in the
[`test/automated/picolibrary/reliable_stream_buffer/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_stream_buffer/main.cc)
source file.

The `::picolibrary::Testing::Automated::Mock_Stream_Buffer` and
`::picolibrary::Testing::Automated::Mock_Reliable_Stream_Buffer` mock I/O stream device
access buffer classes are available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mocks are defined in the
[`include/picolibrary/testing/automated/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/stream.h)/[`source/picolibrary/testing/automated/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/stream.cc)
header/source file pair.

## I/O Stream Core
The `::picolibrary::Stream` and `::picolibrary::Reliable_Stream` I/O stream core classes
are defined in the
[`include/picolibrary/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/stream.h)/[`source/picolibrary/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/stream.cc)
header/source file pair.
These classes store an I/O stream's state information and associate an I/O stream with an
I/O stream device access buffer.
An I/O stream's state includes the following:
- End-of-file reached
- I/O error present
- Fatal error present (normal I/O streams only)

`::picolibrary::Stream` and `::picolibrary::Reliable_Stream` support the following
operations:
- To check if a stream is nominal (no errors present and end-of-file has not been
  reached), use the `::picolibrary::Stream::is_nominal()` and
  `::picolibrary::Reliable_Stream::is_nominal()` member functions.
- To check if errors are present (I/O error and/or fatal error present), use the
  `::picolibrary::Stream::error_present()` and
  `::picolibrary::Reliable_Stream::error_present()` member functions.
- To check if end-of-file has been reached, use the
  `::picolibrary::Stream::end_of_file_reached()` and
  `::picolibrary::Reliable_Stream::end_of_file_reached()` member functions.
- To report that end-of-file has been reached from a derived class, use the
  `::picolibrary::Stream::report_end_of_file_reached()` and
  `::picolibrary::Reliable_Stream::report_end_of_file_reached()` member functions.
- To clear an end-of-file reached report from a derived class, use the
  `::picolibrary::Stream::clear_end_of_file_reached_report()` and
  `::picolibrary::Reliable_Stream::clear_end_of_file_reached_report()` member functions.
- To check if an I/O error is present, use the `::picolibrary::Stream::io_error_present()`
  and `::picolibrary::Reliable_Stream::io_error_present()` member functions.
- To report an I/O error, use the `::picolibrary::Stream::report_io_error()` and
  `::picolibrary::Reliable_Stream::report_io_error()` member functions.
- To clear an I/O error, use the `::picolibrary::Stream::clear_io_error()` and
  `::picolibrary::Reliable_Stream::clear_io_error()` member functions.
- To check if a fatal error is present, use the
  `::picolibrary::Stream::fatal_error_present()` member function.
- To report a fatal error from a derived class, use the
  `::picolibrary::Stream::report_fatal_error()` member function.
- To clear a fatal error from a derived class, use the
  `::picolibrary::Stream::clear_fatal_error()` member function.
- To check if a stream is associated with an I/O stream device access buffer, use the
  `::picolibrary::Stream::buffer_is_set()` and
  `::picolibrary::Reliable_Stream::buffer_is_set()` member functions.
- To associate an I/O stream with an I/O stream device access buffer from a derived class,
  use the `::picolibrary::Stream::set_buffer()` and
  `::picolibrary::Reliable_Stream::set_buffer()` member functions.
- To get the I/O stream device access buffer associated with an I/O stream from a derived
  class, use the `::picolibrary::Stream::buffer()` and
  `::picolibrary::Reliable_Stream::buffer()` member functions.

`::picolibrary::Stream` automated tests are defined in the
[`test/automated/picolibrary/stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/stream/main.cc)
source file.
`::picolibrary::Reliable_Stream` automated tests are defined in the
[`test/automated/picolibrary/reliable_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_stream/main.cc)
source file.

## Output Stream
The `::picolibrary::Output_Stream` and `::picolibrary::Reliable_Output_Stream` output
stream classes are defined in the
[`include/picolibrary/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/stream.h)/[`source/picolibrary/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/stream.cc)
header/source file pair.
These classes define stream unformatted and formatted output operations.
The stream being nominal is a precondition for all output operations.

`::picolibrary::Output_Stream` and `::picolibrary::Reliable_Output_Stream` support the
following operations:
- To write any output that has been buffered to the device associated with a stream, use
  the `::picolibrary::Output_Stream::flush()` and
  `::picolibrary::Reliable_Output_Stream::flush()` member functions.
- To write unformatted data to a stream, use the `::picolibrary::Output_Stream::put()` and
  `::picolibrary::Reliable_Output_Stream::put()` member functions.
- To write formatted data to a stream, use the `::picolibrary::Output_Stream::print()` and
  `::picolibrary::Reliable_Output_Stream::print()` member functions. Formatting is
  controlled by the `::picolibrary::Output_Formatter` preceding each argument or a default
  constructed `::picolibrary::Output_Formatter` if one isn't provided.
```c++
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream, char const * string ) noexcept
{
    // default constructed ::picolibrary::Output_Formatter<char const *> used
    stream.print( string );

    // provided ::picolibrary::Output_Formatter<char const *> used
    stream.print( ::picolibrary::Output_Formatter<char const *>{}, string );
}
```

`::picolibrary::Output_Stream` automated tests are defined in the
[`test/automated/picolibrary/output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/output_stream/main.cc)
source file.
`::picolibrary::Reliable_Output_Stream` automated tests are defined in the
[`test/automated/picolibrary/reliable_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_output_stream/main.cc)
source file.

The `::picolibrary::Testing::Automated::Mock_Output_Stream` and
`::picolibrary::Testing::Automated::Mock_Reliable_Output_Stream` mock output stream
classes are available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration
option is `ON`.
The mocks are defined in the
[`include/picolibrary/testing/automated/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/stream.h)/[`source/picolibrary/testing/automated/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/stream.cc)
header/source file pair.
- To access a stream's mock I/O stream device access buffer, use the
  `::picolibrary::Testing::Automated::Mock_Output_Stream::buffer()` and
  `::picolibrary::Testing::Automated::Mock_Reliable_Output_Stream::buffer()` member
  functions.

The `::picolibrary::Testing::Automated::Output_String_Stream` and
`::picolibrary::Testing::Automated::Reliable_Output_String_Stream` string stream classes
are available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option
is `ON`.
The string streams are define in the
[`include/picolibrary/testing/automated/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/stream.h)/[`source/picolibrary/testing/automated/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/stream.cc)
header/source file pair.
- To access the `std::string` abstracted by the stream, use the
  `::picolibrary::Testing::Automated::Output_String_Stream::string()` and
  `::picolibrary::Testing::Automated::Reliable_Output_String_Stream::string()` member
  functions.

The `::picolibrary::Testing::Interactive::hello_world()` output stream interactive test
helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration
option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/stream.h)/[`source/picolibrary/testing/interactive/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/stream.cc)
header/source file pair.

### Output Formatters
The `::picolibrary::Output_Formatter` template class is defined in the
[`include/picolibrary/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/stream.h)/[`source/picolibrary/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/stream.cc)
header/source file pair.
This template class must be fully or partially specialized for each type that will support
formatted output.
Specialization constructor parameters can be used to let the user configure formatting.

The following `::picolibrary::Output_Formatter` specializations are defined in the
[`include/picolibrary/stream.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/stream.h)/[`source/picolibrary/stream.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/stream.cc)
header/source file pair:
- `::picolibrary::Output_Formatter<char>`
- `::picolibrary::Output_Formatter<char const *>`
- `::picolibrary::Output_Formatter<::picolibrary::ROM::String>`
- `::picolibrary::Output_Formatter<::picolibrary::Error_Code>`
- `::picolibrary::Output_Formatter<Enum,
  std::enable_if_t<::picolibrary::is_error_code_enum_v<Enum>>>`

The `::picolibrary::Output_Formatter<char>` specialization does not support user
formatting configuration.
`::picolibrary::Output_Formatter<char>` automated tests are defined in the
[`test/automated/picolibrary/output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/output_stream/main.cc)
and
[`test/automated/picolibrary/reliable_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_output_stream/main.cc)
source files.

The `::picolibrary::Output_Formatter<char const *>` specialization does not support user
formatting configuration.
`::picolibrary::Output_Formatter<char const *>` automated tests are defined in the
[`test/automated/picolibrary/output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/output_stream/main.cc)
and
[`test/automated/picolibrary/reliable_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_output_stream/main.cc)
source files.

The `::picolibrary::Output_Formatter<::picolibrary::ROM::String>` specialization does not
support user formatting configuration.

The `::picolibrary::Output_Formatter<::picolibrary::Error_Code>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Error_Code>` automated tests are defined
in the
[`test/automated/picolibrary/output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/output_stream/main.cc)
and
[`test/automated/picolibrary/reliable_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_output_stream/main.cc)
source files.
```c++
#include "picolibrary/error.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    // output will be "::picolibrary::Generic_Error::LOGIC_ERROR"
    stream.print( ::picolibrary::Error_Code{ ::picolibrary::Generic_Error::LOGIC_ERROR } );
}
```

The `::picolibrary::Output_Formatter<Enum,
std::enable_if_t<::picolibrary::is_error_code_enum_v<Enum>>>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<Enum,
std::enable_if_t<::picolibrary::is_error_code_enum_v<Enum>>>` automated tests are defined
in the
[`test/automated/picolibrary/output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/output_stream/main.cc)
and
[`test/automated/picolibrary/reliable_output_stream/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/reliable_output_stream/main.cc)
source files.
```c++
#include "picolibrary/error.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    // output will be "::picolibrary::Generic_Error::LOGIC_ERROR"
    stream.print( ::picolibrary::Generic_Error::LOGIC_ERROR );
}
```

The following formatting helper types and `::picolibrary::Output_Formatter`
specializations are defined in the
[`include/picolibrary/format.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/format.h)/[`source/picolibrary/format.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/format.cc)
header/source file pair:
- `::picolibrary::Format::Bin`
- `::picolibrary::Output_Formatter<::picolibrary::Format::Bin<Integer>>`
- `::picolibrary::Format::Dec`
- `::picolibrary::Output_Formatter<::picolibrary::Format::Dec<Integer>,
  std::enable_if_t<std::is_signed_v<Integer>>>`
- `::picolibrary::Output_Formatter<::picolibrary::Format::Dec<Integer>,
  std::enable_if_t<std::is_unsigned_v<Integer>>>`
- `::picolibrary::Format::Hex`
- `::picolibrary::Output_Formatter<::picolibrary::Format::Hex<Integer>>`
- `::picolibrary::Format::Hex_Dump`
- `::picolibrary::Output_Formatter<::picolibrary::Format::Hex_Dump<Iterator>>`

The `::picolibrary::Format::Bin` class is used to print an integer type in binary.
The `::picolibrary::Output_Formatter<::picolibrary::Format::Bin<Integer>>` specialization
does not support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Format::Bin<Integer>>` automated tests are
defined in the
[`test/automated/picolibrary/format/bin/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/format/bin/main.cc)
source file.
```c++
#include <cstdint>

#include "picolibrary/format.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    // output will be "0b01010101"
    stream.print( ::picolibrary::Format::Bin{ std::uint8_t{ 0b01010101 } } );
}
```

The `::picolibrary::Format::Dec` class is used to print an integer type in decimal.
The `::picolibrary::Output_Formatter<::picolibrary::Format::Dec<Integer>,
std::enable_if_t<std::is_signed_v<Integer>>>` and
`::picolibrary::Output_Formatter<::picolibrary::Format::Dec<Integer>,
std::enable_if_t<std::is_unsigned_v<Integer>>>` specializations do not support user
formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Format::Dec<Integer,
std::enable_if_t<std::is_signed_v<Integer>>>` and `::picolibrary::Format::Dec<Integer,
std::enable_if_t<std::is_unsigned_v<Integer>>>` automated tests are defined in the
[`test/automated/picolibrary/format/dec/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/format/dec/main.cc)
source file.
```c++
#include <cstdint>

#include "picolibrary/format.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    // output will be "42"
    stream.print( ::picolibrary::Format::Dec{ std::int8_t{ 42 } } );

    // output will be "-42"
    stream.print( ::picolibrary::Format::Dec{ std::int8_t{ -42 } } );

    // output will be 42
    stream.print( ::picolibrary::Format::Dec{ std::uint8_t{ 42 } } );
}
```

The `::picolibrary::Format::Hex` class is used to print an integer type in hexadecimal.
The `::picolibrary::Output_Formatter<::picolibrary::Format::Hex<Integer>>` specialization
does not support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Format::Hex<Integer>>` automated tests are
defined in the
[`test/automated/picolibrary/format/hex/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/format/hex/main.cc)
source file.
```c++
#include <cstdint>

#include "picolibrary/format.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    // output will be "0x0A"
    stream.print( ::picolibrary::Format::Bin{ std::uint8_t{ 0x0A } } );
}
```

The `::picolibrary::Format::Hex_Dump` class is used to print a hex dump of data.
The `::picolibrary::Output_Formatter<::picolibrary::Format::Hex_Dump<Iterator>>`
specialization does not support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Format::Hex_Dump<Iterator>>` automated
tests are defined in the
[`test/automated/picolibrary/format/hex_dump/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/format/hex_dump/main.cc)
source file.
```c++
#include <string>

#include "picolibrary/format.h"
#include "picolibrary/stream.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    auto const data = std::string{ "{yZZk7V!/{>fm[lxV!$e|:" };

    // output will be
    // "0000000000000000  7B 79 5A 5A 6B 37 56 21 2F 7B 3E 66 6D 5B 6C 78  |{yZZk7V!/{>fm[lx|\n"
    // "0000000000000010  56 21 24 65 7C 3A                                |V!$e|:|          \n"
    // assuming std::size_t is 64-bit
    stream.print( ::picolibrary::Format::Hex_Dump{ data.begin(), data.end() } );
}
