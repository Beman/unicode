# unicode
Unicode Support Library

**This library is under development. It is unstable, buggy, and not suitable for production use.**

This library currently consists of a single header, conversion.hpp, that provides low-level
algorithms and higher-level convenience functions that convert between character
encodings. The plan is to next add one or more additional headers providing low-level
algorithms and higher-level convenience
helpers for utf-8, 16, and 32 strings and string iterators. Example might include
utf_next(), utf_prior(), utf_advance(), utf_size(). The longer range plan is to add
sufficient additional support to ensure easy interoperability with the ICU - International
Components for Unicode - library. The plan is to complement ICU rather than compete with
it.

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

The library is currently implemented as a header only library.

---

© Beman Dawes 2015

This library is licensed under the Boost Software License version 1.0.
See http://www.boost.org/users/license.html
