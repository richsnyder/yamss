#ifndef YAMSS_INSPECTOR_MODES_HPP
#define YAMSS_INSPECTOR_MODES_HPP

#include <fstream>
#include <iostream>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/stream_inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class modes : public stream_inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  modes()
    : stream_inspector<T>()
    , m_brief(false)
    , m_tecplot(true)
    , m_stride(1)
    , m_filename("yamss.dat")
  {
    // empty
  }

  modes(const boost::property_tree::ptree& a_tree)
    : stream_inspector<T>()
  {
    m_brief = a_tree.find("brief") != a_tree.not_found();
    m_tecplot = a_tree.find("no_header") == a_tree.not_found();
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", "modes.dat");
  }

  virtual
  ~modes()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    this->open(m_filename);

    if (m_tecplot)
    {
      size_type n;
      size_type size = a_eom.get_size();
      this->out() << "TITLE = \"Mode History\"" << std::endl;
      this->out() << "VARIABLES = \"Iteration\", \"Time\"";
      for (n = 1; n <= size; ++n)
      {
        this->out() << boost::format(", \"Q(%1%)\"") % n;
      }
      if (!m_brief)
      {
        for (n = 1; n <= size; ++n)
        {
          this->out() << boost::format(", \"Q'(%1%)\"") % n;
        }
        for (n = 1; n <= size; ++n)
        {
          this->out() << boost::format(", \"Q''(%1%)\"") % n;
        }
        for (n = 1; n <= size; ++n)
        {
          this->out() << boost::format(", \"F(%1%)\"") % n;
        }
      }
      this->out() << std::endl << "ZONE DATAPACKING=POINT" << std::endl;
    }
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
      this->out() << boost::format("%1$10d  %2$16.9e") % n % t;
      size_type size = a_eom.get_size();
      for (n = 0; n < size; ++n)
      {
        this->out() << boost::format("  %1$16.9e") % q(n);
      }

      if (!m_brief)
      {
        const vector_type& dq = a_eom.get_velocity(0);
        const vector_type& ddq = a_eom.get_acceleration(0);
        const vector_type& f = a_eom.get_force(0);
        for (n = 0; n < size; ++n)
        {
          this->out() << boost::format("  %1$16.9e") % dq(n);
        }
        for (n = 0; n < size; ++n)
        {
          this->out() << boost::format("  %1$16.9e") % ddq(n);
        }
        for (n = 0; n < size; ++n)
        {
          this->out() << boost::format("  %1$16.9e") % f(n);
        }
      }

      this->out() << std::endl;
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

  bool m_brief;
  bool m_tecplot;
  size_type m_stride;
  std::string m_filename;
}; // modes<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_MODES_HPP
