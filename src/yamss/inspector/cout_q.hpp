#ifndef YAMSS_INSPECTOR_COUT_Q_HPP
#define YAMSS_INSPECTOR_COUT_Q_HPP

#include <algorithm>
#include <iostream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class cout_q : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  cout_q()
    : m_last(0)
    , m_more()
  {
    // empty
  }

  cout_q(const boost::property_tree::ptree& a_tree)
    : m_last(0)
    , m_more()
  {
    // empty
  }

  virtual
  ~cout_q()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    size_type size = a_eom.get_size();
    if (size > 3)
    {
      m_last = 3;
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
    size_type n = a_eom.get_step(0);
    const value_type t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);

    std::cout << boost::format("N = %1$6d   T = %2$10.4E   Q = { ") % n % t;
    if (m_last > 0)
    {
      std::cout << boost::format("%1$+10.3E") % q(0);
    }
    for (n = 1; n < m_last; ++n)
    {
      std::cout << boost::format(", %1$+10.3E") % q(n);
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
}; // cout_q<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_COUT_Q_HPP
