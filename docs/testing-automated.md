# Automated Testing
picolibrary uses [GoogleTest](https://github.com/google/googletest) for automated testing.

## Table of Contents
1. [Tests](#tests)
1. [Utilities](#utilities)
    1. [Movable Mock Handles](#movable-mock-handles)
    1. [Runtime Pseudo-Random Value Generation](#runtime-pseudo-random-value-generation)

## Tests
Automated tests can be found in the
[`test/automated/picolibrary/`](https://github.com/apcountryman/picolibrary/tree/main/test/automated/picolibrary)
directory.
The location of the automated tests for a particular library feature can be found in the
documentation for the feature.
Not all library features have automated tests.

## Utilities

### Movable Mock Handles
`::picolibrary::Testing::Automated::Mock_Handle` is a utility for creating movable mock
handles and is defined in
[`include/picolibrary/testing/automated/mock_handle.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/mock_handle.h).
Examples of how to use `::picolibrary::Testing::Automated::Mock_Handle` can be found in
[`include/picolibrary/testing/automated/`](https://github.com/apcountryman/picolibrary/tree/main/include/picolibrary/testing/automated).

### Runtime Pseudo-Random Value Generation
A variety of utilities for generating pseudo-random values at runtime to aid automated
testing are defined in
[`include/picolibrary/testing/automated/random.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/random.h).

`::picolibrary::Testing::Automated::pseudo_random_number_generator()` is used to
initialize and access the pseudo-random number generator.

The `::picolibrary::Testing::Automated::random()` overload set is used to generate a
pseudo-random value.
The default implementation supports generation of a pseudo-random integer value in a user
specified range.
Specializations are provided for `bool` and `char`.
Additional specializations for some library provided types are defined in other header
files.
Documentation for these specializations can be found in the documentation for the
associated library provided type.

`::picolibrary::Testing::Automated::random_unique_pair()` is used to generate a pair of
unique pseudo-random values.

The `::picolibrary::Testing::Automated::random_container()` overload set is used to
generate a standard container populated with pseudo-randomly generated values.

The `::picolibrary::Testing::Automated::random_array()` overload set is used to generate a
`::picolibrary::Array` populated with pseudo-randomly generated values.

`::picolibrary::Testing::Automated::random_unique_container_pair()` is used to generate a
pair of unique standard containers populated with pseudo-randomly generated values.
