#ifndef YAMSS_INPUT_READER_HPP
#define YAMSS_INPUT_READER_HPP

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "yamss/matrix_cast.hpp"
#include "yamss/runner.hpp"

// Evaluators
#include "yamss/lua_evaluator.hpp"

// Inspectors
#include "yamss/cout_q_inspector.hpp"
#include "yamss/tecplot_modes_inspector.hpp"
#include "yamss/tecplot_q_inspector.hpp"
#include "yamss/xml_inspector.hpp"

// Integrators
#include "yamss/generalized_alpha.hpp"
#include "yamss/newmark_beta.hpp"

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
  typedef lua_evaluator<T> evaluator_type;
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

  void
  create_integrator()
  {
    using namespace boost::property_tree;
    typedef boost::shared_ptr<integrator_type> integrator_ptr;

    std::string dft = "newmark_beta";
    std::string name = m_document.get<std::string>("solution.method.type", dft);
    ptree params = m_document.get_child("solution.method.parameters", ptree());
    if (name == "newmark_beta")
    {
      m_integrator = integrator_ptr(new newmark_beta<value_type>());
    }
    else if (name == "generalized_alpha")
    {
      m_integrator = integrator_ptr(new generalized_alpha<value_type>());
    }
    else
    {
      boost::format fmt("The integration method %1% is not supported");
      throw std::runtime_error(boost::str(fmt % name));
    }
  }

  void
  create_structure_and_eom()
  {
    using namespace boost::property_tree;
    typedef boost::shared_ptr<eom_type> eom_ptr;
    typedef boost::shared_ptr<structure_type> structure_ptr;

    ptree modes = m_document.get_child("modes", ptree());
    int num_modes = modes.count("mode");
    m_eom = eom_ptr(new eom_type(num_modes, m_integrator->stencil_size()));
    m_structure = structure_ptr(new structure_type(num_modes));
  }

  void
  create_runner()
  {
    typedef boost::shared_ptr<runner_type> runner_ptr;
    m_runner = runner_ptr(new runner_type(m_eom, m_structure, m_integrator));
  }

  void
  process_structure()
  {
    using namespace boost::property_tree;
    typedef ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    boost::optional<key_type> id;
    boost::optional<value_type> dof;
    ptree nodes = m_document.get_child("structure.nodes", ptree());
    range_type range = nodes.equal_range("node");
    for (const_iterator p = range.first; p != range.second; ++p)
    {
      id = p->second.get_optional<key_type>("id");
      if (id)
      {
        node_type& new_node = m_structure->add_node(*id);
        if ((dof = p->second.get_optional<value_type>("x")))
        {
          new_node.set_position(0, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("y")))
        {
          new_node.set_position(1, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("z")))
        {
          new_node.set_position(2, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("p")))
        {
          new_node.set_position(3, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("q")))
        {
          new_node.set_position(4, *dof);
        }
        if ((dof = p->second.get_optional<value_type>("r")))
        {
          new_node.set_position(5, *dof);
        }
      }
    }
  }

  void
  process_modes()
  {
    using namespace boost::property_tree;
    typedef ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    size_type mode = 0;
    boost::optional<key_type> id;
    boost::optional<value_type> dof;
    ptree modes = m_document.get_child("modes", ptree());
    ptree nodes;
    range_type mode_range = modes.equal_range("mode");
    range_type node_range;
    for (const_iterator p = mode_range.first; p != mode_range.second; ++p)
    {
      nodes = p->second.get_child("nodes");
      node_range = nodes.equal_range("node");
      for (const_iterator q = node_range.first; q != node_range.second; ++q)
      {
        id = q->second.get_optional<key_type>("id");
        if (id)
        {
          node_type& old_node = m_structure->get_node(*id);
          if ((dof = q->second.get_optional<value_type>("x")))
          {
            old_node.set_mode(mode, 0, *dof);
          }
          if ((dof = q->second.get_optional<value_type>("y")))
          {
            old_node.set_mode(mode, 1, *dof);
          }
          if ((dof = q->second.get_optional<value_type>("z")))
          {
            old_node.set_mode(mode, 2, *dof);
          }
          if ((dof = q->second.get_optional<value_type>("p")))
          {
            old_node.set_mode(mode, 3, *dof);
          }
          if ((dof = q->second.get_optional<value_type>("q")))
          {
            old_node.set_mode(mode, 4, *dof);
          }
          if ((dof = q->second.get_optional<value_type>("r")))
          {
            old_node.set_mode(mode, 5, *dof);
          }
        }
      }
      ++mode;
    }
  }

  void
  process_eom()
  {
    using namespace boost::property_tree;

    ptree matrices = m_document.get_child("eom.matrices", ptree());
    ptree initial = m_document.get_child("eom.initial_conditions", ptree());
    m_eom->set_mass(matrix_cast<T>(matrices.get<std::string>("mass", "")));
    m_eom->set_damping(matrix_cast<T>(matrices.get<std::string>("damping", "")));
    m_eom->set_stiffness(matrix_cast<T>(matrices.get<std::string>("stiffness", "")));
    m_eom->set_displacement(vector_cast<T>(initial.get<std::string>("displacement", "")));
    m_eom->set_velocity(vector_cast<T>(initial.get<std::string>("velocity", "")));
  }

  void
  process_loads()
  {
    using namespace boost::property_tree;
    typedef ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    boost::optional<key_type> id;
    boost::optional<std::string> dof;
    ptree loads = m_document.get_child("loads", ptree());
    ptree nodes;
    range_type load_range = loads.equal_range("load");
    range_type node_range;
    for (const_iterator p = load_range.first; p != load_range.second; ++p)
    {
      id = p->second.get_optional<key_type>("id");
      if (id)
      {
        boost::shared_ptr<evaluator_type> new_evaluator(new lua_evaluator<T>());
        if ((dof = p->second.get_optional<std::string>("equations.x")))
        {
          new_evaluator->set_expression(0, *dof);
        }
        if ((dof = p->second.get_optional<std::string>("equations.y")))
        {
          new_evaluator->set_expression(1, *dof);
        }
        if ((dof = p->second.get_optional<std::string>("equations.z")))
        {
          new_evaluator->set_expression(2, *dof);
        }
        if ((dof = p->second.get_optional<std::string>("equations.p")))
        {
          new_evaluator->set_expression(3, *dof);
        }
        if ((dof = p->second.get_optional<std::string>("equations.q")))
        {
          new_evaluator->set_expression(4, *dof);
        }
        if ((dof = p->second.get_optional<std::string>("equations.r")))
        {
          new_evaluator->set_expression(5, *dof);
        }
        load_type& new_load = m_structure->add_load(*id, new_evaluator);
        nodes = p->second.get_child("nodes", ptree());
        node_range = nodes.equal_range("id");
        for (const_iterator q = node_range.first; q != node_range.second; ++q)
        {
          new_load.add_node(q->second.get_value<key_type>());
        }
      }
    }
  }

  void
  process_solution()
  {
    m_runner->set_time_step(m_document.get<value_type>("solution.time.step", 0.01));
    m_runner->set_final_time(m_document.get<value_type>("solution.time.span", 1.0));
  }

  void
  process_output()
  {
    using namespace boost::property_tree;
    typedef ptree::const_assoc_iterator const_iterator;
    typedef std::pair<const_iterator, const_iterator> range_type;

    std::string type_;
    ptree outputs = m_document.get_child("outputs", ptree());
    range_type range = outputs.equal_range("output");
    for (const_iterator p = range.first; p != range.second; ++p)
    {
      type_ = p->second.get<std::string>("type", "-");
      if (type_ == "default")
      {
        m_runner->add_inspector(
            boost::shared_ptr<cout_q_inspector<T> >(
                new cout_q_inspector<T>()
              )
          );
      }
      else if (type_ == "tecplot_modes")
      {
        std::string filename = p->second.get<std::string>("parameters.filename");
        m_runner->add_inspector(
            boost::shared_ptr<tecplot_modes_inspector<T> >(
                new tecplot_modes_inspector<T>(
                    filename
                  )
              )
          );
      }
      else if (type_ == "tecplot_q")
      {
        std::string filename = p->second.get<std::string>("parameters.filename");
        m_runner->add_inspector(
            boost::shared_ptr<tecplot_q_inspector<T> >(
                new tecplot_q_inspector<T>(
                    filename
                  )
              )
          );
      }
      else if (type_ == "xml")
      {
        std::string filename = p->second.get<std::string>("parameters.filename");
        m_runner->add_inspector(
            boost::shared_ptr<xml_inspector<T> >(
                new xml_inspector<T>(
                    filename
                  )
              )
          );
      }
    }
  }
private:
  boost::property_tree::ptree m_document;

  int m_number_of_modes;

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
