# I<sup>2</sup>C Facilities
I<sup>2</sup>C facilities are defined in the
[`include/picolibrary/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/i2c.h)/[`source/picolibrary/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/i2c.cc)
header/source file pair.

## Table of Contents
1. [Device Addressing](#device-addressing)
1. [Controller](#controller)
1. [Device](#device)

## Device Addressing
The `::picolibrary::I2C::Address_Numeric` class is used to store an I<sup>2</sup>C device
address in numeric (right justified) format.
- To get the minimum valid address, use the `::picolibrary::I2C::Address_Numeric::min()`
  static member function.
- To get the maximum valid address, use the `::picolibrary::I2C::Address_Numeric::max()`
  static member function.
- To get an address in its unsigned integer representation, use the
  `::picolibrary::I2C::Address_Numeric::as_unsigned_integer()` member function.
- Direct comparison is supported.

`::picolibrary::I2C::Address_Numeric` automated tests are defined in the
[`test/automated/picolibrary/i2c/address_numeric/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/address_numeric/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::I2C::Address_Numeric`
if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.

The `::picolibrary::I2C::Address_Transmitted` class is used to store an I<sup>2</sup>C
device address in transmitted (left shifted) format.
- To get the minimum valid address, use the
  `::picolibrary::I2C::Address_Transmitted::min()` static member function.
- To get the maximum valid address, use the
  `::picolibrary::I2C::Address_Transmitted::max()` static member function.
- To get an address in its unsigned integer representation, use the
  `::picolibrary::I2C::Address_Transmitted::as_unsigned_integer()` member function.
- Direct comparison is supported.

`::picolibrary::I2C::Address_Transmitted` automated tests are defined in the
[`test/automated/picolibrary/i2c/address_transmitted/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/address_transmitted/main.cc)
source file.

A `std::ostream` insertion operator is defined for
`::picolibrary::I2C::Address_Transmitted` if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.

`::picolibrary::Testing::Automated::random()` specializations for
`::picolibrary::I2C::Address_Numeric` and `::picolibrary::I2C::Address_Transmitted` are
available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The specializations are defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.

## Controller
picolibrary I<sup>2</sup>C controllers should consider the following to be fatal errors:
- Encountering a bus error while attempting to interact with a device
- Losing arbitration while attempting to interact with a device (multi-controller buses
  are not supported)

The `::picolibrary::I2C::Basic_Controller_Concept` concept class defines the expected
interface of an I<sup>2</sup>C basic controller.
- To initialize a basic controller's hardware, use a basic controller implementation's
  `initialize()` member function.
- To check if a bus error is present, use a basic controller implementation's
  `bus_error_present()` member function.
- To transmit a start condition, use a basic controller implementation's `start()` member
  function.
- To transmit a repeated start condition, use a basic controller implementation's
  `repeated_start()` member function.
- To transmit a stop condition, use a basic controller implementation's `stop()` member
  function.
- To address a device, use a basic controller implementation's `address()` member
  function.
- To read data from a device, use a basic controller implementation's `read()` member
  function.
- To write data to a device, use a basic controller implementation's `write()` member
  function.

The `::picolibrary::Testing::Automated::I2C::Mock_Basic_Controller` mock I<sup>2</sup>C
basic controller class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.

The `::picolibrary::I2C::Controller_Concept` concept class defines the expected interface
of an I<sup>2</sup>C controller.
- To initialize a controller's hardware, use a controller implementation's `initialize()`
  member function.
- To check if a bus error is present, use a controller implementation's
  `bus_error_present()` member function.
- To transmit a start condition, use a controller implementation's `start()` member
  function.
- To transmit a repeated start condition, use a controller implementation's
  `repeated_start()` member function.
- To transmit a stop condition, use a controller implementation's `stop()` member
  function.
- To address a device, use a controller implementation's `address()` member function.
- To read data from a device, use a controller implementation's `read()` member functions.
- To write data to a device, use a controller implementation's `write()` member functions.

To add I<sup>2</sup>C controller functionality to an I<sup>2</sup>C basic controller
implementation, use the `::picolibrary::I2C::Controller` template class.
`::picolibrary::I2C::Controller` automated tests are defined in the
[`test/automated/picolibrary/i2c/controller/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/controller/main.cc)
source file.

The `::picolibrary::Testing::Automated::I2C::Mock_Controller` mock I<sup>2</sup>C
controller class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::I2C::scan()` interactive test helper is
available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option is
`ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/i2c.h)/[`source/picolibrary/testing/interactive/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/i2c.cc)
header/source file pair.

To ensure that I<sup>2</sup>C bus interactions are properly terminated, use the
`::picolibrary::I2C::Bus_Control_Guard` RAII bus control guard template class to transmit
start and stop conditions.
`::picolibrary::I2C::Bus_Control_Guard` automated tests are defined in the
[`test/automated/picolibrary/i2c/bus_control_guard/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/bus_control_guard/main.cc)
source file.

To check if an I<sup>2</sup>C device is responsive, use the `::picolibrary::I2C::ping()`
functions.
`::picolibrary::I2C::ping()` automated tests are defined in the
[`test/automated/picolibrary/i2c/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/main.cc)
source file.

To scan an I<sup>2</sup>C bus, use the `::picolibrary::I2C::scan()` functions.
To report functor errors to the caller, use the
`::picolibrary::Functor_Can_Fail_Return_Functor` or
`::picolibrary::Functor_Can_Fail_Discard_Functor` policy.
`::picolibrary::I2C::scan()` automated tests are defined in the
[`test/automated/picolibrary/i2c/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/main.cc)
source file.
```c++
#include "picolibrary/algorithm.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"

void foo(
    ::picolibrary::I2C::Address_Numeric address,
    ::picolibrary::I2C::Operation operation,
    ::picolibrary::I2C::Response response ) noexcept;

auto bar(
    ::picolibrary::I2C::Address_Numeric address,
    ::picolibrary::I2C::Operation operation,
    ::picolibrary::I2C::Response response ) noexcept -> Result<void>;

template<typename Controller>
auto wibble( Controller & controller ) noexcept
{
    return ::picolibrary::I2C::scan( controller, foo );
}

template<typename Controller>
auto wobble( Controller & controller ) noexcept
{
    auto result = ::picolibrary::I2C::scan<::picolibrary::Functor_Can_Fail_Return_Functor>(
        controller,
        bar );
    if ( result.is_error() ) {
        // handle error
    } // if

    return result.value();
}

template<typename Controller>
void wubble( Controller & controller ) noexcept
{
    auto result = ::picolibrary::I2C::scan<::picolibrary::Functor_Can_Fail_Discard_Functor>(
        controller,
        bar );
    if ( result.is_error() ) {
        // handle error
    } // if
}
```

## Device
The `::picolibrary::I2C::Device_Address_Numeric` class is used to restrict
`::picolibrary::I2C::Address_Numeric` to the range of I<sup>2</sup>C device addresses used
by a specific type of I<sup>2</sup>C device.
- To get the minimum valid address, use the
  `::picolibrary::I2C::Device_Address_Numeric::min()` static member function.
- To get the maximum valid address, use the
  `::picolibrary::I2C::Device_Address_Numeric::max()` static member function.

The `::picolibrary::I2C::Device_Address_Transmitted` class is used to restrict
`::picolibrary::I2C::Address_Transmitted` to the range of I<sup>2</sup>C device addresses
used by a specific type of I<sup>2</sup>C device.
- To get the minimum valid address, use the
  `::picolibrary::I2C::Device_Address_Transmitted::min()` static member function.
- To get the maximum valid address, use the
  `::picolibrary::I2C::Device_Address_Transmitted::max()` static member function.

The `::picolibrary::I2C::Device` template class implements low level functionality for
interacting with an I<sup>2</sup>C device.
If an I<sup>2</sup>C bus does not have any multiplexers, use the
`::picolibrary::I2C::Bus_Multiplexer_Aligner` class as a device's bus multiplexer aligner.
- To get a device's address, use the `::picolibrary::I2C::Device::address()` member
  function.
- To get the fatal error that occurs if a device does not respond when addressed or does
  not acknowledge a write, use the
  `::picolibrary::I2C::Device::nonresponsive_device_error()` member function.
- To check if a device is responsive, use the `::picolibrary::I2C::Device::ping()` member
  functions.
- To align a bus's multiplexer(s) (if any) to enable communication with a device from a
  derived class, use the `::picolibrary::I2C::Device::align_bus_multiplexer()` member
  function.
- To get the controller used to communicate with a device from a derived class, use the
  `::picolibrary::I2C::Device::controller()` member function.
- To read a register or a block of registers from a derived class, use the
  `::picolibrary::I2C::Device::read()` member functions.
- To write to a register or a block of registers from a derived class, use the
  `::picolibrary::I2C::Device::write()` member functions.

`::picolibrary::I2C::Device` automated tests are defined in the
[`test/automated/picolibrary/i2c/device/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/i2c/device/main.cc)
source file.

The `::picolibrary::Testing::Automated::I2C::Mock_Device` mock I<sup>2</sup>C device class
is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/i2c.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/i2c.h)/[`source/picolibrary/testing/automated/i2c.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/i2c.cc)
header/source file pair.
