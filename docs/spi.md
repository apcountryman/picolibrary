# Serial Peripheral Interface (SPI) Facilities
SPI facilities are defined in the
[`include/picolibrary/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/spi.h)/[`source/picolibrary/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/spi.cc)
header/source file pair.

## Controller
The `::picolibrary::SPI::Basic_Controller_Concept` concept class defines the expected
interface of an SPI basic controller.
To initialize a basic controller's hardware, use a basic controller implementation's
`initialize()` member function.
To configure a basic controller's clock and data exchange bit order to meet a specific
device's communication requirements, use a basic controller implementation's `configure()`
member function.
To exchange data with a device, use a basic controller implementation's `exchange()`
member function.

The `::picolibrary::Testing::Automated::SPI::Mock_Basic_Controller` mock SPI basic
controller class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/spi.h)/[`source/picolibrary/testing/automated/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/spi.cc)
header/source file pair.

The `::picolibrary::SPI::Controller_Concept` concept class defines the expected interface
of an SPI controller.
To initialize a controller's hardware, use a controller implementation's `initialize()`
member function.
To configure a controller's clock and data exchange bit order to meet a specific device's
communication requirements, use a controller implementation's `configure()` member
function.
To exchange data with a device, use a controller implementation's `exchange()` member
functions.
To receive data from a device, use a controller implementation's `receive()` member
functions.
To transmit data to a device, use a controller implementation's `transmit()` member
functions.

To add SPI controller functionality to an SPI basic controller implementation, use the
`::picolibrary::SPI::Controller` template class.
`::picolibrary::SPI::Controller` automated tests are defined in the
[`test/automated/picolibrary/spi/controller/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/spi/controller/main.cc)
source file.

The `::picolibrary::Testing::Automated::SPI::Mock_Controller` mock SPI controller class is
available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/spi.h)/[`source/picolibrary/testing/automated/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/spi.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::SPI::echo()` interactive test helper is
available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option is
`ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/spi.h)/[`source/picolibrary/testing/interactive/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/spi.cc)
header/source file pair.

## Device Selection
The `::picolibrary::SPI::Device_Selector_Concept` concept class defines the expected
interface of an SPI device selector.
To initialize a device selector's hardware, use a device selector implementation's
`initialize()` member function.
To select a device, use a device selector implementation's `select()` member function.
To deselect a device, use a device selector implementation's `deselect()` member function.

The `::picolibrary::SPI::GPIO_Output_Pin_Device_Selector` template class is a GPIO output
pin based SPI device selector implementation.
`::picolibrary::SPI::GPIO_Output_Pin_Device_Selector` automated tests are defined in the
[`test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc)
source file.

The `::picolibrary::Testing::Automated::SPI::Mock_Device_Selector` mock SPI device
selector class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/spi.h)/[`source/picolibrary/testing/automated/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/spi.cc)
header/source file pair.

To ensure that SPI devices are deselected, use the
`::picolibrary::SPI::Device_Selection_Guard` RAII device selection guard template class to
select and deselect devices.
`::picolibrary::SPI::Device_Selection_Guard` automated tests are defined in the
[`test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc)
source file.

## Device
The `::picolibrary::SPI::Device` template class implements low level functionality for
interacting with an SPI device.
To initialize a device's device selection hardware from a derived class, use the
`::picolibrary::SPI::Device::initialize()` member function.
To configure the controller's clock and data exchange bit order to meet a device's
communication requirements from a derived class, use the
`::picolibrary::SPI::Device::configure()` member function.
To access the device selector used to select and deselect a device from a derived class,
use the `::picolibrary::SPI::Device::device_selector()` member function.
To exchange data with a device from a derived class, use the
`::picolibrary::SPI::Device::exchange()` member functions.
To receive data from a device from a derived class, use the
`::picolibrary::SPI::Device::receive()` member functions.
To transmit data to a device from a derived class, use the
`::picolibrary::SPI::Device::transmit()` member functions.
`::picolibrary::SPI::Device` automated tests are defined in the
[`test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/spi/gpio_output_pin_device_selector/main.cc)
source file.

The `::picolibrary::Testing::Automated::SPI::Mock_Device` mock SPI device class is
available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/spi.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/spi.h)/[`source/picolibrary/testing/automated/spi.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/spi.cc)
header/source file pair.
