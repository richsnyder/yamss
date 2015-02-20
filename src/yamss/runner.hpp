#ifndef YAMSS_RUNNER_HPP
#define YAMSS_RUNNER_HPP

#include <algorithm>
#include <iostream>
#include <list>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include "yamss/eom.hpp"
#include "yamss/inspector.hpp"
#include "yamss/integrator.hpp"
#include "yamss/structure.hpp"

namespace yamss {

template <typename T = double>
class runner
{
public:
  typedef T value_type;
  typedef const T& const_reference;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;
  typedef inspector<T> inspector_type;
  typedef integrator<T> integrator_type;
  typedef boost::shared_ptr<eom_type> eom_pointer;
  typedef boost::shared_ptr<structure_type> structure_pointer;
  typedef boost::shared_ptr<inspector_type> inspector_pointer;
  typedef boost::shared_ptr<integrator_type> integrator_pointer;

  runner(const eom_pointer a_eom,
         const structure_pointer a_structure,
         const integrator_pointer a_integrator)
    : m_eom(a_eom)
    , m_structure(a_structure)
    , m_integrator(a_integrator)
    , m_time_step(0.01)
    , m_final_time(1.0)
  {
    // empty
  }

  ~runner()
  {
    // empty
  }

  eom_pointer
  get_eom()
  {
    return m_eom;
  }

  structure_pointer
  get_structure()
  {
    return m_structure;
  }

  integrator_pointer
  get_integrator()
  {
    return m_integrator;
  }

  const_reference
  get_time_step() const
  {
    return m_time_step;
  }

  const_reference
  get_final_time() const
  {
    return m_final_time;
  }

  void
  set_time_step(const_reference a_time_step)
  {
    m_time_step = a_time_step;
  }

  void
  set_final_time(const_reference a_final_time)
  {
    m_final_time = a_final_time;
  }

  void
  add_inspector(const inspector_pointer a_inspector)
  {
    m_inspectors.push_back(a_inspector);
  }

  void
  initialize()
  {
    m_structure->apply_loads(m_eom->get_time(0));
    m_eom->set_force(m_structure->get_generalized_force());
    m_eom->compute_acceleration();
    std::for_each(
        m_inspectors.begin(),
        m_inspectors.end(),
        boost::bind(&runner<T>::initialize, this, _1)
      );
  }

  void
  step()
  {
      m_eom->advance(m_time_step);
      m_structure->apply_loads(m_eom->get_time(0));
      m_eom->set_force(m_structure->get_generalized_force());
      m_integrator->operator()(*m_eom);
      std::for_each(
          m_inspectors.begin(),
          m_inspectors.end(),
          boost::bind(&runner<T>::update, this, _1)
        );
  }

  void
  finalize()
  {
    std::for_each(
        m_inspectors.begin(),
        m_inspectors.end(),
        boost::bind(&runner<T>::finalize, this, _1)
      );
  }

  void
  run()
  {
    while (m_eom->get_time(0) < m_final_time)
    {
      step();
    }
  }

  void
  run(const_reference a_time_step, const_reference a_final_time)
  {
    m_time_step = a_time_step;
    m_final_time = a_final_time;
    run();
  }
protected:
  runner()
  {
    // empty
  }

  runner(const runner& a_other)
    : m_eom(a_other.m_eom)
    , m_structure(a_other.m_structure)
    , m_integrator(a_other.m_integrator)
    , m_time_step(a_other.m_time_step)
    , m_final_time(a_other.m_final_time)
  {
    // empty
  }

  runner&
  operator=(const runner& a_other)
  {
    m_eom = a_other.m_eom;
    m_structure = a_other.m_structure;
    m_integrator = a_other.m_integrator;
    m_time_step = a_other.m_time_step;
    m_final_time = a_other.m_final_time;
    return *this;
  }

  void
  initialize(inspector_pointer a_inspector)
  {
    a_inspector->initialize(*m_eom, *m_structure);
  }

  void
  update(inspector_pointer a_inspector)
  {
    a_inspector->update(*m_eom, *m_structure);
  }

  void
  finalize(inspector_pointer a_inspector)
  {
    a_inspector->finalize(*m_eom, *m_structure);
  }
private:
  eom_pointer m_eom;
  structure_pointer m_structure;
  integrator_pointer m_integrator;
  value_type m_time_step;
  value_type m_final_time;
  std::list<inspector_pointer> m_inspectors;
}; // runner<T> class

} // yamss namespace

#endif // YAMSS_RUNNER_HPP
