//  boost/unicode/detail/hex_string.hpp  -----------------------------------------------//

//  � Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_UNICODE_HEX_STRING_HPP
#define BOOST_UNICODE_HEX_STRING_HPP

#include <boost/endian/conversion.hpp>

namespace boost { namespace unicode { namespace detail {

  template <class T>
  std::string to_hex(T x)
  {
    const char hex[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    std::string tmp;
    typename underlying<T>::type big_x
      = boost::endian::native_to_big(static_cast<typename underlying<T>::type>(x));

    const unsigned char* p = reinterpret_cast<const unsigned char*>(&big_x);
    const unsigned char* e = p + sizeof(T);

    for (; p < e; ++p)
    {
      tmp += hex[*p >> 4];    // high-order nibble
      tmp += hex[*p & 0x0f];  // low-order nibble
    }
    return tmp;
  }

  template <class String>
  std::string hex_string(const String& s)
  {
    std::string tmp;
    for (auto x : s)
    {
      tmp += " 0x";
      tmp += to_hex(x);
    }
    return tmp;
  }
}}}  // namespace boost::unicode::detail

#endif  // BOOST_UNICODE_HEX_STRING_HPP
