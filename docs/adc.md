# ADC Facilities
ADC facilities are defined in the
[`include/picolibrary/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/adc.h)/[`source/picolibrary/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/adc.cc)
header/source file pair.

## Table of Contents
1. [Sample](#sample)
1. [ADC Types](#adc-types)
    1. [Blocking, Single Sample ADC](#blocking-single-sample-adc)
    1. [Non-Blocking, Single Sample ADC](#non-blocking-single-sample-adc)
    1. [Blocking, Free Running ADC](#blocking-free-running-adc)
    1. [Non-Blocking, Free Running ADC](#non-blocking-free-running-adc)

## Sample
The `::picolibrary::ADC::Sample` template class is used to store ADC samples.
- To get the minimum valid sample, use the `::picolibrary::ADC::Sample::min()` static
  member function.
- To get the maximum valid sample, use the `::picolibrary::ADC::Sample::max()` static
  member function.
- To get a sample in its unsigned integer representation, use the
  `::picolibrary::ADC::Sample::as_unsigned_integer()` member function.
- Direct comparison is supported.

`::picolibrary::ADC::Sample` automated tests are defined in the
[`test/automated/picolibrary/adc/sample/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/adc/sample/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::ADC::Sample` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.

`::picolibrary::Testing::Automated::random()` specializations for several
`::picolibrary::ADC::Sample` types are available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The specializations are defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.
`::picolibrary::Testing::Automated::random()` specializations are defined for the
following `::picolibrary::ADC::Sample` types:
- `::picolibrary::ADC::Sample<std::uint_fast8_t, 8>`
- `::picolibrary::ADC::Sample<std::uint_fast16_t, 10>`
- `::picolibrary::ADC::Sample<std::uint_fast16_t, 12>`
- `::picolibrary::ADC::Sample<std::uint_fast16_t, 14>`
- `::picolibrary::ADC::Sample<std::uint_fast16_t, 16>`
- `::picolibrary::ADC::Sample<std::uint_fast32_t, 18>`
- `::picolibrary::ADC::Sample<std::uint_fast32_t, 20>`
- `::picolibrary::ADC::Sample<std::uint_fast32_t, 24>`

## ADC Types

### Blocking, Single Sample ADC
The `::picolibrary::ADC::Blocking_Single_Sample_Converter_Concept` concept class defines
the expected interface of a blocking, single sample ADC implementation.
- To initialize an ADC's hardware, use an ADC implementation's `initialize()` member
  function.
- To get a sample, use an ADC implementation's `sample()` member function.

The `::picolibrary::Testing::Automated::ADC::Mock_Blocking_Single_Sample_Converter` mock
blocking, single sample ADC class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.

The `::picolibrary::Testing::Interactive::ADC::sample_blocking_single_sample_converter()`
interactive test helper is available if the `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING`
project configuration option is `ON`.
The interactive test helper is defined in the
[`include/picolibrary/testing/interactive/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/interactive/adc.h)/[`source/picolibrary/testing/interactive/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/interactive/adc.cc)
header/source file pair.

### Non-Blocking, Single Sample ADC
The `::picolibrary::ADC::Non_Blocking_Single_Sample_Converter_Concept` concept class
defines the expected interface of a non-blocking, single sample ADC implementation.
- To initialize an ADC's hardware, use an ADC implementation's `initialize()` member
  function.
- To initiate a conversion, use an ADC implementation's `initiate_conversion()` member
  function.
- To check if a sample is available, use an ADC implementation's `sample_is_available()`
  member function.
- To get a sample, use an ADC implementation's `sample()` member function.

The `::picolibrary::Testing::Automated::ADC::Mock_Non_Blocking_Single_Sample_Converter`
mock non-blocking, single sample ADC class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.

### Blocking, Free Running ADC
The `::picolibrary::ADC::Blocking_Free_Running_Converter_Concept` concept class defines
the expected interface of a blocking, free running ADC implementation.
- To initialize an ADC's hardware, use an ADC implementation's `initialize()` member
  function.
- To start an ADC, use an ADC implementation's `start_converter()` member function.
- To stop an ADC, use an ADC implementation's `stop_converter()` member function.
- To get a sample, use an ADC implementation's `sample()` member function.

The `::picolibrary::Testing::Automated::ADC::Mock_Blocking_Free_Running_Converter` mock
blocking, free running ADC class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.

### Non-Blocking, Free Running ADC
The `::picolibrary::ADC::Non_Blocking_Free_Running_Converter_Concept` concept class
defines the expected interface of a non-blocking, free running ADC implementation.
- To initialize an ADC's hardware, use an ADC implementation's `initialize()` member
  function.
- To start an ADC, use an ADC implementation's `start_converter()` member function.
- To stop an ADC, use an ADC implementation's `stop_converter()` member function.
- To check if a sample is available, use an ADC implementation's `sample_is_available()`
  member function.
- To get a sample, use an ADC implementation's `sample()` member function.

The `::picolibrary::Testing::Automated::ADC::Mock_Non_Blocking_Free_Running_Converter`
mock non-blocking, free running ADC class is available if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/adc.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/adc.h)/[`source/picolibrary/testing/automated/adc.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/adc.cc)
header/source file pair.
