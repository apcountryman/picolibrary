# Error Handling Facilities

## Error Identification
Error identification facilities are defined in the
[`include/picolibrary/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/error.h)/[`source/picolibrary/error.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/error.cc)
header/source file pair.

The `::picolibrary::Error_Code` type is used to identify errors.
An error can be identified using the combination of the address of the error's error
category (`::picolibrary::Error_Category`) and the error's error ID
(`::picolibrary::Error_ID`).
To get a reference to an error's error category, use the
`::picolibrary::Error_Code::category()` member function.
To get an error's error ID, use the `::picolibrary::Error_Code::id()` member function.

## Assertions

### Checking Precondition Expectations
Facilities for checking precondition expectations are defined in the
[`include/picolibrary/precondition.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/precondition.h)/[`source/picolibrary/precondition.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/precondition.cc)
header/source file pair.

To check a function precondition expectation, use the `PICOLIBRARY_EXPECT()` macro.
If the `expectation` macro argument evaluates to `false`,
`::picolibrary::trap_fatal_error()` will be called.
If code is structured in a way that an execution path being taken indicates a precondition
expectation has not been met, the `PICOLIBRARY_EXPECTATION_NOT_MET()` macro can be used to
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
postcondition guarantee has not been met, the `PICOLIBRARY_GUARANTEE_NOT_MET()` macro can
be used to unconditionally call `::picolibrary::trap_fatal_error()`.

### Trapping Fatal Errors

## Error Propagation
