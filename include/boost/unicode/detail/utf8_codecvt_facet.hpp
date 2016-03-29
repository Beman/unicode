// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// NOTE WELL: This header is used for testing only; it is not used by the implementation
// of the library itself, and should not be used for production code

#ifndef BOOST_UNICODE_UTF8_CODECVT_FACET_HPP
#define BOOST_UNICODE_UTF8_CODECVT_FACET_HPP

#define BOOST_UTF8_BEGIN_NAMESPACE \
     namespace boost { namespace unicode { namespace detail {

#define BOOST_UTF8_END_NAMESPACE }}}
#define BOOST_UTF8_DECL

#include <boost/detail/utf8_codecvt_facet.hpp>

#undef BOOST_UTF8_BEGIN_NAMESPACE
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL

#endif
