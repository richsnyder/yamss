#ifndef YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP
#define YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP

#include <fstream>
#include <iostream>
#include <streambuf>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T>
class stream_inspector : public inspector<T>
{
protected:
  stream_inspector()
    : m_buffer(NULL)
    , m_output(NULL)
    , m_file()
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
  open(const std::string& a_filename)
  {
    if (a_filename.empty())
    {
      m_buffer = std::cout.rdbuf();
    }
    else
    {
      m_file.open(a_filename.c_str());
      m_buffer = m_file.rdbuf();
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
  std::ofstream m_file;
}; // stream_inspector<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_STREAM_INSPECTOR_HPP
