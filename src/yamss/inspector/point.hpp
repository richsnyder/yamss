#ifndef YAMSS_INSPECTOR_POINT_HPP
#define YAMSS_INSPECTOR_POINT_HPP

#include <armadillo>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"
#include "yamss/inspector/ostream.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class point : public inspector<T>
{
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;
  typedef typename ostream::path_type path_type;

  point()
    : m_tecplot(true)
    , m_key(0)
    , m_stride(1)
    , m_filename("point.dat")
  {
    // empty
  }

  point(const boost::property_tree::ptree& a_tree)
  {
    m_tecplot = a_tree.find("no_header") == a_tree.not_found();
    m_key = a_tree.get<key_type>("node", 0);
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", "point.dat");
  }

  virtual
  ~point()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom,
             const structure_type& a_structure,
             const path_type& a_directory)
  {
    m_out.open(a_directory, m_filename);

    if (m_tecplot)
    {
      size_type n;
      size_type size = a_eom.get_size();
      m_out << "TITLE = \"Point History\"" << std::endl;
      m_out << "VARIABLES = \"Iteration\", \"Time\"";
      m_out << ", \"X\", \"Y\", \"Z\"" << std::endl;
      m_out << std::endl << "ZONE DATAPACKING=POINT" << std::endl;
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

      const auto& watched_node = a_structure.get_node(m_key);
      vector_type position = watched_node.get_displaced_position(q);
      auto x = position(0);
      auto y = position(1);
      auto z = position(2);

      m_out << boost::format("%1$10d  %2$16.9e") % n % t;
      m_out << boost::format("  %1$16.9e  %2$16.9e  %3$16.9e") % x % y % z;
      m_out << std::endl;
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    m_out.close();
  }

  virtual
  void
  get_files(std::set<path_type>& a_set) const
  {
    if (!m_filename.empty())
    {
      a_set.insert(m_filename);
    }
  }
private:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  bool m_tecplot;
  key_type m_key;
  size_type m_stride;
  std::string m_filename;

  ostream m_out;
}; // point<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_POINT_HPP
