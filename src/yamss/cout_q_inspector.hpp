#ifndef YAMSS_COUT_Q_INSPECTOR_HPP
#define YAMSS_COUT_Q_INSPECTOR_HPP

#include <algorithm>
#include <iostream>
#include <boost/format.hpp>
#include <eigen3/Eigen/Dense>
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
    : m_columns(1)
    , m_line_end("\n")
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
    size_type pos;
    size_type size = a_eom.get_size();
    m_columns = 1 + std::min(size_type(5), size);
    if (size >= m_columns)
    {
      m_line_end = " ...\n";
    }
    else
    {
      m_line_end = "\n";
    }
    std::cout << "Time        ";
    for (pos = 1; pos < m_columns; ++pos)
    {
      std::cout << boost::format("Mode %1%      ") % pos;
    }
    std::cout << m_line_end;
    for (pos = 0; pos < m_columns; ++pos)
    {
      std::cout << "----------  ";
    }
    std::cout << m_line_end;
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    size_type pos;
    const value_type t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);

    std::cout << boost::format("%1$10.4E") % t;
    for (pos = 0; pos < m_columns - 1; ++pos)
    {
      std::cout << boost::format("  %1$+10.3E") % q(pos);
    }
    std::cout << m_line_end;
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    // empty
  }
private:
  typedef size_t size_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> vector_type;

  size_type m_columns;
  std::string m_line_end;
}; // cout_q_inspector<T> class

} // yamss namespace

#endif // YAMSS_COUT_Q_INSPECTOR_HPP
