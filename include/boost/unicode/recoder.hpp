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
    using from_value_type = FromT;
    using to_value_type = ToT;

    recoder(boost::string_view from_name, boost::string_view to_name);
    ~recoder();

    const std::string& from_name() const noexcept;
    const std::string& to_name() const noexcept;

    template <class Error = ufffd<ToCharT>>
    OutputIterator recode(const FromT* first, const FromT* last, OutputIterator result,
      Error eh = Error());

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

  recoder::recoder(boost::string_view from_name, boost::string_view to_name)
    : from_name_(from), to_name(to)_, cd_(iconv(to, from))
  {
    if (cd_ == (iconv_t)-1)
      throw "open barf with errno " + std::to_string(errno);
  }

  ~recoder::recoder()
  {
    if (cd_ != (iconv_t)-1)
      iconv_close(cd_);
  }

  inline const std::string& recoder::from_name() const noexcept { return from_name_; }
  inline const std::string& recoder::to_name() const noexcept { return to_name_; }

  template <class Error>
  OutputIterator recoder::recode(const FromT* first, const FromT* last, OutputIterator result,
    Error eh)
  {
    //  The POSIX iconv declaration we must adapt to is:
    //    size_t iconv(iconv_t cd, const char **inbuf, size_t *inbytesleft,
    //      char **outbuf, size_t *outbytesleft);
    
    const char* inbuf = static_cast<const char*>(first);
    std::size_t inbytesleft = static_cast<const char*>(last) - inbuf;

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
          *result++ = *static_cast<ToCharT*>(cur);
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



};

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_RECODER_HPP
