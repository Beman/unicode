//  clean_utf8.cpp  --------------------------------------------------------------------//

//  © Copyright Beman Dawes 2013, 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <fstream>
#include <string>
#include "../include/boost/unicode/string_encoding.hpp"
#include <boost/detail/lightweight_main.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;
using boost::unicode::utf8;

namespace
{
  string input_path;
  std::ifstream in;
  string line;
}

int cpp_main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cerr << "Invoke: clean_utf8 <input-path>"
         << endl;
    return 1;
  }

  input_path = argv[1];
  in.open(input_path);
  if (!in)
  {
    cerr << "Could not open " << input_path << endl;
    return 1;
  }

  //  read each line

  while (in.good())
  {
    getline(in, line);
    string cleaned_line = boost::unicode::to_string<utf8, utf8>(line);
    cout << cleaned_line << '\n';
  }

  if (!in.eof())
  {
    cerr << "Error reading " << input_path << endl;
    return 1;
  }

  return 0;
}