# unicode
Unicode Support Library

**This library is under development. It is unstable, buggy, and not suitable for production use.**

This library provides algorithms and convenience functions intended to extend Unicode
support for the C++ standard library and its technical reports. No modifications are made
to the existing standard library. Instead, the Unicode support already built into the C++
language and standard library is extended by providing additional functionality.

Sequences and strings of character types `char`, `wchar_t`, `char16_t`, and `char32_t`
are supported.

Unicode Transformation Format (UTF) encodings UTF-8, UTF-16, and UTF-32 are supported.
Converting any other encoding to and from the UTF encodings is also supported if a
`codecvt<wchar_t, char>` facet is available.

The relationship between C++ character types and supported encodings is shown in the
following table:

| Type | Encodings |
| ----- | ------ |
| `char` | UTF-8; conversion from non-UTF encodings is supported by providing a `codecvt<wchar_t, char>` facet |
| `char16_t` | UTF-16 |
| `char32_t`| UFT-32 |
| `wchar_t` | UTF-8, UTF-16, or UFT-32, depending on the size of `wchar_t` |

---

© Beman Dawes 2015

This library is licensed under the Boost Software License version 1.0.
See http://www.boost.org/users/license.html
