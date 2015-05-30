# string_encoding
String Encoding Conversion Library

**This library is under development. It is unstable, buggy, and not suitable for production use.**

This library provides algorithms and convenience functions that convert the
encoding of C++ character sequences.

Sequences and strings of character types `char`, `wchar_t`, `char16_t`, and `char32_t`
are supported.

Support is provided for native encodings, `codecvt<wchar_t, char>` supported encodings, and
Unicode Transformation Format encodings UTF-8, UTF-16, and UTF-32.

The relationship between character type and supported encodings is shown in the
following table:

| Type | Encodings |
| ----- | ------ |
| `char` | native, any encoding with `codecvt<wchar_t, char>` support, UTF-8 |
| `wchar_t` | native |
| `char16_t` | UTF-16 |
| `char32_t`| UFT-32 |

---

© Beman Dawes 2015

This library is licensed under the Boost Software License version 1.0.
See http://www.boost.org/users/license.html
