# Library Version
To access the null-terminated library version string that may be stored in ROM, include
[`picolibrary/version.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/version.h)
and use `::picolibrary::version()`:
```c++
#include "picolibrary/stream.h"
#include "picolibrary/version.h"

void foo( ::picolibrary::Reliable_Output_Stream & stream ) noexcept
{
    stream.put( ::picolibrary::version() );
}
```
The null-terminated library version string is populated with the output of the following
Git command:
```shell
git describe --match=none --always --dirty --broken
```

The `picolibrary` static library does not include `::picolibrary::version()`.
To use `::picolibrary::version()`, link with the `picolibrary-version` static library.
```cmake
target_link_libraries(
    foo
    picolibrary
    picolibrary-version
)
```
