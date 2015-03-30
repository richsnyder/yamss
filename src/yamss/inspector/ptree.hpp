#ifndef YAMSS_INSPECTOR_PTREE_HPP
#define YAMSS_INSPECTOR_PTREE_HPP

#include <iterator>
#include <string>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "yamss/inspector/inspector.hpp"

namespace yamss {
namespace inspector {

template <typename T = double>
class ptree : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;
  typedef typename inspector<T>::path_type path_type;

  ptree()
    : m_stride(1)
    , m_filename("yamss.xml")
    , m_directory()
    , m_document()
  {
    // empty
  }

  ptree(const boost::property_tree::ptree& a_tree)
    : m_directory()
    , m_document()
  {
    m_stride = a_tree.get<size_type>("stride", 1);
    m_filename = a_tree.get<std::string>("filename", "yamss.xml");
  }

  virtual
  ~ptree()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom,
             const structure_type& a_structure,
             const path_type& a_directory)
  {
    m_document.clear();
    m_directory = a_directory;
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    const size_type n = a_eom.get_step(0);
    if (n % m_stride == 0)
    {
      const value_type& t = a_eom.get_time(0);
      const vector_type& q = a_eom.get_displacement(0);
      const vector_type& dq = a_eom.get_velocity(0);
      const vector_type& ddq = a_eom.get_acceleration(0);
      const vector_type& f = a_eom.get_force(0);

      boost::property_tree::ptree tree_f;
      boost::property_tree::ptree tree_q;
      boost::property_tree::ptree tree_dq;
      boost::property_tree::ptree tree_ddq;
      boost::property_tree::ptree timestep;

      for (size_type i = 0; i < a_eom.get_size(); ++i)
      {
        tree_f.add("mode", as_string(f(i)));
        tree_q.add("mode", as_string(q(i)));
        tree_dq.add("mode", as_string(dq(i)));
        tree_ddq.add("mode", as_string(ddq(i)));
      }
      timestep.put("iteration", n);
      timestep.put("time", as_string(t));
      timestep.add_child("displacement", tree_q);
      timestep.add_child("velocity", tree_dq);
      timestep.add_child("acceleration", tree_ddq);
      timestep.add_child("force", tree_f);
      m_document.add_child("yamss_output.timesteps.timestep", timestep);
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    std::string filename = (m_directory / m_filename).native();
    std::string ext = boost::filesystem::extension(m_filename);
    boost::to_lower(ext);
    if (ext == ".info")
    {
      boost::property_tree::write_info(filename, m_document);
    }
    else if (ext == ".json")
    {
      boost::property_tree::write_json(filename, m_document);
    }
    else
    {
      boost::property_tree::write_xml(filename, m_document);
    }
  }

  virtual
  void
  get_files(std::set<path_type>& a_set) const
  {
    a_set.insert(m_filename);
  }
protected:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  std::string
  as_string(const value_type& a_value)
  {
    return boost::str(boost::format("%1$+16.9e") % a_value);
  }

  size_type m_stride;
  std::string m_filename;

  path_type m_directory;
  boost::property_tree::ptree m_document;
}; // ptree<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_PTREE_HPP
