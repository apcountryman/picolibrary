# Usage

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
See `include/picolibrary/fatal_error.h` for more information.

## Development
The repository's Git `pre-commit` hook script is the simplest way to configure, build, and
test picolibrary during development.
See the `pre-commit` script's help text for usage details.
```shell
./git/hooks/pre-commit --help
```

Additional checks, such as static analysis, are performed by the repository's GitHub
Actions CI workflow.
