#ifndef YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP
#define YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP

#include <iostream>
#include <streambuf>
#include <boost/filesystem/fstream.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T>
class stream_inspector : public inspector<T>
{
public:
  typedef typename inspector<T>::path_type path_type;

  virtual
  void
  get_files(std::set<path_type>& a_set) const
  {
    a_set.insert(m_files.begin(), m_files.end());
  }
protected:
  stream_inspector()
    : m_buffer(NULL)
    , m_output(NULL)
    , m_file()
    , m_files()
  {
    // empty
  }

  virtual
  ~stream_inspector()
  {
    close();
  }

  std::ostream&
  out()
  {
    return *m_output;
  }

  void
  open(const path_type& a_directory, const std::string& a_filename)
  {
    if (a_filename.empty())
    {
      m_buffer = std::cout.rdbuf();
    }
    else
    {
      path_type path = a_directory / a_filename;
      m_file.open(path);
      m_buffer = m_file.rdbuf();
      m_files.insert(a_filename);
    }
    m_output = new std::ostream(m_buffer);
  }

  void
  close()
  {
    if (m_output)
    {
      delete m_output;
      m_output = NULL;
    }
    m_buffer = NULL;
    m_file.close();
  }
private:
  std::streambuf* m_buffer;
  std::ostream* m_output;
  boost::filesystem::ofstream m_file;
  std::set<path_type> m_files;
}; // stream_inspector<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP
