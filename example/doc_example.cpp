//  unicode/example/doc_examples.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/unicode/string_encoding.hpp>
#include <string>
#include <locale>
#include <cvt/big5>  // vendor supplied
#include <cvt/sjis>  // vendor supplied

using namespace boost::unicode;
using namespace std;

string sjisstr() { string s; /*load s*/ return s; }
string big5str() { string s; /*load s*/ return s; }

template <class CharT> struct barf;
template <> struct barf<char>
  {const char* operator()() const {throw "barf"; return "";}};
template <> struct barf<char16_t>
  {const char16_t* operator()() const {throw "barf"; return u"";}};
template <> struct barf<char32_t>
  {const char32_t* operator()() const {throw "barf"; return U"";}};
template <> struct barf<wchar_t>
  {const wchar_t* operator()() const {throw "barf"; return L"";}};


int main()
{
  
  string     locstr("abc123...");       // narrow encoding known to std::locale()
  string     u8str(u8"abc123$€𐐷𤭢..."); // UTF-8 encoded
  u16string u16str(u"abc123$€𐐷𤭢...");  // UTF-16 encoded
  u32string u32str(U"abc123$€𐐷𤭢...");  // UTF-32 encoding
  wstring     wstr(L"abc123$€𐐷𤭢...");  // implementation defined wide encoding

  string  s;
  wstring ws;
  u16string u16s;
  u32string u32s;

  stdext::cvt::codecvt_big5<wchar_t> big5;  // vendor supplied Big-5 facet
  stdext::cvt::codecvt_sjis<wchar_t> sjis;  // vendor supplied Shift-JIS facet

  auto loc = std::locale();
  auto& loc_ccvt(std::use_facet<ccvt_type>(loc));

  u16s = to_string<utf16, utf8>(u8str);                 // UTF-16 from UTF-8
  ws = to_string<wide, narrow>(locstr, loc_ccvt);       // wide from narrow
  u32s = to_string<utf32, narrow>(sjisstr(), sjis);     // UTF-32 from Shift-JIS
  s = to_string<narrow, utf32>(u32str, big5);           // Big-5 from UTF-32
  s = to_string<narrow, narrow>(big5str(), big5, sjis); // Shift-JIS from Big-5

  s = to_string<utf8, utf8>(u8str);  // replace ill-formed characters
                                     //   with u8"\uFFFD"
  s = to_string<utf8, utf16>(u16str, barf<char>());  // throw if ill formed

  //u16s = to_string<utf16, narrow>(locstr);  // error; missing codecvt arg
  //u16s = to_string<utf16, narrow>(locstr, big5, sjis);  // error; extra codecvt arg
}
