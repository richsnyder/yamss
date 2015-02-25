#ifndef YAMSS_ABOUT_HPP
#define YAMSS_ABOUT_HPP

#include <string>
#include <vector>
#include <boost/format.hpp>
#include "yamss/version.hpp"

namespace yamss {

class about
{
public:
  static
  std::string
  author();

  static
  std::string
  copyright();

  static
  std::vector<std::string>
  credits();

  static
  std::string
  description();

  static
  std::string
  email();

  static
  std::string
  legal();

  static
  std::string
  license_type();

  static
  std::string
  name();

  static
  int
  version();

  static
  std::string
  version_string();
}; // about class

} // yamss namespace

#endif // YAMSS_ABOUT_HPP
