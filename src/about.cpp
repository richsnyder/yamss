#include "yamss/about.hpp"

namespace yamss {

std::string
about::author()
{
  return "Richard Snyder";
}

std::string
about::copyright()
{
  return "Copyright (c) 2015, " + author();
}

std::vector<std::string>
about::credits()
{
  std::vector<std::string> credits_;
  credits_.push_back("Richard Snyder");
  return credits_;
}

std::string
about::description()
{
  return name() + ", version " + version_string();
}

std::string
about::email()
{
  return "richard.snyder.13@us.af.mil";
}

std::string
about::legal()
{
  return description() + "\n\n" + copyright()
      + "\nUse, modification, and distribution is subject to the BSD License."
      + "\nSee the accompanying file LICENSE.md for the full license."
      + "\nTerms can also be found at http://creativecommons.org/licenses/BSD/.";
}

std::string
about::license_type()
{
  return "BSD";
}

std::string
about::name()
{
  return "yamss";
}

int
about::version()
{
  return ((YAMSS_VERSION_MAJOR) << 16)
      | ((YAMSS_VERSION_MINOR) << 8)
      | (YAMSS_VERSION_PATCH);
}

std::string
about::version_string()
{
  return YAMSS_VERSION;
}

} // yamss namespace
