#ifndef YAMSS_INPUT_READER_HPP
#define YAMSS_INPUT_READER_HPP

#include <limits>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "yamss/matrix_cast.hpp"
#include "yamss/runner.hpp"

// Evaluators
#include "yamss/evaluator/interface.hpp"
#include "yamss/evaluator/lua.hpp"

// Inspectors
#include "yamss/inspector/modes.hpp"
#include "yamss/inspector/motion.hpp"
#include "yamss/inspector/ptree.hpp"
#include "yamss/inspector/summary.hpp"

// Integrators
#include "yamss/integrator/generalized_alpha.hpp"
#include "yamss/integrator/newmark_beta.hpp"

namespace yamss {

template <typename T>
class input_reader
{
public:
  typedef T value_type;
  typedef runner<T> runner_type;

  input_reader()
    : m_eom()
    , m_structure()
    , m_integrator()
    , m_runner()
  {
    parse_standard_input();
    create_integrator();
    create_structure_and_eom();
    create_runner();
    process_nodes();
    process_elements();
    process_modes();
    process_eom();
    process_loads();
    process_solution();
    process_output();
  }

  input_reader(const std::string& a_filename)
    : m_eom()
    , m_structure()
    , m_integrator()
    , m_runner()
  {
    parse_file(a_filename);
    create_integrator();
    create_structure_and_eom();
    create_runner();
    process_nodes();
    process_elements();
    process_modes();
    process_eom();
    process_loads();
    process_solution();
    process_output();
  }

  boost::shared_ptr<runner_type>
  get_runner()
  {
    return m_runner;
  }
protected:
  typedef size_t key_type;
  typedef size_t size_type;
  typedef node<T> node_type;
  typedef load<T> load_type;
  typedef element element_type;
  typedef typename runner_type::eom_type eom_type;
  typedef typename runner_type::integrator_type integrator_type;
  typedef typename runner_type::structure_type structure_type;

  void
  parse_standard_input()
  {
    boost::property_tree::ptree document;
    boost::property_tree::read_xml(std::cin, document);
    m_document = document.get_child("yamss");
  }

  void
  parse_file(const std::string& a_filename)
  {
    boost::property_tree::ptree document;
    boost::property_tree::read_xml(a_filename, document);
    m_document = document.get_child("yamss");
  }

  template <typename Integrator>
  void
  assign_integrator(const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;

    try
    {
      pt::ptree params = a_tree.get_child("solution.method.parameters");
      m_integrator = boost::make_shared<Integrator>(params);
    }
    catch (pt::ptree_bad_path& e)
    {
      m_integrator = boost::make_shared<Integrator>();
    }
  }

  void
  create_integrator()
  {
    namespace pt = boost::property_tree;

    std::string type_;
    type_ = m_document.get<std::string>("solution.method.type", "newmark_beta");
    if (type_ == "newmark_beta")
    {
      assign_integrator<integrator::newmark_beta<T> >(m_document);
    }
    else if (type_ == "generalized_alpha")
    {
      assign_integrator<integrator::generalized_alpha<T> >(m_document);
    }
    else
    {
      boost::format fmt("The integration method %1% is not supported");
      throw std::runtime_error(boost::str(fmt % type_));
    }
  }

  void
  create_structure_and_eom()
  {
    namespace pt = boost::property_tree;

    size_type num_modes = 0;
    size_type stencil_size = m_integrator->stencil_size();
    try
    {
      num_modes = m_document.get_child("modes").count("mode");
    }
    catch (pt::ptree_bad_path& e)
    {
      // empty
    }
    m_eom = boost::make_shared<eom_type>(num_modes, stencil_size);
    m_structure = boost::make_shared<structure_type>(num_modes);
  }

  void
  create_runner()
  {
    m_runner = boost::make_shared<runner_type>(m_eom,
                                               m_structure,
                                               m_integrator);
  }

  void
  process_nodes()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    pt::ptree nodes_tree;
    try
    {
      nodes_tree = m_document.get_child("structure.nodes");
    }
    catch (pt::ptree_bad_path& e)
    {
      return;
    }

    const_iterator p;
    boost::optional<key_type> id;
    boost::optional<value_type> dof;
    range_type range = nodes_tree.equal_range("node");
    for (p = range.first; p != range.second; ++p)
    {
      id = p->second.get_optional<key_type>("id");
      if (id)
      {
        node_type& node_ = m_structure->add_node(*id);
        if ((dof = p->second.get_optional<value_type>("x")))
        {
          node_.set_position(0, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("y")))
        {
          node_.set_position(1, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("z")))
        {
          node_.set_position(2, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("p")))
        {
          node_.set_position(3, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("q")))
        {
          node_.set_position(4, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("r")))
        {
          node_.set_position(5, *dof);
        }
      }
    }
  }

  element_type::shape_type
  get_shape(const std::string& a_keyword) const
  {
    element_type::shape_type shape;
    if (a_keyword == "line")
    {
      shape = element_type::LINE;
    }
    else if (a_keyword == "tria")
    {
      shape = element_type::TRIANGLE;
    }
    else if (a_keyword == "quad")
    {
      shape = element_type::QUADRILATERAL;
    }
    else
    {
      boost::format fmt("The element type %1% is not supported");
      throw std::runtime_error(boost::str(fmt % a_keyword));
    }
    return shape;
  }

  void
  add_vertices(element_type& a_element,
               const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    key_type id;
    size_type pos;
    const_iterator p;
    range_type range = a_tree.equal_range("v");
    for (p = range.first, pos = 0; p != range.second; ++p, ++pos)
    {
      id = p->second.get_value<key_type>();
      a_element.set_vertex(pos, id);
    }
  }

  void
  process_elements()
  {
    namespace pt = boost::property_tree;

    element::shape_type shape;
    boost::optional<key_type> id;
    pt::ptree::const_iterator p;
    try
    {
      pt::ptree elements = m_document.get_child("structure.elements");
      for (p = elements.begin(); p != elements.end(); ++p)
      {
        id = p->second.get_optional<key_type>("id");
        if (id)
        {
          shape = get_shape(p->first);
          element_type& element_ = m_structure->add_element(*id, shape);
          add_vertices(element_, p->second);
        }
      }
    }
    catch (pt::ptree_bad_path& e)
    {
      // empty
    }
  }

  void
  process_modes()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    const_iterator p;
    const_iterator q;
    size_type mode = 0;
    boost::optional<key_type> id;
    boost::optional<value_type> dof;
    pt::ptree nodes_tree;
    try
    {
      pt::ptree modes_tree = m_document.get_child("modes");
      range_type range = m_document.get_child("modes").equal_range("mode");
      for (p = range.first; p != range.second; ++p)
      {
        try
        {
          nodes_tree = p->second.get_child("nodes");
          range_type nrange = nodes_tree.equal_range("node");
          for (q = nrange.first; q != nrange.second; ++q)
          {
            id = q->second.get_optional<key_type>("id");
            if (id)
            {
              node_type& node_ = m_structure->get_node(*id);
              if ((dof = q->second.get_optional<value_type>("x")))
              {
                node_.set_mode(mode, 0, *dof);
              }
              if ((dof = q->second.get_optional<value_type>("y")))
              {
                node_.set_mode(mode, 1, *dof);
              }
              if ((dof = q->second.get_optional<value_type>("z")))
              {
                node_.set_mode(mode, 2, *dof);
              }
              if ((dof = q->second.get_optional<value_type>("p")))
              {
                node_.set_mode(mode, 3, *dof);
              }
              if ((dof = q->second.get_optional<value_type>("q")))
              {
                node_.set_mode(mode, 4, *dof);
              }
              if ((dof = q->second.get_optional<value_type>("r")))
              {
                node_.set_mode(mode, 5, *dof);
              }
            }
          }
        }
        catch (pt::ptree_bad_path& e)
        {
          // empty
        }
        ++mode;
      }
    }
    catch (pt::ptree_bad_path& e)
    {
      // empty
    }
  }

  void
  process_eom()
  {
    boost::optional<std::string> str;
    boost::property_tree::ptree tree;

    try
    {
      tree = m_document.get_child("eom.matrices");
      if ((str = tree.get_optional<std::string>("mass")))
      {
        m_eom ->set_mass(matrix_cast<value_type>(*str));
      }
      if ((str = tree.get_optional<std::string>("damping")))
      {
        m_eom->set_damping(matrix_cast<value_type>(*str));
      }
      if ((str = tree.get_optional<std::string>("stiffness")))
      {
        m_eom->set_stiffness(matrix_cast<value_type>(*str));
      }
    }
    catch (boost::property_tree::ptree_bad_path& e)
    {
      // empty
    }

    try
    {
      tree = m_document.get_child("eom.initial_conditions");
      if ((str = tree.get_optional<std::string>("displacement")))
      {
        m_eom->set_displacement(vector_cast<value_type>(*str));
      }
      if ((str = tree.get_optional<std::string>("velocity")))
      {
        m_eom->set_velocity(vector_cast<value_type>(*str));
      }
    }
    catch (boost::property_tree::ptree_bad_path& e)
    {
      // empty
    }
  }

  void
  add_all_nodes(load_type& a_load, const boost::property_tree::ptree& a_tree)
  {
    typename structure_type::node_iterator p;
    if (a_tree.find("all") != a_tree.not_found())
    {
      for (p = m_structure->begin_nodes(); p != m_structure->end_nodes(); ++p)
      {
        a_load.add_node(p->get_key());
      }
    }
  }

  void
  add_range_nodes(load_type& a_load, const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    key_type id;
    key_type lower;
    key_type upper;
    key_type min_key = std::numeric_limits<key_type>::min();
    key_type max_key = std::numeric_limits<key_type>::max();

    const_iterator p;
    typename structure_type::node_iterator q;
    range_type range = a_tree.equal_range("range");
    for (p = range.first; p != range.second; ++p)
    {
      lower = p->second.get<key_type>("begin", min_key);
      upper = p->second.get<key_type>("end", max_key);
      for (q = m_structure->begin_nodes(); q != m_structure->end_nodes(); ++q)
      {
        id = q->get_key();
        if (lower <= id && id <= upper)
        {
          a_load.add_node(id);
        }
      }
    }
  }

  void
  add_single_nodes(load_type& a_load, const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    const_iterator p;
    range_type range = a_tree.equal_range("node");
    for (p = range.first; p != range.second; ++p)
    {
      a_load.add_node(p->second.get_value<key_type>());
    }
  }

  void
  add_nodes(load_type& a_load, const boost::property_tree::ptree& a_tree)
  {
    typedef typename structure_type::node_iterator node_iterator;

    boost::property_tree::ptree tree;
    try
    {
      tree = a_tree.get_child("nodes");
    }
    catch (boost::property_tree::ptree_bad_path& e)
    {
      return;
    }

    add_all_nodes(a_load, tree);
    add_range_nodes(a_load, tree);
    add_single_nodes(a_load, tree);
  }

  template <typename Evaluator>
  void
  add_load(key_type& a_id, const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;

    boost::shared_ptr<evaluator::evaluator<value_type> > ptr;
    try
    {
      pt::ptree params = a_tree.get_child("parameters");
      ptr = boost::make_shared<Evaluator>(params);
    }
    catch (pt::ptree_bad_path& e)
    {
      ptr = boost::make_shared<Evaluator>();
    }
    load_type& load_ = m_structure->add_load(a_id, ptr);
    add_nodes(load_, a_tree);
  }

  void
  process_loads()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    pt::ptree loads_tree;
    try
    {
      loads_tree = m_document.get_child("loads");
    }
    catch (pt::ptree_bad_path& e)
    {
      return;
    }

    const_iterator p;
    const_iterator q;
    boost::optional<key_type> id;
    boost::optional<std::string> type_;
    range_type range = loads_tree.equal_range("load");
    for (p = range.first; p != range.second; ++p)
    {
      id = p->second.get_optional<key_type>("id");
      type_ = p->second.get<std::string>("type", "lua");
      if (id)
      {
        if (*type_ == "interface")
        {
          add_load<evaluator::interface<T> >(*id, p->second);
        }
        else if (*type_ == "lua")
        {
          add_load<evaluator::lua<T> >(*id, p->second);
        }
        else
        {
          boost::format fmt("The load evaluator method %1% is not supported");
          throw std::runtime_error(boost::str(fmt % *type_));
        }
      }
    }
  }

  void
  process_solution()
  {
    namespace pt = boost::property_tree;

    value_type t = m_document.get<value_type>("solution.time.span", 1.0);
    value_type dt = m_document.get<value_type>("solution.time.step", 0.01);
    m_runner->set_time_step(dt);
    m_runner->set_final_time(t);

    pt::ptree dofs_tree;
    try
    {
      dofs_tree = m_document.get_child("solution").get_child("dofs");
    }
    catch (pt::ptree_bad_path& e)
    {
      return;
    }

    if (dofs_tree.count("x") == 0)
    {
      m_structure->deactivate_dof(0);
    }
    if (dofs_tree.count("y") == 0)
    {
      m_structure->deactivate_dof(1);
    }
    if (dofs_tree.count("z") == 0)
    {
      m_structure->deactivate_dof(2);
    }
    if (dofs_tree.count("p") == 0)
    {
      m_structure->deactivate_dof(3);
    }
    if (dofs_tree.count("q") == 0)
    {
      m_structure->deactivate_dof(4);
    }
    if (dofs_tree.count("r") == 0)
    {
      m_structure->deactivate_dof(5);
    }
  }

  template <typename Output>
  void
  add_inspector(const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;

    boost::shared_ptr<inspector::inspector<value_type> > ptr;
    try
    {
      pt::ptree params = a_tree.get_child("parameters");
      ptr = boost::make_shared<Output>(params);
    }
    catch (pt::ptree_bad_path& e)
    {
      ptr = boost::make_shared<Output>();
    }
    m_runner->add_inspector(ptr);
  }

  void
  process_output()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    pt::ptree outputs_tree;
    try
    {
      outputs_tree = m_document.get_child("outputs");
    }
    catch (pt::ptree_bad_path& e)
    {
      return;
    }

    const_iterator p;
    std::string type_;
    range_type range = outputs_tree.equal_range("output");
    for (p = range.first; p != range.second; ++p)
    {
      type_ = p->second.get<std::string>("type");
      if (type_ == "modes")
      {
        add_inspector<inspector::modes<T> >(p->second);
      }
      else if (type_ == "motion")
      {
        add_inspector<inspector::motion<T> >(p->second);
      }
      else if (type_ == "ptree")
      {
        add_inspector<inspector::ptree<T> >(p->second);
      }
      else if (type_ == "summary")
      {
        add_inspector<inspector::summary<T> >(p->second);
      }
      else
      {
        boost::format fmt("The output method %1% is not supported");
        throw std::runtime_error(boost::str(fmt % type_));
      }
    }
  }
private:
  boost::property_tree::ptree m_document;

  boost::shared_ptr<eom_type> m_eom;
  boost::shared_ptr<structure_type> m_structure;
  boost::shared_ptr<integrator_type> m_integrator;
  boost::shared_ptr<runner_type> m_runner;
}; // input_reader<T> class

template <typename T>
boost::shared_ptr<runner<T> >
read_input()
{
  input_reader<T> reader;
  return reader.get_runner();
}

template <typename T>
boost::shared_ptr<runner<T> >
read_input(const std::string& a_filename)
{
  input_reader<T> reader(a_filename);
  return reader.get_runner();
}

} // yamss namespace

#endif // YAMSS_INPUT_READER_HPP
