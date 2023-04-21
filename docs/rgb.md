# RGB Facilities
RGB facilities are defined in the
[`include/picolibrary/rgb.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/rgb.h)/[`source/picolibrary/rgb.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/rgb.cc)
header/source file pair.

## Table of Contents
- [Color](#color)

## Color
The `::picolibrary::RGB::Color` struct is used to identify a color.

A `::picolibrary::Testing::Automated::random()` specialization for
`::picolibrary::RGB::Color` is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The specialization is defined in the
[`include/picolibrary/testing/automated/rgb.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/rgb.h)/[`source/picolibrary/testing/automated/rgb.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/rgb.cc)
header/source file pair.
