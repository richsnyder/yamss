#ifndef YAMSS_EOM_HPP
#define YAMSS_EOM_HPP

#include <vector>
#include <armadillo>
#include "yamss/iterate.hpp"

namespace yamss {

template <typename T = double>
class eom
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef arma::Col<T> vector_type;
  typedef arma::Mat<T> matrix_type;

  eom(size_type a_dofs, size_type a_steps)
    : m_mass(a_dofs, a_dofs)
    , m_damping(a_dofs, a_dofs)
    , m_stiffness(a_dofs, a_dofs)
    , m_iterates(a_steps, iterate_type(a_dofs))
  {
    m_mass.eye();
    m_damping.eye();
    m_stiffness.eye();
  }

  eom(const eom& a_other)
    : m_mass(a_other.m_mass)
    , m_damping(a_other.m_damping)
    , m_stiffness(a_other.m_stiffness)
    , m_iterates(a_other.m_iterates)
  {
    // empty
  }

  ~eom()
  {
    // empty
  }

  eom&
  operator=(const eom& a_other)
  {
    m_mass = a_other.m_mass;
    m_damping = a_other.m_damping;
    m_stiffness = a_other.m_stiffness;
    m_iterates = a_other.m_iterates;
    return *this;
  }

  size_type
  get_size() const
  {
    return m_mass.n_rows;
  }

  const matrix_type&
  get_mass() const
  {
    return m_mass;
  }

  const matrix_type&
  get_damping() const
  {
    return m_damping;
  }

  const matrix_type&
  get_stiffness() const
  {
    return m_stiffness;
  }

  const_reference
  get_time(size_type a_step) const
  {
    return m_iterates[a_step].get_time();
  }

  const_reference
  get_time_step(size_type a_step) const
  {
    return m_iterates[a_step].get_time_step();
  }

  const vector_type&
  get_displacement(size_type a_step) const
  {
    return m_iterates[a_step].get_displacement();
  }

  const vector_type&
  get_velocity(size_type a_step) const
  {
    return m_iterates[a_step].get_velocity();
  }

  const vector_type&
  get_acceleration(size_type a_step) const
  {
    return m_iterates[a_step].get_acceleration();
  }

  const vector_type&
  get_force(size_type a_step) const
  {
    return m_iterates[a_step].get_force();
  }

  void
  set_mass(const matrix_type& a_mass)
  {
    m_mass = a_mass;
  }

  void
  set_mass(size_type a_row, size_type a_col, const_reference a_value)
  {
    m_mass(a_row, a_col) = a_value;
  }

  void
  set_damping(const matrix_type& a_damping)
  {
    m_damping = a_damping;
  }

  void
  set_damping(size_type a_row, size_type a_col, const_reference a_value)
  {
    m_damping(a_row, a_col) = a_value;
  }

  void
  set_stiffness(const matrix_type& a_stiffness)
  {
    m_stiffness = a_stiffness;
  }

  void
  set_stiffness(size_type a_row, size_type a_col, const_reference a_value)
  {
    m_stiffness(a_row, a_col) = a_value;
  }

  void
  set_displacement(const vector_type& a_displacement)
  {
    m_iterates[0].set_displacement(a_displacement);
  }

  void
  set_displacement(size_type a_dof, const_reference a_value)
  {
    m_iterates[0].set_displacement(a_dof, a_value);
  }

  void
  set_velocity(const vector_type& a_velocity)
  {
    m_iterates[0].set_velocity(a_velocity);
  }

  void
  set_velocity(size_type a_dof, const_reference a_value)
  {
    m_iterates[0].set_velocity(a_dof, a_value);
  }

  void
  set_acceleration(const vector_type& a_acceleration)
  {
    m_iterates[0].set_acceleration(a_acceleration);
  }

  void
  set_acceleration(size_type a_dof, const_reference a_value)
  {
    m_iterates[0].set_acceleration(a_dof, a_value);
  }

  void
  set_force(const vector_type& a_force)
  {
    m_iterates[0].set_force(a_force);
  }

  void
  set_force(size_type a_dof, const_reference a_value)
  {
    m_iterates[0].set_force(a_dof, a_value);
  }

  void
  compute_acceleration()
  {
    const iterate_type& iterate = m_iterates[0];
    vector_type lhs;
    vector_type rhs = iterate.get_force()
        - m_damping * iterate.get_velocity()
        - m_stiffness * iterate.get_displacement();
    arma::solve(lhs, m_mass, rhs);
    m_iterates[0].set_acceleration(lhs);
  }

  void
  advance(const_reference a_time_step)
  {
    if (m_iterates.size() > 1)
    {
      for (size_type n = m_iterates.size() - 1; n > 0; --n)
      {
        m_iterates[n] = m_iterates[n - 1];
      }
    }
    if (m_iterates.size() > 0)
    {
      m_iterates[0].set_time_step(a_time_step);
    }
    if (m_iterates.size() > 1)
    {
      m_iterates[0].set_time(m_iterates[1].get_time() + a_time_step);
    }
  }
private:
  typedef iterate<T> iterate_type;
  typedef std::vector<iterate_type> iterates_type;

  eom()
  {
    // empty
  }

  matrix_type m_mass;
  matrix_type m_damping;
  matrix_type m_stiffness;
  iterates_type m_iterates;
}; // eom<T> class

} // yamss namespace

#endif // YAMSS_EOM_HPP
