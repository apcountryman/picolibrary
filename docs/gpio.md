# General Purpose Input/Output (GPIO) Facilities
GPIO facilities are defined in the
[`include/picolibrary/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/gpio.h)/[`source/picolibrary/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/gpio.cc)
header/source file pair.

## Input Pin
The `::picolibrary::GPIO::Input_Pin_Concept` concept class defines the expected interface
of a GPIO input pin.
To initialize an input pin's hardware, use an input pin implementation's `initialize()`
member function.
To check if an input pin is in the low state, use an input pin implementation's `is_low()`
member function.
To check if an input pin is in the high state, use an input pin implementation's
`is_high()` member function.

picolibrary assumes that the high pin/signals state is the active pin/signal state.
All input pin implementations should use this assumption.
If the high pin/signal state is not the active pin/signal state, the
`::picolibrary::GPIO::Active_Low_Input_Pin` template class can be used to invert an input
pin implementation's behavior.
`::picolibrary::GPIO::Active_Low_Input_Pin` automated tests are defined in the
[`test/automated/picolibrary/gpio/active_low_input_pin/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/gpio/active_low_input_pin/main.cc)
source file.

The `::picolibrary::Testing::Automated::GPIO::Mock_Input_Pin` mock GPIO input pin class is
available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/gpio.h)/[`source/picolibrary/testing/automated/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/gpio.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::GPIO::state()` interactive test helper is
available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option is
`ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/gpio.h)/[`source/picolibrary/testing/interactive/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/gpio.cc)
header/source file pair.

## Internally Pulled-Up Input Pin
The `::picolibrary::GPIO::Internally_Pulled_Up_Input_Pin_Concept` concept class defines
the expected interface of a GPIO internally pulled-up input pin.
Internally pulled-up input pin functionality is a superset of input pin functionality.
To initialize an internally pulled-up input pin's hardware, use an internally pulled-up
input pin implementation's `initialize()` member function.
To check if an internally pulled-up input pin's internal pull-up resistor is disabled, use
an internally pulled-up input pin implementation's `pull_up_is_disabled()` member
function.
To check if an internally pulled-up input pin's internal pull-up resistor is enabled, use
an internally pulled-up input pin implementation's `pull_up_is_enabled()` member function.
To disable an internally pulled-up input pin's internal pull-up resistor, use an
internally pulled-up input pin implementation's `disable_pull_up()` member function.
To enable an internally pulled-up input pin's internal pull-up resistor, use an internally
pulled-up input pin implementation's `enable_pull_up()` member function.
To check if an internally pulled-up input pin is in the low state, use an internally
pulled-up input pin implementation's `is_low()` member function.
To check if an internally pulled-up input pin is in the high state, use an internally
pulled-up input pin implementation's `is_high()` member function.

The `::picolibrary::Testing::Automated::GPIO::Mock_Internally_Pulled_Up_Input_Pin` mock
GPIO internally pulled-up input pin class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/gpio.h)/[`source/picolibrary/testing/automated/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/gpio.cc)
header/source file pair.

## Output Pin
The `::picolibrary::GPIO::Output_Pin_Concept` concept class defines the expected interface
of a GPIO output pin.
To initialize an output pin's hardware, use an output pin implementation's `initialize()`
member function.
To transition an output pin to the low state, use an output pin implementation's
`transition_to_low()` member function.
To transition an output pin to the high state, use an output pin implementation's
`transition_to_high()` member function.
To toggle the state of an output pin, use an output pin implementation's `toggle()` member
function.

picolibrary assumes that the high pin/signals state is the active pin/signal state.
All output pin implementations should use this assumption.
If the high pin/signal state is not the active pin/signal state, the
`::picolibrary::GPIO::Active_Low_Output_Pin` template class can be used to invert an
output pin implementation's behavior.
`::picolibrary::GPIO::Active_Low_Output_Pin` automated tests are defined in the
[`test/automated/picolibrary/gpio/active_low_output_pin/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/gpio/active_low_output_pin/main.cc)
source file.

The `::picolibrary::Testing::Automated::GPIO::Mock_Output_Pin` mock GPIO output pin class
is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/gpio.h)/[`source/picolibrary/testing/automated/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/gpio.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::GPIO::toggle()` interactive test helper is
available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` project configuration option is
`ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/gpio.h)/[`source/picolibrary/testing/interactive/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/gpio.cc)
header/source file pair.

## Input/Output (I/O) Pin
The `::picolibrary::GPIO::IO_Pin_Concept` concept class defines the expected interface of
a GPIO I/O pin.
To initialize an I/O pin's hardware, use an I/O pin implementation's `initialize()` member
function.
To check if an I/O pin is in the low state, use an I/O pin implementation's `is_low()`
member function.
To check if an I/O pin is in the high state, use an I/O pin implementation's `is_high()`
member function.
To transition an I/O pin to the low state, use an I/O pin implementation's
`transition_to_low()` member function.
To transition an I/O pin to the high state, use an I/O pin implementation's
`transition_to_high()` member function.
To toggle the state of an I/O pin, use an I/O pin implementation's `toggle()` member
function.

picolibrary assumes that the high pin/signals state is the active pin/signal state.
All I/O pin implementations should use this assumption.
If the high pin/signal state is not the active pin/signal state, the
`::picolibrary::GPIO::Active_Low_IO_Pin` template class can be used to invert an I/O pin
implementation's behavior.

The `::picolibrary::Testing::Automated::GPIO::Mock_IO_Pin` mock GPIO I/O pin class is
available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is
`ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/gpio.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/gpio.h)/[`source/picolibrary/testing/automated/gpio.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/gpio.cc)
header/source file pair.
