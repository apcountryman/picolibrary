# Media Access Control (MAC) Address
The `::picolibrary::MAC_Address` MAC address class is defined in the
[`include/picolibrary/mac_address.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/mac_address.h)/[`source/picolibrary/mac_address.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/mac_address.cc)
header/source file pair.
To get the minimum valid address, use the `::picolibrary::MAC_Address::min()` static
member function.
To get the maximum valid address, use the `::picolibrary::MAC_Address::max()` static
member function.
To check if an address is locally administered, use the
`::picolibrary::MAC_Address::is_locally_administered()` member function.
To check if an address is universally administered, use the
`::picolibrary::MAC_Address::is_universally_administered()` member function.
To check if an address is a multicast address, use the
`::picolibrary::MAC_Address::is_multicast()` member function.
To check if an address is a unicast address, use the
`::picolibrary::MAC_Address::is_unicast()` member function.
To get an address in its byte array representation, use the
`::picolibrary::MAC_Address::as_byte_array()` member function.
To get an address in its unsigned integer representation, use the
`::picolibrary::MAC_Address::as_unsigned_integer()` member function.
Direct comparison is supported.
`::picolibrary::MAC_Address` automated tests are defined in the
[`test/automated/picolibrary/mac_address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/mac_address/main.cc)
source file.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::MAC_Address`.
The `::picolibrary::Output_Formatter<::picolibrary::MAC_Address>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::MAC_Address>` automated tests are defined
in the
[`test/automated/picolibrary/mac_address/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/mac_address/main.cc)
source file.

A `::picolibrary::Testing::Automated::random()` specialization for
`::picolibrary::MAC_Address` is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING`
project configuration option is `ON`.
The specialization is defined in the
[`include/picolibrary/testing/automated/mac_address.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/mac_address.h)/[`source/picolibrary/testing/automated/mac_address.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/mac_address.cc)
header/source file pair.
