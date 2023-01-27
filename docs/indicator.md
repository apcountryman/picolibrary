# Indicator Facilities
Indicator facilities are defined in the
[`include/picolibrary/indicator.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/indicator.h)/[`source/picolibrary/indicator.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/indicator.cc)
header/source file pair.

## Fixed Intensity Indicator
The `::picolibrary::Indicator::Fixed_Intensity_Indicator_Concept` concept class defines
the expected interface of a fixed intensity indicator.
To initialize an indicator's hardware, use an indicator implementation's `initialize()`
member function.
To extinguish an indicator, use an indicator implementation's `extinguish()` member
function.
To illuminate an indicator, use an indicator implementation's `illuminate()` member
function.
To toggle the state of an indicator, use an indicator implementation's `toggle()` member
function.

The `::picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator` template class
is a GPIO output pin based fixed intensity indicator implementation.
`::picolibrary::Indicator::GPIO_Output_Pin_Fixed_Intensity_Indicator` automated tests are
defined in the
[`test/automated/picolibrary/indicator/gpio_output_pin_fixed_intensity_indicator/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/indicator/gpio_output_pin_fixed_intensity_indicator/main.cc)
source file.

The `::picolibrary::Testing::Automated::Indicator::Mock_Fixed_Intensity_Indicator` mock
fixed intensity indicator class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/indicator.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/indicator.h)/[`source/picolibrary/testing/automated/indicator.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/indicator.cc)
header/source file pair.
