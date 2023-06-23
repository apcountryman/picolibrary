# Automated Testing
picolibrary uses [GoogleTest](https://github.com/google/googletest) for automated testing.

## Table of Contents
1. [Tests](#tests)
1. [Utilities](#utilities)
    1. [Movable Mock Handles](#movable-mock-handles)

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
