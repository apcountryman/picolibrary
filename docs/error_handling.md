# Error Handling Facilities

## Table of Contents
1. [Error Identification](#error-identification)
    1. [Library Defined Errors](#library-defined-errors)
    1. [Defining Additional Errors](#defining-additional-errors)
1. [Assertions](#assertions)
    1. [Checking Precondition Expectations](#checking-precondition-expectations)
    1. [Checking Postcondition Guarantees](#checking-postcondition-guarantees)
    1. [Trapping Fatal Errors](#trapping-fatal-errors)
1. [Error Propagation](#error-propagation)

## Error Identification
Error identification facilities are defined in the
[`include/picolibrary/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/error.h)/[`source/picolibrary/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/error.cc)
header/source file pair.

The `::picolibrary::Error_Code` class is used to identify errors.
An error can be identified using the combination of the address of the error's error
category (`::picolibrary::Error_Category`) and the error's error ID
(`::picolibrary::Error_ID`).
- To get a reference to an error's error category, use the
  `::picolibrary::Error_Code::category()` member function.
- To get an error's error ID, use the `::picolibrary::Error_Code::id()` member function.
- Implicit conversion from an enum class to a `::picolibrary::Error_Code` is enabled if
  `::picolibrary::is_error_code_enum` has been appropriately specialized for the enum
  class.
- Direct comparison for equality or inequality is supported.

`::picolibrary::Error_Code` automated tests are defined in the
[`test/automated/picolibrary/error_code/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/error_code/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::Error_Code` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/error.h)/[`source/picolibrary/testing/automated/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/error.cc)
header/source file pair.

The `::picolibrary::Error_Category` class is used to get human readable error information.
- To get an error category's name, use the `::picolibrary::Error_Category::name()` member
  function.
- To get an error ID's description, use the
  `::picolibrary::Error_Category::error_description()` member function.

If the `PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION` project configuration
option is `ON`, these functions will return empty strings.

### Library Defined Errors
picolibrary defines the following error sets:
- Generic errors
- Mock errors

The generic error set is defined in the
[`include/picolibrary/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/error.h)/[`source/picolibrary/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/error.cc)
header/source file pair.

The `::picolibrary::Generic_Error` enum class's enumerators identify the specific errors
in the set.
Implicit conversion from `::picolibrary::Generic_Error` to `::picolibrary::Error_Code` is
enabled.

The `::picolibrary::Generic_Error_Category` class is the error category for the set.
- To get a reference to the `::picolibrary::Generic_Error_Category` instance, use the
  `::picolibrary::Generic_Error_Category::instance()` static member function.

Generic error set automated tests are defined in the
[`test/automated/picolibrary/generic_error/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/generic_error/main.cc)
and
[`test/automated/picolibrary/generic_error_category/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/generic_error_category/main.cc)
source files.

A `std::ostream` insertion operator is defined for `::picolibrary::Generic_Error` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/error.h)/[`source/picolibrary/testing/automated/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/error.cc)
header/source file pair.

The mock error set is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock error set is defined in the
[`include/picolibrary/testing/automated/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/error.h)/[`source/picolibrary/testing/automated/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/error.cc)
header/source file pair.

The `::picolibrary::Testing::Automated::Mock_Error` enum class is used to identify
"specific" errors in the set.
`::picolibrary::Testing::Automated::random()` is specialized for the
`::picolibrary::Testing::Automated::Mock_Error` enum class so that "specific"
pseudo-random mock errors can be generated at runtime for use in automated tests.

The `::picolibrary::Testing::Automated::Mock_Error_Category` class is the error category
for the set.
- To get a reference to a common `::picolibrary::Testing::Automated::Mock_Error_Category`
  instance, use the `::picolibrary::Testing::Automated::Mock_Error_Category::instance()`
  static member function.
  Additional `::picolibrary::Testing::Automated::Mock_Error_Category` instances can be
  constructed as needed to support automated testing.

### Defining Additional Errors
To create an additional error set, do the following:
1. Define an enum class whose enumerators identify the specific errors in the set.
   The underlying type for the enum class should be `::picolibrary::Error_ID`.
2. Create an error category class for the error set.
   `::picolibrary::Error_Category` must be a public base of this class.
   This class should be a singleton.
3. Define a `make_error_code()` function in the same namespace as the enum class that
   takes the enum class and returns a `::picolibrary::Error_Code`.
4. Register the enum class as an error code enum by specializing
   `::picolibrary::is_error_code_enum` for the enum class.

## Assertions

### Checking Precondition Expectations
Facilities for checking precondition expectations are defined in the
[`include/picolibrary/precondition.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/precondition.h)/[`source/picolibrary/precondition.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/precondition.cc)
header/source file pair.

To check a function precondition expectation, use the `PICOLIBRARY_EXPECT()` macro.
If the `expectation` macro argument evaluates to `false`,
`::picolibrary::trap_fatal_error()` will be called.
If code is structured in a way that an execution path being taken indicates a precondition
expectation has not been met, use the `PICOLIBRARY_EXPECTATION_NOT_MET()` macro to
unconditionally call `::picolibrary::trap_fatal_error()`.

To create a function overload that lets the user bypass precondition expectation checks,
use the `::picolibrary::Bypass_Precondition_Expectation_Checks` type.
Users can then use the `::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS` constant to
select the function overload that bypasses the precondition expectation checks.
```c++
#include <cstdint>

#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/precondition.h"

void foo() noexcept
{
    auto vector = ::picolibrary::Fixed_Capacity_Vector<std::uint_fast8_t, 8>{};

    // precondition expectation checks run
    vector.push_back( 0 );

    // precondition expectation checks bypassed
    vector.push_back( ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0 );
}
```

To create a function overload that lets the user run the function's precondition
expectation checks while bypassing a called function's precondition expectation checks,
use the `::picolibrary::Run_Precondition_Expectation_Checks` type.
Users can then use the `::picolibrary::RUN_PRECONDITION_EXPECTATION_CHECKS` constant to
select the function overload that runs the function's precondition expectation checks
while bypassing the called function's precondition expectation checks.
```c++
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/i2c.h"
#include "picolibrary/precondition.h"

void foo() noexcept
{
    auto vector = ::picolibrary::Fixed_Capacity_Vector<::picolibrary::I2C::Address_Numeric, 8>{};

    // ::picolibrary::Fixed_Capacity_Vector::emplace_back() precondition expectation checks run
    // ::picolibrary::I2C::Address_Numeric::Address_Numeric() precondition expectation checks run
    vector.emplace_back( 0b0100'000 );

    // ::picolibrary::Fixed_Capacity_Vector::emplace_back() precondition expectation checks bypassed
    // ::picolibrary::I2C::Address_Numeric::Address_Numeric() precondition expectation checks run
    vector.emplace_back( ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS, 0b0100'000 );

    // ::picolibrary::Fixed_Capacity_Vector::emplace_back() precondition expectation checks run
    // ::picolibrary::I2C::Address_Numeric::Address_Numeric() precondition expectation checks run
    vector.emplace_back( ::picolibrary::RUN_PRECONDITION_EXPECTATION_CHECKS, 0b0100'000 );

    // ::picolibrary::Fixed_Capacity_Vector::emplace_back() precondition expectation checks bypassed
    // ::picolibrary::I2C::Address_Numeric::Address_Numeric() precondition expectation checks bypassed
    vector.emplace_back(
        ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS,
        ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS,
        0b0100'000 );

    // ::picolibrary::Fixed_Capacity_Vector::emplace_back() precondition expectation checks run
    // ::picolibrary::I2C::Address_Numeric::Address_Numeric() precondition expectation checks bypassed
    vector.emplace_back(
        ::picolibrary::RUN_PRECONDITION_EXPECTATION_CHECKS,
        ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS,
        0b0100'000 );
}
```

### Checking Postcondition Guarantees
Facilities for checking postcondition guarantees are defined in the
[`include/picolibrary/postcondition.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/postcondition.h)/[`source/picolibrary/postcondition.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/postcondition.cc)
header/source file pair.

To check a function postcondition guarantee, use the `PICOLIBRARY_ENSURE()` macro.
If the `guarantee` macro argument evaluates to `false`,
`::picolibrary::trap_fatal_error()` will be called.
If code is structured in a way that an execution path being taken indicates a
postcondition guarantee has not been met, use the `PICOLIBRARY_GUARANTEE_NOT_MET()` macro
to unconditionally call `::picolibrary::trap_fatal_error()`.

### Trapping Fatal Errors
`::picolibrary::trap_fatal_error()` is called if a precondition expectation check or a
postcondition guarantee check fails.
This function is declared in
[`include/picolibrary/fatal_error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/fatal_error.h).
The `picolibrary` static library does not provide an implementation for this function.
See
[`include/picolibrary/fatal_error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/fatal_error.h)
for more information.

## Error Propagation
Facilities for propagating errors are defined in the
[`include/picolibrary/result.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/result.h)/[`source/picolibrary/result.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/result.cc)
header/source file pair.

The `::picolibrary::Result` class is used as the return type for operations that can fail
and report failure information to the caller instead of using assertions.
`::picolibrary::Result` has specializations that support the following use cases:
- No information is generated by the operation (`::picolibrary::Result<void, false>`)
- Trivially destructible information is generated by the operation
  (`::picolibrary::Result<Value_Type, true>`)
- Non-trivially destructible information is generated by the operation
  (`::picolibrary::Result<Value_Type, false>`)

Implicit conversion from `Value_Type` or `::picolibrary::Error_Code` to
`::picolibrary::Result` is enabled if implicit conversion between `Value_Type` and
`::picolibrary::Error_Code` is not possible.
If implicit conversion between `Value_Type` and `::picolibrary::Error_Code` is possible,
the `::picolibrary::Result` must be explicitly constructed using the
`::picolibrary::Value_Tag` or `::picolibrary::Error_Tag` constructor overloads.
The `::picolibrary::VALUE` and `::picolibrary::ERROR` constants are provide to support
this.

`::picolibrary::Result` supports the following operations:
- To check if an operation failed, use the `::picolibrary::Result::is_error()` member
  function.
- To get the result of a failed operation, use the `::picolibrary::Result::error()` member
  function.
- To get the result of a successful operation that generated information, use the
  `::picolibrary::Result::value()` member function.
