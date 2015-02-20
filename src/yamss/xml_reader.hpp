#ifndef YAMSS_XML_READER_HPP
#define YAMSS_XML_READER_HPP

#include <fstream>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "yamss/lua_evaluator.hpp"
#include "yamss/matrix_cast.hpp"
#include "yamss/newmark_beta.hpp"
#include "yamss/rapidxml.hpp"
#include "yamss/runner.hpp"
#include "yamss/cout_q_inspector.hpp"
#include "yamss/tecplot_modes_inspector.hpp"
#include "yamss/tecplot_q_inspector.hpp"
#include "yamss/xml_inspector.hpp"

namespace yamss {

template <typename T>
class xml_reader
{
public:
  typedef T value_type;
  typedef runner<T> runner_type;

  xml_reader(const std::string& a_filename)
    : m_eom()
    , m_structure()
    , m_integrator()
    , m_runner()
  {
    read_file(a_filename);
    parse_file();
    validate();
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

  void
  read_file(const std::string& a_filename)
  {
    std::ifstream in(a_filename.c_str());
    if (!in)
    {
      throw std::runtime_error("Could not open the XML file for reading");
    }
    in.seekg(0, std::ios::end);
    m_contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&m_contents[0], m_contents.size());
    in.close();
  }

  void
  parse_file()
  {
    m_document.parse<0>(const_cast<char*>(m_contents.c_str()));
    m_root = m_document.first_node("yamss");
  }

  void
  validate()
  {
    boost::format fmt("Invalid XML file: %1%");
    if (!m_root)
    {
      fmt % "the root node must be <yamss>";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("eom"))
    {
      fmt % "missing <eom> node";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("loads"))
    {
      fmt % "missing <loads> node";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("modes"))
    {
      fmt % "missing <modes> node";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("output"))
    {
      fmt % "missing <output> node";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("solution"))
    {
      fmt % "missing <solution> node";
      throw std::runtime_error(boost::str(fmt));
    }
    if (!m_root->first_node("structure"))
    {
      fmt % "missing <structure> node";
      throw std::runtime_error(boost::str(fmt));
    }
  }

  void
  create_integrator()
  {
    using namespace rapidxml;

    xml_node<>* xml_solution = m_root->first_node("solution");
    xml_attribute<>* xml_method = xml_solution->last_attribute("method");
    std::string name = "newmark_beta";
    if (xml_method)
    {
      name = xml_method->value();
    }
    if (name == "newmark_beta")
    {
      m_integrator = boost::shared_ptr<typename runner_type::integrator_type>(
            new newmark_beta<value_type>()
          );
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
    using namespace rapidxml;

    int number_of_modes = 0;
    xml_node<>* xml_modes = m_root->first_node("modes");
    xml_node<>* xml_mode = xml_modes->first_node("mode");
    while (xml_mode)
    {
      ++number_of_modes;
      xml_mode = xml_mode->next_sibling("mode");
    }

    m_eom = boost::shared_ptr<typename runner_type::eom_type>(
        new typename runner_type::eom_type(
            number_of_modes,
            m_integrator->stencil_size()
          )
      );

    m_structure = boost::shared_ptr<typename runner_type::structure_type>(
        new typename runner_type::structure_type(
            number_of_modes
          )
      );
  }

  void
  create_runner()
  {
    m_runner = boost::shared_ptr<runner_type>(
        new runner_type(
            m_eom,
            m_structure,
            m_integrator
          )
      );
  }

  void
  process_structure()
  {
    using namespace rapidxml;

    key_type id;
    xml_node<>* xml_structure = m_root->first_node("structure");
    xml_node<>* xml_nodes = xml_structure->first_node("nodes");
    xml_node<>* xml_node_ = xml_nodes->first_node("node");
    xml_node<>* xml_dof;
    xml_attribute<>* xml_id;
    while (xml_node_)
    {
      xml_id = xml_node_->last_attribute("id");
      if (xml_id)
      {
        id = boost::lexical_cast<key_type>(xml_id->value());
        node_type& node_ = m_structure->add_node(id);
        if ((xml_dof = xml_node_->first_node("x")))
        {
          node_.set_position(0, boost::lexical_cast<value_type>(xml_dof->value()));
        }
        if ((xml_dof = xml_node_->first_node("y")))
        {
          node_.set_position(1, boost::lexical_cast<value_type>(xml_dof->value()));
        }
        if ((xml_dof = xml_node_->first_node("z")))
        {
          node_.set_position(2, boost::lexical_cast<value_type>(xml_dof->value()));
        }
        if ((xml_dof = xml_node_->first_node("p")))
        {
          node_.set_position(3, boost::lexical_cast<value_type>(xml_dof->value()));
        }
        if ((xml_dof = xml_node_->first_node("q")))
        {
          node_.set_position(4, boost::lexical_cast<value_type>(xml_dof->value()));
        }
        if ((xml_dof = xml_node_->first_node("r")))
        {
          node_.set_position(5, boost::lexical_cast<value_type>(xml_dof->value()));
        }
      }
      xml_node_ = xml_node_->next_sibling("node");
    }
  }

  void
  process_modes()
  {
    using namespace rapidxml;

    key_type node_ref;
    size_type mode = 0;
    xml_node<>* xml_modes = m_root->first_node("modes");
    xml_node<>* xml_mode = xml_modes->first_node("mode");
    xml_node<>* xml_nodes;
    xml_node<>* xml_node_;
    xml_node<>* xml_dof;
    xml_attribute<>* xml_ref;
    while (xml_mode)
    {
      xml_nodes = xml_mode->first_node("nodes");
      xml_node_ = xml_nodes->first_node("node");
      while (xml_node_)
      {
        xml_ref = xml_node_->last_attribute("ref");
        if (xml_ref)
        {
          node_ref = boost::lexical_cast<key_type>(xml_ref->value());
          node_type& node_ = m_structure->get_node(node_ref);
          if ((xml_dof = xml_node_->first_node("x")))
          {
            node_.set_mode(mode, 0, boost::lexical_cast<value_type>(xml_dof->value()));
          }
          if ((xml_dof = xml_node_->first_node("y")))
          {
            node_.set_mode(mode, 1, boost::lexical_cast<value_type>(xml_dof->value()));
          }
          if ((xml_dof = xml_node_->first_node("z")))
          {
            node_.set_mode(mode, 2, boost::lexical_cast<value_type>(xml_dof->value()));
          }
          if ((xml_dof = xml_node_->first_node("p")))
          {
            node_.set_mode(mode, 3, boost::lexical_cast<value_type>(xml_dof->value()));
          }
          if ((xml_dof = xml_node_->first_node("q")))
          {
            node_.set_mode(mode, 4, boost::lexical_cast<value_type>(xml_dof->value()));
          }
          if ((xml_dof = xml_node_->first_node("r")))
          {
            node_.set_mode(mode, 5, boost::lexical_cast<value_type>(xml_dof->value()));
          }
        }
        xml_node_ = xml_node_->next_sibling("node");
      }
      xml_mode = xml_mode->next_sibling("mode");
      mode++;
    }
  }

  void
  process_eom()
  {
    using namespace rapidxml;

    xml_node<>* xml_eom = m_root->first_node("eom");
    xml_node<>* xml_matrices = xml_eom->first_node("matrices");
    xml_node<>* xml_initial = xml_eom->first_node("initial_conditions");
    xml_node<>* xml_element;
    if (xml_matrices)
    {
      if ((xml_element = xml_matrices->first_node("mass")))
      {
        m_eom->set_mass(matrix_cast<T>(xml_element->value()));
      }
      if ((xml_element = xml_matrices->first_node("damping")))
      {
        m_eom->set_damping(matrix_cast<T>(xml_element->value()));
      }
      if ((xml_element = xml_matrices->first_node("stiffness")))
      {
        m_eom->set_stiffness(matrix_cast<T>(xml_element->value()));
      }
    }
    if (xml_initial)
    {
      if ((xml_element = xml_initial->first_node("displacement")))
      {
        m_eom->set_displacement(vector_cast<T>(xml_element->value()));
      }
      if ((xml_element = xml_initial->first_node("velocity")))
      {
        m_eom->set_velocity(vector_cast<T>(xml_element->value()));
      }
    }
  }

  void
  process_loads()
  {
    using namespace rapidxml;

    key_type load_id;
    key_type node_ref;
    xml_node<>* xml_loads = m_root->first_node("loads");
    xml_node<>* xml_load = xml_loads->first_node("load");
    xml_node<>* xml_equations;
    xml_node<>* xml_nodes;
    xml_node<>* xml_node_;
    xml_node<>* xml_dof;
    xml_attribute<>* xml_id;
    xml_attribute<>* xml_ref;
    while (xml_load)
    {
      xml_id = xml_load->last_attribute("id");
      xml_nodes = xml_load->first_node("nodes");
      xml_equations = xml_load->first_node("equations");
      if (xml_id && xml_nodes)
      {
        load_id = boost::lexical_cast<key_type>(xml_id->value());
        boost::shared_ptr<evaluator_type> evaluator_(new lua_evaluator<T>());
        if ((xml_dof = xml_equations->first_node("x")))
        {
          evaluator_->set_expression(0, xml_dof->value());
        }
        if ((xml_dof = xml_equations->first_node("y")))
        {
          evaluator_->set_expression(1, xml_dof->value());
        }
        if ((xml_dof = xml_equations->first_node("z")))
        {
          evaluator_->set_expression(2, xml_dof->value());
        }
        if ((xml_dof = xml_equations->first_node("p")))
        {
          evaluator_->set_expression(3, xml_dof->value());
        }
        if ((xml_dof = xml_equations->first_node("q")))
        {
          evaluator_->set_expression(4, xml_dof->value());
        }
        if ((xml_dof = xml_equations->first_node("r")))
        {
          evaluator_->set_expression(5, xml_dof->value());
        }
        load_type& load_ = m_structure->add_load(load_id, evaluator_);
        xml_node_ = xml_nodes->first_node("node");
        while (xml_node_)
        {
          xml_ref = xml_node_->last_attribute("ref");
          if (xml_ref)
          {
            node_ref = boost::lexical_cast<key_type>(xml_ref->value());
            load_.add_node(node_ref);
          }
          xml_node_ = xml_node_->next_sibling("node");
        }
      }
      xml_load = xml_load->next_sibling("load");
    }
  }

  void
  process_solution()
  {
    using namespace rapidxml;

    xml_node<>* xml_solution = m_root->first_node("solution");
    xml_node<>* xml_time = xml_solution->first_node("time");
    xml_node<>* xml_element;
    if (xml_time)
    {
      if ((xml_element = xml_time->first_node("step")))
      {
        m_runner->set_time_step(boost::lexical_cast<value_type>(xml_element->value()));
      }
      if ((xml_element = xml_time->first_node("span")))
      {
        m_runner->set_final_time(boost::lexical_cast<value_type>(xml_element->value()));
      }
    }
  }

  void
  process_output()
  {
    using namespace rapidxml;

    xml_node<>* xml_output = m_root->first_node("output");
    xml_node<>* xml_element;
    xml_attribute<>* xml_filename;
    if (xml_output)
    {
      if ((xml_element = xml_output->first_node("default")))
      {
        m_runner->add_inspector(
            boost::shared_ptr<cout_q_inspector<T> >(
                new cout_q_inspector<T>()
              )
          );
      }
      if ((xml_element = xml_output->first_node("tecplot_modes")))
      {
        xml_filename = xml_element->last_attribute("filename");
        m_runner->add_inspector(
            boost::shared_ptr<tecplot_modes_inspector<T> >(
                new tecplot_modes_inspector<T>(
                    xml_filename->value()
                  )
              )
          );
      }
      if ((xml_element = xml_output->first_node("tecplot_q")))
      {
        xml_filename = xml_element->last_attribute("filename");
        m_runner->add_inspector(
            boost::shared_ptr<tecplot_q_inspector<T> >(
                new tecplot_q_inspector<T>(
                    xml_filename->value()
                  )
              )
          );
      }
      if ((xml_element = xml_output->first_node("xml")))
      {
        xml_filename = xml_element->last_attribute("filename");
        m_runner->add_inspector(
            boost::shared_ptr<xml_inspector<T> >(
                new xml_inspector<T>(
                    xml_filename->value()
                  )
              )
          );
      }
    }
  }
private:
  std::string m_contents;
  rapidxml::xml_document<> m_document;
  rapidxml::xml_node<>* m_root;

  int m_number_of_modes;

  boost::shared_ptr<typename runner_type::eom_type> m_eom;
  boost::shared_ptr<typename runner_type::structure_type> m_structure;
  boost::shared_ptr<typename runner_type::integrator_type> m_integrator;
  boost::shared_ptr<runner_type> m_runner;
}; // xml_reader<T> class

template <typename T>
boost::shared_ptr<runner<T> >
read_xml(const std::string& a_filename)
{
  xml_reader<T> reader(a_filename);
  return reader.get_runner();
}

} // yamss namespace

#endif // YAMSS_XML_READER_HPP
