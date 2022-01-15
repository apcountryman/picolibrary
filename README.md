# picolibrary
[![CI](https://github.com/apcountryman/picolibrary/actions/workflows/ci.yml/badge.svg)](https://github.com/apcountryman/picolibrary/actions/workflows/ci.yml)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.0-4baaaa.svg)](CODE_OF_CONDUCT.md)

A C++ microcontroller driver/utility library targeted for use with resource
constrained microcontrollers.

## Obtaining the Source Code
HTTPS:
```shell
git clone --recurse-submodules https://github.com/apcountryman/picolibrary.git
```
SSH:
```shell
git clone --recurse-submodules git@github.com:apcountryman/picolibrary.git
```

## Usage (Dependency)
To use this library as a dependency, simply add the subdirectory containing this
repository to a CMake build, and link with the `picolibrary` static library.
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

## Usage (Development)
This repository's Git `pre-commit` hook script is the simplest way to configure, build,
and test this project during development.
See the `pre-commit` script's help text for usage details.
```shell
./git/hooks/pre-commit --help
```

Additional checks, such as static analysis, are performed by this project's GitHub Actions
CI workflow.

## Versioning
Post version 0.1.0, `picolibrary` will follow the [Abseil Live at Head
philosophy](https://abseil.io/about/philosophy).

## Workflow
`picolibrary` uses the [GitHub flow](https://guides.github.com/introduction/flow/)
workflow.

## Git Hooks
To install this repository's Git hooks, execute the `install` script located in the
`git/hooks` directory.
See the `install` script's help text for usage details.
```shell
./git/hooks/install --help
```

## Code of Conduct
`picolibrary` has adopted the Contributor Covenant 2.0 code of conduct.
For more information, [see the `CODE_OF_CONDUCT.md` file in this
repository](CODE_OF_CONDUCT.md).

## Contributing
If you are interested in contributing to `picolibrary`, please [read the `CONTRIBUTING.md`
file in this repository](CONTRIBUTING.md).

## Authors
- Andrew Countryman
- Jay Lamb

## License
`picolibrary` is licensed under the Apache License, Version 2.0.
For more information, [see the `LICENSE` file in this repository](LICENSE).
