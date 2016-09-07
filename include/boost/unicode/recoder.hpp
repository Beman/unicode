//  boost/unicode/recoder.hpp  ---------------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  Adapter to turn the POSIX iconv.h interface into an STL-like algoithm interface     //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#if !defined(BOOST_UNICODE_RECODER_HPP)
#define BOOST_UNICODE_RECODER_HPP

#include <boost/unicode/string_encoding.hpp>
#include <iconv.h>
#include <boost/assert.hpp>

//--------------------------------------------------------------------------------------//
//                                    Synopsis                                          //
//--------------------------------------------------------------------------------------//
namespace boost
{
namespace unicode
{

  template <class FromCharT, class ToCharT>
  class recoder
  {
  public:
    using from_value_type = FromCharT;
    using to_value_type = ToCharT;

    recoder(const std::string& from_name, const std::string& to_name);
    ~recoder();

    const std::string& from_name() const noexcept;
    const std::string& to_name() const noexcept;

    template <class OutputIterator, class Error = ufffd<ToCharT>>
    OutputIterator recode(const FromCharT* first, const FromCharT* last,
      OutputIterator result, Error eh = Error());

  private:  // exposition only

    std::string from_name_;   // from encoding name
    std::string to_name_;     // to encoding name
    iconv_t     cd_;          // iconv conversion descriptor
    
  };

}  // namespace unicode
}  // namespace boost

//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//
namespace boost
{
namespace unicode
{
  template <class FromCharT, class ToCharT>
  recoder<FromCharT, ToCharT>::recoder(const std::string& from_name,
    const std::string& to_name)
    : from_name_(from_name), to_name_(to_name), cd_(iconv_open(
        to_name.c_str(), from_name.c_str()))
  {
    if (cd_ == (iconv_t)-1)
      throw "open barf with errno " + std::to_string(errno);
  }

  template <class FromCharT, class ToCharT>
  recoder<FromCharT, ToCharT>::~recoder()
  {
    if (cd_ != (iconv_t)-1)
      iconv_close(cd_);
  }

  template <class FromCharT, class ToCharT>
  inline const std::string& recoder<FromCharT, ToCharT>::from_name() const noexcept
    { return from_name_; }

  template <class FromCharT, class ToCharT>
  inline const std::string& recoder<FromCharT, ToCharT>::to_name() const noexcept
    { return to_name_; }

  template <class FromCharT, class ToCharT>
  template <class OutputIterator, class Error>
  OutputIterator recoder<FromCharT, ToCharT>::recode(
    const FromCharT* first, const FromCharT* last, OutputIterator result, Error eh)
  {
    //  The POSIX iconv declaration being adapted to is:
    //    size_t iconv(iconv_t cd, const char **inbuf, size_t *inbytesleft,
    //      char **outbuf, size_t *outbytesleft);
    
    char* inbuf = const_cast<char*>(reinterpret_cast<const char*>(first));
    BOOST_ASSERT((reinterpret_cast<const char*>(last) - inbuf) >= 0);
    std::size_t inbytesleft = static_cast<std::size_t>(reinterpret_cast<const char*>(last) - inbuf);

    std::array<char, 8> buf;  // TODO: macro that reduces size for stress testing

    char* outbuf; 
    std::size_t outbytesleft;
    std::size_t iconv_result;

    // TODO: initialization call to iconv()

    //  loop until the entire input sequence is processed by the codecvt facet 

    while (inbytesleft !=0)
    {
      outbuf = buf.data();
      outbytesleft = buf.size();

      BOOST_ASSERT_MSG(cd_ != (iconv_t)-1, "recoder::recode logic error");

      iconv_result = iconv(cd_, &inbuf, &inbytesleft, &outbuf, &outbytesleft);

      if (inbytesleft == 0)           // success; the input string has been converted
      {
        for (auto cur = buf.data(); cur <= outbuf; cur += sizeof(ToCharT))
          *result++ = *reinterpret_cast<ToCharT*>(cur);
      }
      //else if (ccvt_result == std::codecvt_base::error)
      //{
      //  for (; to != to_next; ++to)
      //    *result++ = *to;
      //  for (auto it = eh(); *it != '\0'; ++it)
      //    *result++ = *to;
      //  from = from_next + 1;  // bypass error
      //}
      //else  // ccvt_result == std::codecvt_base::partial
      //{
      //  if (to_next == buf.data())
      //  {
      //    for (auto it = eh(); *it != '\0'; ++it)
      //      *result++ = *to;
      //    from = from_end;
      //  }
      //  else
      //  {
      //    // eliminate the possibility that buf does not have enough room
      //    for (; to != to_next; ++to)
      //      *result++ = *to;
      //    from = from_next;
      //  }
      //}
    }
    return result;
  }


}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_RECODER_HPP
