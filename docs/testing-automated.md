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

## Test Coverage
To generate an automated testing coverage report, build the
`picolibrary-automated-testing-coverage` target.
The automated testing coverage report can then be found in the build directory's
`picolibrary-automated-testing-coverage-report` subdirectory.
The reported coverage will be misleadingly high due to picolibrary's heavy use of
templates that are not explicitly instantiated.

## Utilities

### Movable Mock Handles
`::picolibrary::Testing::Automated::Mock_Handle` is a utility for creating movable mock
handles and is defined in
[`include/picolibrary/testing/automated/mock_handle.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/mock_handle.h).
Examples of how to use `::picolibrary::Testing::Automated::Mock_Handle` can be found in
[`include/picolibrary/testing/automated/`](https://github.com/apcountryman/picolibrary/tree/main/include/picolibrary/testing/automated).
