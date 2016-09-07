#include <boost/unicode/recoder.hpp>
#include <iterator>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/unicode/detail/hex_string.hpp>

using boost::unicode::detail::hex_string;

int main()
{
  boost::unicode::recoder<char, char32_t> rcdr("UTF-8", "UTF-32");

  std::u32string tmp;
  std::string test(u8"abc");
  std::u32string expected(U"abc");

  rcdr.recode(test.c_str(), test.c_str()+test.size(), std::back_inserter(tmp));

  BOOST_TEST(tmp == expected);
  std::cout << "result:" << hex_string(tmp) << std::endl;
  std::cout << "expect:" << hex_string(expected) << std::endl;

  return boost::report_errors();
}