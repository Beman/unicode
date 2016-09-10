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
#include <type_traits>
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
    // Note: The POSIX spec says "If iconv() encounters a character in the input buffer
    // that is valid, but for which an identical character does not exist in the target
    // codeset, iconv() performs an implementation-dependent conversion on this
    // character." So the eh argument is only applied when an input sub-sequence is
    // invalid (i.e. ill-formed) for the input encoding.

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
    if (cd_ == iconv_t(-1))
      throw "open barf with errno " + std::to_string(errno);
  }

  template <class FromCharT, class ToCharT>
  recoder<FromCharT, ToCharT>::~recoder()
  {
    if (cd_ != iconv_t(-1))
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
    BOOST_ASSERT(cd_ != iconv_t(-1));  // recoder construction failed,
                                       //   yet recode has been called

    //  The POSIX iconv declaration being adapted to is:
    //    size_t iconv(iconv_t cd, const char **inbuf, size_t *inbytesleft,
    //      char **outbuf, size_t *outbytesleft);
    
    char* inbuf = const_cast<char*>(reinterpret_cast<const char*>(first));
    BOOST_ASSERT((reinterpret_cast<const char*>(last) - inbuf) >= 0);
    std::size_t inbytesleft
      = static_cast<std::size_t>(reinterpret_cast<const char*>(last) - inbuf);

    std::array<char, BOOST_UNICODE_BUFFER_SIZE> buf;

    char* outbuf; 
    std::size_t outbytesleft;
    std::size_t iconv_result;
    bool first_output_code_point = true;

    // TODO:
    // put the conversion descriptor cd_ into its initial shift state
    //outbuf = buf.data();
    //outbytesleft = buf.size();
    //iconv(cd_, 0, 0, &outbuf, &outbytesleft);

    //  loop until the entire input sequence is processed by iconv() 

    while (inbytesleft !=0)
    {
      outbuf = buf.data();
      outbytesleft = buf.size();

      //std::cout << "\nbefore iconv(), inbytesleft=" << inbytesleft
      //  << ", outbytesleft=" << outbytesleft << std::endl;

      iconv_result = iconv(cd_, &inbuf, &inbytesleft, &outbuf, &outbytesleft);

      //std::cout << "after iconv(), inbytesleft=" << inbytesleft
      //  << ", outbytesleft=" << outbytesleft
      //  << ", return=" << iconv_result << ", errno=" << errno << std::endl;

      int saved_errno = errno;  // save errno in case *result++ resets it
      auto out_it = buf.data(); // get ready to output buffer contents

      // ignore leading char16_t or char32_t byte order marker (BOM) gratuitously 
      // inserted by libstdc++
      if (first_output_code_point
        && (std::is_same<ToCharT, char32_t>::value
          || std::is_same<ToCharT, char16_t>::value)
        && outbytesleft <= (buf.size()- sizeof(ToCharT))
        && *reinterpret_cast<ToCharT*>(buf.data()) == 0xFEFF  
         )
      {
        first_output_code_point = false;
        out_it += sizeof(ToCharT);
        outbytesleft -= sizeof(ToCharT);
      }

      // output the buffer contents, if any
      for (; out_it != outbuf; out_it += sizeof(ToCharT))
        *result++ = *reinterpret_cast<ToCharT*>(out_it);

      if (iconv_result != std::size_t(-1))   // success; no error reported
      {
        BOOST_ASSERT(inbytesleft == 0);  // there was no error, so the entire input
                                         //   sequence should have been converted
        return result;
      }
      
      if (saved_errno == E2BIG)  // E2BIG: lack of space in the output buffer
        continue;

      if (saved_errno == EILSEQ // EILSEQ: invalid multibyte sequence in the input
          || saved_errno == EINVAL)
      {
        for (auto it = eh(); *it != '\0'; ++it)  // output any error message
          *result++ = *it;
         if (inbytesleft <= sizeof(FromCharT))
          break;
        // move forward one code-unit  
        inbuf += sizeof(FromCharT);
        inbytesleft -= sizeof(FromCharT);
      }
      
      else  // some totally unexpected error, so bail out
      {
        throw std::system_error(saved_errno, std::system_category(), "recoder::recode");
      }
    }
    return result;
  }


}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_RECODER_HPP
