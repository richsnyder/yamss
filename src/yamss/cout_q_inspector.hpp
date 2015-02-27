#ifndef YAMSS_COUT_Q_INSPECTOR_HPP
#define YAMSS_COUT_Q_INSPECTOR_HPP

#include <algorithm>
#include <iostream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector.hpp"

namespace yamss {

template <typename T = double>
class cout_q_inspector : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  cout_q_inspector()
    : m_last(0)
    , m_more()
  {
    // empty
  }

  cout_q_inspector(const boost::property_tree::ptree& a_tree)
    : m_last(0)
    , m_more()
  {
    // empty
  }

  virtual
  ~cout_q_inspector()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    size_type size = a_eom.get_size();
    if (size > 4)
    {
      m_last = 4;
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
    size_type pos;
    const value_type t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);

    std::cout << boost::format("T = %1$10.4E   Q = { ") % t;
    if (m_last > 0)
    {
      std::cout << boost::format("%1$+10.3E") % q(0);
    }
    for (pos = 1; pos < m_last; ++pos)
    {
      std::cout << boost::format(", %1$+10.3E") % q(pos);
    }
    std::cout << m_more << " }" << std::endl;
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    // empty
  }
private:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  size_type m_last;
  std::string m_more;
}; // cout_q_inspector<T> class

} // yamss namespace

#endif // YAMSS_COUT_Q_INSPECTOR_HPP
