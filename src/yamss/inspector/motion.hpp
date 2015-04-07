#ifndef YAMSS_INSPECTOR_MOTION_HPP
#define YAMSS_INSPECTOR_MOTION_HPP

#include <iostream>
#include <map>
#include <set>
#include <armadillo>
#include <boost/algorithm/string/case_conv.hpp>
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
    , m_format(TECPLOT)
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
    std::string format = a_tree.get<std::string>("format", "tecplot");
    boost::to_lower(format);
    if (format == "ply")
    {
      m_format = PLY;
    }
    else if (format == "tec" || format == "tecplot")
    {
      m_format = TECPLOT;
    }
    else
    {
      boost::format fmt("The %1% format is not supported");
      throw std::runtime_error(boost::str(fmt % format));
    }
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
      switch (m_format)
      {
        case PLY:
          write_ply(out, a_eom, a_structure);
          break;
        case TECPLOT:
          write_tecplot(out, a_eom, a_structure);
          break;
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

  void
  write_ply(std::ostream& a_out,
            const eom_type& a_eom,
            const structure_type& a_structure)
  {
    size_type i;
    vector_type x;
    const size_type n = a_eom.get_step(0);
    const value_type& t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);

    a_out << "ply" << std::endl;
    a_out << "format ascii 1.0" << std::endl;
    a_out << "comment Iteration " << n << std::endl;
    a_out << "comment Time " << boost::format("%16.9e") % t << std::endl;
    a_out << "element vertex " << a_structure.get_number_of_nodes() << std::endl;
    a_out << "property float x" << std::endl;
    a_out << "property float y" << std::endl;
    a_out << "property float z" << std::endl;
    a_out << "element face " << a_structure.get_number_of_elements() << std::endl;
    a_out << "property list uchar int vertex_indices" << std::endl;
    a_out << "end_header" << std::endl;

    std::map<key_type, size_type> indices;
    typename structure_type::const_node_iterator np;
    boost::format node_formatter("%1$16.9e %2$16.9e %3$16.9e");
    for (np = a_structure.begin_nodes(), i = 0;
         np != a_structure.end_nodes();
         ++np, ++i)
    {
      indices[np->get_key()] = i;
      x = np->get_displaced_position(q);
      a_out << node_formatter % x(0) % x(1) % x(2) << std::endl;
    }

    size_type len;
    typename structure_type::const_element_iterator ep;
    for (ep = a_structure.begin_elements();
         ep != a_structure.end_elements();
         ++ep)
    {
      len = ep->get_size();
      a_out << len;
      for (i = 0; i < len; ++i)
      {
        a_out << " " << indices[ep->get_vertex(i)];
      }
      a_out << std::endl;
    }
  }

  void
  write_tecplot(std::ostream& a_out,
                const eom_type& a_eom,
                const structure_type& a_structure)
  {
    size_type i;
    vector_type x;
    size_type elem;
    std::set<key_type>::const_iterator p;
    const size_type n = a_eom.get_step(0);
    const vector_type& q = a_eom.get_displacement(0);
    boost::format node_formatter("%1$16.9e %2$16.9e %3$16.9e");

    a_out << "TITLE = \"Structural Deformation\"" << std::endl;
    a_out << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;

    if (m_line_nodes.size() > 0)
    {
      a_out << "ZONE T=\"Iteration " << n << "\"" <<
          ", DATAPACKING=POINT" <<
          ", NODES=" << m_line_nodes.size() <<
          ", ELEMENTS=" << m_line_elements.n_rows <<
          ", ZONETYPE=FELINESEG" << std::endl;
      for (p = m_line_nodes.begin(); p != m_line_nodes.end(); ++p)
      {
        x = a_structure.get_node(*p).get_displaced_position(q);
        a_out << node_formatter % x(0) % x(1) % x(2) << std::endl;
      }
      for (elem = 0; elem < m_line_elements.n_rows; ++elem)
      {
        a_out
            << m_line_elements(elem, 0) << " "
            << m_line_elements(elem, 1) << std::endl;
      }
    }

    if (m_quad_nodes.size() > 0)
    {
      a_out << "ZONE T=\"Iteration " << n << "\"" <<
          ", DATAPACKING=POINT" <<
          ", NODES=" << m_quad_nodes.size() <<
          ", ELEMENTS=" << m_quad_elements.n_rows <<
          ", ZONETYPE=FEQUADRILATERAL" << std::endl;
      for (p = m_quad_nodes.begin(); p != m_quad_nodes.end(); ++p)
      {
        x = a_structure.get_node(*p).get_displaced_position(q);
        a_out << node_formatter % x(0) % x(1) % x(2) << std::endl;
      }
      for (elem = 0; elem < m_quad_elements.n_rows; ++elem)
      {
        a_out
            << m_quad_elements(elem, 0) << " "
            << m_quad_elements(elem, 1) << " "
            << m_quad_elements(elem, 2) << " "
            << m_quad_elements(elem, 3) << std::endl;
      }
    }
  }
private:
  enum format_type
  {
    TECPLOT,
    PLY
  };

  typedef size_t key_type;
  typedef size_t size_type;
  typedef element element_type;
  typedef arma::Col<T> vector_type;

  size_type m_stride;
  path_type m_directory;
  std::string m_filename;
  format_type m_format;

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
