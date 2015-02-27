#ifndef YAMSS_INSPECTOR_TECPLOT_Q_HPP
#define YAMSS_INSPECTOR_TECPLOT_Q_HPP

#include <fstream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class tecplot_q : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  tecplot_q()
    : m_out()
    , m_filename("yamss.q.dat")
  {
    // empty
  }

  tecplot_q(const boost::property_tree::ptree& a_tree)
    : m_out()
  {
    m_filename = a_tree.get<std::string>("filename", "yamss.q.dat");
  }

  virtual
  ~tecplot_q()
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
    m_out << "TITLE = \"Modal Coordinates\"" << std::endl;
    m_out << "VARIABLES = \"Time\"";
    for (size_type n = 0; n < a_eom.get_size(); ++n)
    {
      m_out << boost::format(" \"Mode %1%\"") % (n + 1);
    }
    m_out << std::endl << "ZONE DATAPACKING=POINT" << std::endl;
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    const value_type t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);

    m_out << boost::format("%1$16.9e") % t;
    for (size_type n = 0; n < q.size(); ++n)
    {
      m_out << boost::format("  %1$16.9e") % q(n);
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
}; // tecplot_q<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_TECPLOT_Q_HPP