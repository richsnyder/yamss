#include "yamss/inspector/ostream.hpp"

namespace yamss {
namespace inspector {

ostream::ostream()
  : m_file_stream()
  , ::std::ostream(std::cout.rdbuf())
{
  // empty
}

ostream::ostream(const char* a_filename)
  : m_file_stream(a_filename)
  , ::std::ostream(m_file_stream.rdbuf())
{
  // empty
}

ostream::ostream(const path_type& a_directory, const path_type& a_filename)
  : ::std::ostream(NULL)
{
  open(a_directory, a_filename);
}

ostream::~ostream()
{
  // close();
}

void
ostream::close()
{
  m_file_stream.close();
}

void
ostream::open(const path_type& a_directory, const path_type& a_filename)
{
  if (a_filename.empty())
  {
    init(std::cout.rdbuf());
  }
  else
  {
    m_file_stream.open(a_directory / a_filename);
    init(m_file_stream.rdbuf());
  }
}

} // inspector namespace
} // yamss namespace
