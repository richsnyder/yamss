#ifndef YAMSS_INSPECTOR_TECPLOT_HPP
#define YAMSS_INSPECTOR_TECPLOT_HPP

#include <fstream>
#include <iostream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class tecplot : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  tecplot()
    : m_brief(false)
    , m_stride(1)
    , m_filename("yamss.dat")
    , m_out(NULL)
  {
    // empty
  }

  tecplot(const boost::property_tree::ptree& a_tree)
    : m_out(NULL)
  {
    m_brief = a_tree.find("brief") != a_tree.not_found();
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", "yamss.dat");
  }

  virtual
  ~tecplot()
  {
    close();
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    open();

    *m_out << "TITLE = \"Mode History\"" << std::endl;
    *m_out << "VARIABLES = \"Iteration\", \"Time\"";
    size_type n;
    size_type size = a_eom.get_size();
    for (n = 1; n <= size; ++n)
    {
      *m_out << boost::format(", \"Q(%1%)\"") % n;
    }
    if (!m_brief)
    {
      for (n = 1; n <= size; ++n)
      {
        *m_out << boost::format(", \"Q'(%1%)\"") % n;
      }
      for (n = 1; n <= size; ++n)
      {
        *m_out << boost::format(", \"Q''(%1%)\"") % n;
      }
      for (n = 1; n <= size; ++n)
      {
        *m_out << boost::format(", \"F(%1%)\"") % n;
      }
    }
    *m_out << std::endl << "ZONE DATAPACKING=POINT" << std::endl;
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    size_type n = a_eom.get_step(0);
    if (n % m_stride == 0)
    {
      const value_type t = a_eom.get_time(0);
      const vector_type& q = a_eom.get_displacement(0);
      *m_out << boost::format("%1$10d  %2$16.9e") % n % t;
      size_type size = a_eom.get_size();
      for (n = 0; n < size; ++n)
      {
        *m_out << boost::format("  %1$16.9e") % q(n);
      }

      if (!m_brief)
      {
        const vector_type& dq = a_eom.get_velocity(0);
        const vector_type& ddq = a_eom.get_acceleration(0);
        const vector_type& f = a_eom.get_force(0);
        for (n = 0; n < size; ++n)
        {
          *m_out << boost::format("  %1$16.9e") % dq(n);
        }
        for (n = 0; n < size; ++n)
        {
          *m_out << boost::format("  %1$16.9e") % ddq(n);
        }
        for (n = 0; n < size; ++n)
        {
          *m_out << boost::format("  %1$16.9e") % f(n);
        }
      }

      *m_out << std::endl;
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    close();
  }
protected:
  void
  open()
  {
    if (m_out == NULL)
    {
      if (m_filename.empty())
      {
        m_out = &std::cout;
      }
      else
      {
        std::ofstream* out = new std::ofstream(m_filename.c_str());
        m_out = dynamic_cast<std::ostream*>(out);
      }
    }
  }

  void
  close()
  {
    if (m_out != NULL)
    {
      if (!m_filename.empty())
      {
        std::ofstream* out = dynamic_cast<std::ofstream*>(m_out);
        if (out->is_open())
        {
          out->close();
        }
        delete m_out;
      }
      m_out = NULL;
    }
  }
private:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  bool m_brief;
  size_type m_stride;
  std::string m_filename;

  std::ostream* m_out;
}; // tecplot<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_TECPLOT_HPP
