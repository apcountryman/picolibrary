# Read-Only Memory (ROM) Facilities
ROM facilities are defined in the `include/picolibrary/rom.h`/`source/picolibrary/rom.cc`
header/source file pair.

## Null-Terminated Strings
The `::picolibrary::ROM::String` type is used for handles to null-terminated strings that
may be stored in ROM.
This type defaults to `char const *`.
A HIL can replace this type with a HIL specific version by doing the following:
- Configure the `PICOLIBRARY_HIL_INCLUDE_DIR` picolibrary project configuration option
- Provide `picolibrary/hil/rom.h`
- Define `PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED` in `picolibrary/hil/rom.h`
- Define the HIL specific version of `::picolibrary::ROM::String` in
  `picolibrary/hil/rom.h`

If a function is being overloaded to work with null-terminated strings stored in RAM and
null-terminated strings that may be stored in ROM, the `::picolibrary::ROM::String`
overload must only be defined if `PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED` is defined since
`::picolibrary::ROM::String` defaults to `char const *`:
```c++
void foo( char const * string ) noexcept
{
}

#ifdef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
void foo( ::picolibrary::ROM::String string ) noexcept;
{
}
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
```

To create a string literal that can be stored in ROM, use the `PICOLIBRARY_ROM_STRING()`
macro:
```c++
auto foo() noexcept -> ::picolibrary::ROM::String
{
    return PICOLIBRARY_ROM_STRING( "string" );
}
```
If a toolchain does not store string literals in ROM automatically, a HIL can replace the
`PICOLIBRARY_ROM_STRING()` macro by doing the following:
- Configure the `PICOLIBRARY_HIL_INCLUDE_DIR` picolibrary project configuration option
- Provide `picolibrary/hil/rom.h`
- Define the macro replacement in `picolibrary/hil/rom.h`

To get the length of a null-terminated string that may be stored in ROM, use the
`::picolibrary::ROM::strlen()` function:
```c++
void foo( ::picolibrary::ROM::String string ) noexcept
{
    auto const length = ::picolibrary::ROM::strlen( string );
}
```
