#ifndef YAMSS_CLP_HPP
#define YAMSS_CLP_HPP

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "yamss/about.hpp"

namespace yamss {

class clp
{
public:
  clp(int argc, char* argv[]);

  bool
  good() const;

  bool
  complex_mode() const;

  bool
  has_input_filename() const;

  std::string
  input_filename() const;

  void
  legal(std::ostream& a_out) const;

  void
  usage(std::ostream& a_out) const;

  void
  version(std::ostream& a_out) const;
protected:
  void
  initialize();

  void
  parse(int argc, char* argv[]);

  void
  report(std::ostream& a_out) const;
private:
  std::string m_program_name;
  std::string m_input_filename;
  boost::program_options::options_description m_argument_options;
  boost::program_options::options_description m_visible_options;
  boost::program_options::positional_options_description m_positional;
  boost::program_options::variables_map m_variables_map;
}; // clp class

} // yamss namespace

#endif // YAMSS_CLP_HPP
