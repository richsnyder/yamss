#ifndef YAMSS_INSPECTOR_SUMMARY_HPP
#define YAMSS_INSPECTOR_SUMMARY_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/stream_inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class summary : public stream_inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  summary()
    : stream_inspector<T>()
    , m_limit(3)
    , m_stride(1)
    , m_filename()
    , m_last(0)
    , m_more()
  {
    // empty
  }

  summary(const boost::property_tree::ptree& a_tree)
    : stream_inspector<T>()
    , m_last(0)
    , m_more()
  {
    m_limit = a_tree.get<size_type>("limit", 3);
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", "");
  }

  virtual
  ~summary()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    this->open(m_filename);

    size_type size = a_eom.get_size();
    if (size > m_limit)
    {
      m_last = m_limit;
      m_more = ", ...";
    }
    else
    {
      m_last = size;
      m_more = "";
    }
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    const size_type n = a_eom.get_step(0);
    if (n % m_stride == 0)
    {
      const value_type t = a_eom.get_time(0);
      const vector_type& q = a_eom.get_displacement(0);
      this->out() << boost::format("N = %1$6d   T = %2$10.4E   Q = { ") % n % t;
      if (m_last > 0)
      {
        this->out() << boost::format("%1$+10.3E") % q(0);
      }
      for (size_type i = 1; i < m_last; ++i)
      {
        this->out() << boost::format(", %1$+10.3E") % q(i);
      }
      this->out() << m_more << " }" << std::endl;
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    this->close();
  }
private:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  size_type m_limit;
  size_type m_stride;
  std::string m_filename;

  size_type m_last;
  std::string m_more;
}; // summary<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_SUMMARY_HPP
