# string_encoding
String Encoding Conversion Library

This library provides algorithms and convenience functions that convert the
encoding of C++ character sequences.

Sequences and strings of character types `char`, `wchar_t`, `char16_t`, and `char32_t`
are supported.

Support is provided for native encodings, `codecvt<wchar_t, char>` encodings, and
Unicode Transformation Format encodings UTF-8, UTF-16, and UTF-32.

The relationship between character type and supported encodings is shown in the
following table:

| Type | Encodings |
| ----- | ------ |
| `char` | native, any with `codecvt<wchar_t, char>`, UTF-8 |
| `wchar_t` | native |
| `char16_t` | UTF-16 |
| `char32_t`| UFT-32 |

© Beman Dawes 2015

The library is licensed under the Boost Software License version 1.0.
