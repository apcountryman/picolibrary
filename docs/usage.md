# Usage

## Table of Contents
1. [Dependency](#dependency)
    1. [Configuration Options](#configuration-options)
1. [Development](#development)

## Dependency
To use picolibrary as a dependency, simply add the subdirectory containing the repository
to a CMake build, and link with the `picolibrary` static library.
```cmake
add_subdirectory( picolibrary )
```
```cmake
target_link_libraries(
    foo
    picolibrary
)
```

The `picolibrary` static library does not provide an implementation for
`::picolibrary::trap_fatal_error()`.
See
[`include/picolibrary/fatal_error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/fatal_error.h)
for more information.

The `picolibrary` static library does not include `::picolibrary::version()`.
To use `::picolibrary::version()`, link with the `picolibrary-version` static library.
```cmake
target_link_libraries(
    foo
    picolibrary
    picolibrary-version
)
```

### Configuration Options
picolibrary supports the following project configuration options:
- `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` (defaults to `OFF`): enable automated testing
    - The following project configuration options are available if
      `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` is `ON`:
        - `PICOLIBRARY_USE_PARENT_PROJECT_GOOGLE_TEST` (defaults to `ON`): use parent
          project's Google Test
- `PICOLIBRARY_ENABLE_INTERACTIVE_TESTING` (defaults to `OFF`): enable interactive testing
- `PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION` (defaults to `OFF`):
  suppress assertion failure location information (see
  [`include/picolibrary/fatal_error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/fatal_error.h),
  [`include/picolibrary/precondition.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/precondition.h),
  and
  [`include/picolibrary/postcondition.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/postcondition.h),
  for more information)
- `PICOLIBRARY_SUPPRESS_HUMAN_READABLE_ERROR_INFORMATION` (defaults to `OFF`): suppress
  human readable error information (see
  [`include/picolibrary/error.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/error.h)
  for more information)
- `PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION` (defaults to `OFF`): suppress
  human readable event information (see
  [`include/picolibrary/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/event.h)
  for more information)

## Development
The repository's Git `pre-commit` hook script is the simplest way to configure, build, and
test picolibrary during development.
See the `pre-commit` script's help text for usage details.
```shell
./git/hooks/pre-commit --help
```

Additional checks, such as static analysis, are performed by the repository's GitHub
Actions CI workflow.
