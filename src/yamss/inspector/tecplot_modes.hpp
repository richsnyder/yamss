#ifndef YAMSS_INSPECTOR_TECPLOT_MODES_HPP
#define YAMSS_INSPECTOR_TECPLOT_MODES_HPP

#include <fstream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class tecplot_modes : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  tecplot_modes()
    : m_out()
    , m_filename("yamss.modes.dat")
  {
    // empty
  }

  tecplot_modes(const boost::property_tree::ptree& a_tree)
    : m_out()
  {
    m_filename = a_tree.get<std::string>("filename", "yamss.modes.dat");
  }

  virtual
  ~tecplot_modes()
  {
    if (m_out.is_open())
    {
      m_out.close();
    }
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    if (!m_out.is_open())
    {
      m_out.open(m_filename.c_str());
    }
    m_out << "TITLE = \"Mode History\"" << std::endl;
    m_out << "VARIABLES = \"Iteration\", \"Time\"";
    size_type n;
    size_type size = a_eom.get_size();
    for (n = 1; n <= size; ++n)
    {
      m_out << boost::format(" \"Q(%1%)\"") % n;
    }
    for (n = 1; n <= size; ++n)
    {
      m_out << boost::format(" \"Q'(%1%)\"") % n;
    }
    for (n = 1; n <= size; ++n)
    {
      m_out << boost::format(" \"Q''(%1%)\"") % n;
    }
    for (n = 1; n <= size; ++n)
    {
      m_out << boost::format(" \"F(%1%)\"") % n;
    }
    m_out << std::endl << "ZONE DATAPACKING=POINT" << std::endl;
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    size_type n = a_eom.get_step(0);
    const value_type t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);
    const vector_type& dq = a_eom.get_velocity(0);
    const vector_type& ddq = a_eom.get_acceleration(0);
    const vector_type& f = a_eom.get_force(0);

    m_out << boost::format("%1$10d  %2$16.9e") % n % t;
    size_type size = a_eom.get_size();
    for (n = 0; n < size; ++n)
    {
      m_out << boost::format("  %1$16.9e") % q(n);
    }
    for (n = 0; n < size; ++n)
    {
      m_out << boost::format("  %1$16.9e") % dq(n);
    }
    for (n = 0; n < size; ++n)
    {
      m_out << boost::format("  %1$16.9e") % ddq(n);
    }
    for (n = 0; n < size; ++n)
    {
      m_out << boost::format("  %1$16.9e") % f(n);
    }
    m_out << std::endl;
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    if (m_out.is_open())
    {
      m_out.close();
    }
  }
private:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  std::ofstream m_out;
  std::string m_filename;
}; // tecplot_modes<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_TECPLOT_MODES_HPP
