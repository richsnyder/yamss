#ifndef YAMSS_INSPECTOR_OSTREAM_HPP
#define YAMSS_INSPECTOR_OSTREAM_HPP

#include <iostream>
#include <boost/filesystem/fstream.hpp>

namespace yamss {
namespace inspector {

class ostream : public ::std::ostream
{
public:
  typedef boost::filesystem::path path_type;

  ostream();

  ostream(const char* a_filename);

  ostream(const path_type& a_directory, const path_type& a_filename);

  virtual
  ~ostream();

  void
  close();

  void
  open(const path_type& a_directory, const path_type& a_filename);
private:
  boost::filesystem::ofstream m_file_stream;
}; // ostream class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_OSTREAM_HPP
