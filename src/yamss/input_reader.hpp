#ifndef YAMSS_INPUT_READER_HPP
#define YAMSS_INPUT_READER_HPP

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "yamss/matrix_cast.hpp"
#include "yamss/runner.hpp"

// Evaluators
#include "yamss/evaluator/lua.hpp"

// Inspectors
#include "yamss/inspector/ptree.hpp"
#include "yamss/inspector/summary.hpp"
#include "yamss/inspector/tecplot.hpp"

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
    process_structure();
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
  typedef typename runner_type::eom_type eom_type;
  typedef typename runner_type::integrator_type integrator_type;
  typedef typename runner_type::structure_type structure_type;

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
    m_runner = boost::make_shared<runner_type>(m_eom, m_structure, m_integrator);
  }

  void
  process_structure()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    const_iterator p;
    boost::optional<key_type> id;
    boost::optional<value_type> dof;
    range_type range = m_document.get_child("structure.nodes").equal_range("node");
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
    range_type range = m_document.get_child("modes").equal_range("mode");
    for (p = range.first; p != range.second; ++p)
    {
      range_type nrange = p->second.get_child("nodes").equal_range("node");
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
      ++mode;
    }
  }

  void
  process_eom()
  {
    boost::optional<std::string> str;
    boost::property_tree::ptree tree;

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

  template <typename Evaluator>
  void
  add_load(key_type& a_id, const boost::property_tree::ptree& a_tree)
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    const_iterator p;
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
    range_type range = a_tree.get_child("nodes").equal_range("id");
    for (p = range.first; p != range.second; ++p)
    {
      load_.add_node(p->second.get_value<key_type>());
    }
  }

  void
  process_loads()
  {
    namespace pt = boost::property_tree;
    typedef pt::ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    const_iterator p;
    const_iterator q;
    boost::optional<key_type> id;
    boost::optional<std::string> type_;
    range_type range = m_document.get_child("loads").equal_range("load");
    for (p = range.first; p != range.second; ++p)
    {
      id = p->second.get_optional<key_type>("id");
      type_ = p->second.get<std::string>("type", "lua");
      if (id)
      {
        if (*type_ == "lua")
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
    value_type t = m_document.get<value_type>("solution.time.span", 1.0);
    value_type dt = m_document.get<value_type>("solution.time.step", 0.01);
    m_runner->set_time_step(dt);
    m_runner->set_final_time(t);
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

    const_iterator p;
    std::string type_;
    range_type range = m_document.get_child("outputs").equal_range("output");
    for (p = range.first; p != range.second; ++p)
    {
      type_ = p->second.get<std::string>("type");
      if (type_ == "summary")
      {
        add_inspector<inspector::summary<T> >(p->second);
      }
      else if (type_ == "ptree")
      {
        add_inspector<inspector::ptree<T> >(p->second);
      }
      else if (type_ == "tecplot")
      {
        add_inspector<inspector::tecplot<T> >(p->second);
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
read_input(const std::string& a_filename)
{
  input_reader<T> reader(a_filename);
  return reader.get_runner();
}

} // yamss namespace

#endif // YAMSS_INPUT_READER_HPP
