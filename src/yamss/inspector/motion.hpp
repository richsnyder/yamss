#ifndef YAMSS_INSPECTOR_MOTION_HPP
#define YAMSS_INSPECTOR_MOTION_HPP

#include <iostream>
#include <map>
#include <set>
#include <armadillo>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class motion : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;
  typedef typename inspector<T>::path_type path_type;

  motion()
    : m_stride(1)
    , m_directory(".")
    , m_filename("motion/snapshot.%04d.dat")
    , m_files()
  {
    // empty
  }

  motion(const boost::property_tree::ptree& a_tree)
    : m_directory(".")
    , m_files()
  {
    std::string filename = "motion/snapshot.%04d.dat";
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", filename);
  }

  virtual
  ~motion()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom,
             const structure_type& a_structure,
             const path_type& a_directory)
  {
    m_counter = 0;
    m_directory = a_directory;
    create_directory();
    process_structure(a_structure);
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    const size_type n = a_eom.get_step(0);
    if (n % m_stride == 0)
    {
      vector_type x;
      size_type elem;
      std::set<key_type>::const_iterator p;
      const vector_type& q = a_eom.get_displacement(0);

      boost::format fmt(m_filename);
      std::string filename = boost::str(fmt % m_counter++);
      m_files.insert(filename);
      boost::filesystem::ofstream out(m_directory / filename);
      out << "TITLE = \"Structural Deformation\"" << std::endl;
      out << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
      fmt.parse("%1$16.9e %2$16.9e %3$16.9e");

      if (m_line_nodes.size() > 0)
      {
        out << "ZONE T=\"Iteration " << n << "\"" <<
            ", DATAPACKING=POINT" <<
            ", NODES=" << m_line_nodes.size() <<
            ", ELEMENTS=" << m_line_elements.n_rows <<
            ", ZONETYPE=FELINESEG" << std::endl;
        for (p = m_line_nodes.begin(); p != m_line_nodes.end(); ++p)
        {
          x = a_structure.get_node(*p).get_displaced_position(q);
          out << fmt % x(0) % x(1) % x(2) << std::endl;
        }
        for (elem = 0; elem < m_line_elements.n_rows; ++elem)
        {
          out << m_line_elements(elem, 0) << " "
              << m_line_elements(elem, 1) << std::endl;
        }
      }

      if (m_quad_nodes.size() > 0)
      {
        out << "ZONE T=\"Iteration " << n << "\"" <<
            ", DATAPACKING=POINT" <<
            ", NODES=" << m_quad_nodes.size() <<
            ", ELEMENTS=" << m_quad_elements.n_rows <<
            ", ZONETYPE=FEQUADRILATERAL" << std::endl;
        for (p = m_quad_nodes.begin(); p != m_quad_nodes.end(); ++p)
        {
          x = a_structure.get_node(*p).get_displaced_position(q);
          out << fmt % x(0) % x(1) % x(2) << std::endl;
        }
        for (elem = 0; elem < m_quad_elements.n_rows; ++elem)
        {
          out << m_quad_elements(elem, 0) << " "
              << m_quad_elements(elem, 1) << " "
              << m_quad_elements(elem, 2) << " "
              << m_quad_elements(elem, 3) << std::endl;
        }
      }
      out.close();
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    m_line_nodes.clear();
    m_quad_nodes.clear();
    m_line_elements.reset();
    m_quad_elements.reset();
  }

  virtual
  void
  get_files(std::set<path_type>& a_set) const
  {
    a_set.insert(m_files.begin(), m_files.end());
  }
protected:
  void
  create_directory()
  {
    boost::format fmt(m_filename);
    boost::filesystem::path path = m_directory / boost::str(fmt % 0);
    bool status = boost::filesystem::create_directory(path.parent_path());
  }

  void
  process_structure(const structure_type& a_structure)
  {
    size_type n;
    size_type line_count = 0;
    size_type quad_count = 0;
    element_type::shape_type shape;
    std::set<key_type>::const_iterator q;
    std::map<key_type, size_type> line_indices;
    std::map<key_type, size_type> quad_indices;
    typename structure_type::const_element_iterator p;

    for (p = a_structure.begin_elements();
         p != a_structure.end_elements();
         ++p)
    {
      shape = p->get_shape();
      if (shape == element_type::LINE)
      {
        m_line_nodes.insert(p->get_vertex(0));
        m_line_nodes.insert(p->get_vertex(1));
        ++line_count;
      }
      else if (shape == element_type::TRIANGLE)
      {
        m_quad_nodes.insert(p->get_vertex(0));
        m_quad_nodes.insert(p->get_vertex(1));
        m_quad_nodes.insert(p->get_vertex(2));
        ++quad_count;
      }
      else if (shape == element_type::QUADRILATERAL)
      {
        m_quad_nodes.insert(p->get_vertex(0));
        m_quad_nodes.insert(p->get_vertex(1));
        m_quad_nodes.insert(p->get_vertex(2));
        m_quad_nodes.insert(p->get_vertex(3));
        ++quad_count;
      }
    }

    for (q = m_line_nodes.begin(), n = 0; q != m_line_nodes.end(); ++q, ++n)
    {
      line_indices[*q] = n;
    }
    for (q = m_quad_nodes.begin(), n = 0; q != m_quad_nodes.end(); ++q, ++n)
    {
      quad_indices[*q] = n;
    }

    m_line_elements.resize(line_count, 2);
    m_quad_elements.resize(quad_count, 4);
    line_count = 0;
    quad_count = 0;
    for (p = a_structure.begin_elements();
         p != a_structure.end_elements();
         ++p)
    {
      shape = p->get_shape();
      if (shape == element_type::LINE)
      {
        m_line_elements(line_count, 0) = line_indices[p->get_vertex(0)];
        m_line_elements(line_count, 1) = line_indices[p->get_vertex(1)];
        ++line_count;
      }
      else if (shape == element_type::TRIANGLE)
      {
        m_quad_elements(quad_count, 0) = quad_indices[p->get_vertex(0)];
        m_quad_elements(quad_count, 1) = quad_indices[p->get_vertex(1)];
        m_quad_elements(quad_count, 2) = quad_indices[p->get_vertex(2)];
        m_quad_elements(quad_count, 3) = quad_indices[p->get_vertex(2)];
        ++quad_count;
      }
      else if (shape == element_type::QUADRILATERAL)
      {
        m_quad_elements(quad_count, 0) = quad_indices[p->get_vertex(0)];
        m_quad_elements(quad_count, 1) = quad_indices[p->get_vertex(1)];
        m_quad_elements(quad_count, 2) = quad_indices[p->get_vertex(2)];
        m_quad_elements(quad_count, 3) = quad_indices[p->get_vertex(3)];
        ++quad_count;
      }
    }
  }
private:
  typedef size_t key_type;
  typedef size_t size_type;
  typedef element element_type;
  typedef arma::Col<T> vector_type;

  size_type m_stride;
  path_type m_directory;
  std::string m_filename;
  std::set<path_type> m_files;

  size_type m_counter;
  std::set<key_type> m_line_nodes;
  std::set<key_type> m_quad_nodes;
  arma::Mat<size_type> m_line_elements;
  arma::Mat<size_type> m_quad_elements;
}; // motion<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_MOTION_HPP
