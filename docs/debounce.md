# Debounce Facilities
Debounce facilities are defined in the
[`include/picolibrary/debounce.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/debounce.h)/[`source/picolibrary/debounce.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/debounce.cc)
header/source file pair.

## Table of Contents
1. [Single Signal Debouncer](#single-signal-debouncer)

## Single Signal Debouncer
The `::picolibrary::Debounce::Single_Signal_Debouncer` class is used to debounce a single
signal.
- To update a debouncer with a new signal sample, use the
  `::picolibrary::Debounce::Single_Signal_Debouncer::debounce()` member function.
- To check if a debounced signal is in the high state, use the
  `::picolibrary::Debounce::Single_Signal_Debouncer::is_high()` member function.
- To check if a debounced signal is in the low state, use the
  `::picolibrary::Debounce::Single_Signal_Debouncer::is_low()` member function.

`::picolibrary::Debounce::Single_Signal_Debouncer` automated tests are defined in the
[`test/automated/picolibrary/debounce/single_signal_debouncer/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/debounce/single_signal_debouncer/main.cc)
source file.
