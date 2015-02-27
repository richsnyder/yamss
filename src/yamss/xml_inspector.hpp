#ifndef YAMSS_XML_INSPECTOR_HPP
#define YAMSS_XML_INSPECTOR_HPP

#include <fstream>
#include <iterator>
#include <string>
#include <boost/format.hpp>
#include "yamss/inspector.hpp"
#include "yamss/rapidxml_print.hpp"

namespace yamss {

template <typename T = double>
class xml_inspector : public inspector<T>
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  xml_inspector()
    : m_filename("yamss.xml")
    , m_document()
    , m_root()
    , m_timesteps()
  {
    // empty
  }

  xml_inspector(const boost::property_tree::ptree& a_tree)
    : m_document()
    , m_root()
    , m_timesteps()
  {
    m_filename = a_tree.get<std::string>("filename", "yamss.xml");
  }

  virtual
  ~xml_inspector()
  {
    // empty
  }

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure)
  {
    m_document.clear();
    m_root = append_node(&m_document, "yamss_output");
    m_timesteps = append_node(m_root, "timesteps");
  }

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure)
  {
    using namespace rapidxml;

    const value_type& t = a_eom.get_time(0);
    const vector_type& q = a_eom.get_displacement(0);
    const vector_type& dq = a_eom.get_velocity(0);
    const vector_type& ddq = a_eom.get_acceleration(0);
    const vector_type& f = a_eom.get_force(0);

    xml_node<>* xml_timestep = append_node(m_timesteps, "timestep");
    append_attribute(xml_timestep, "time", as_string(t));
    xml_node<>* xml_modes = append_node(xml_timestep, "modes");
    xml_node<>* xml_q = append_node(xml_modes, "displacement");
    xml_node<>* xml_dq = append_node(xml_modes, "velocity");
    xml_node<>* xml_ddq = append_node(xml_modes, "acceleration");
    xml_node<>* xml_f = append_node(xml_modes, "force");
    for (size_type n = 0; n < a_eom.get_size(); ++n)
    {
      append_node(xml_q, "mode", as_string(q(n)));
      append_node(xml_dq, "mode", as_string(dq(n)));
      append_node(xml_ddq, "mode", as_string(ddq(n)));
      append_node(xml_f, "mode", as_string(f(n)));
    }
  }

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure)
  {
    std::ofstream out(m_filename.c_str());
    rapidxml::print(std::ostream_iterator<char>(out), m_document, 0);
    out.close();
  }
protected:
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;

  std::string
  as_string(const value_type& a_value)
  {
    return boost::str(boost::format("%1$+16.9e") % a_value);
  }

  rapidxml::xml_node<>*
  append_node(rapidxml::xml_node<>* a_parent,
              const char* a_name)
  {
    using namespace rapidxml;

    xml_node<>* node = m_document.allocate_node(node_element, a_name);
    a_parent->append_node(node);
    return node;
  }

  rapidxml::xml_node<>*
  append_node(rapidxml::xml_node<>* a_parent,
              const std::string& a_name)
  {
    using namespace rapidxml;

    const char* name = m_document.allocate_string(a_name.c_str());
    xml_node<>* node = m_document.allocate_node(node_element, name);
    a_parent->append_node(node);
    return node;
  }

  rapidxml::xml_node<>*
  append_node(rapidxml::xml_node<>* a_parent,
              const char* a_name,
              const std::string& a_value)
  {
    using namespace rapidxml;

    const char* value = m_document.allocate_string(a_value.c_str());
    xml_node<>* node = m_document.allocate_node(node_element, a_name, value);
    a_parent->append_node(node);
    return node;
  }

  rapidxml::xml_node<>*
  append_node(rapidxml::xml_node<>* a_parent,
              const std::string& a_name,
              const std::string& a_value)
  {
    using namespace rapidxml;

    const char* name = m_document.allocate_string(a_name.c_str());
    const char* value = m_document.allocate_string(a_value.c_str());
    xml_node<>* node = m_document.allocate_node(node_element, name, value);
    a_parent->append_node(node);
    return node;
  }

  void
  append_attribute(rapidxml::xml_node<>* a_node,
                   const char* a_name,
                   const std::string& a_value)
  {
    using namespace rapidxml;

    const char* value = m_document.allocate_string(a_value.c_str());
    xml_attribute<>* attribute = m_document.allocate_attribute(a_name, value);
    a_node->append_attribute(attribute);
  }

  std::string m_filename;
  rapidxml::xml_document<> m_document;
  rapidxml::xml_node<>* m_root;
  rapidxml::xml_node<>* m_timesteps;
}; // xml_inspector<T> class

} // yamss namespace

#endif // YAMSS_XML_INSPECTOR_HPP
