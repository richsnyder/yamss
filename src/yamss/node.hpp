#ifndef YAMSS_NODE_HPP
#define YAMSS_NODE_HPP

#include <armadillo>

namespace yamss {

template <typename T>
class node
{
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef arma::Col<T> vector_type;
  typedef arma::Mat<T> matrix_type;

  node(key_type a_key, size_type a_number_of_modes)
    : m_key(a_key)
    , m_position(6)
    , m_force(6)
    , m_modes(a_number_of_modes, 6)
  {
    m_position.zeros();
    m_force.zeros();
    m_modes.zeros();
  }

  node(const node& a_other)
    : m_key(a_other.m_key)
    , m_position(a_other.m_position)
    , m_force(a_other.m_force)
    , m_modes(a_other.m_modes)
  {
    // empty
  }

  ~node()
  {
    // empty
  }

  node&
  operator=(const node& a_other)
  {
    m_key = a_other.m_key;
    m_position = a_other.m_position;
    m_force = a_other.m_force;
    m_modes = a_other.m_modes;
    return *this;
  }

  key_type
  get_key() const
  {
    return m_key;
  }

  const vector_type&
  get_position() const
  {
    return m_position;
  }

  const_reference
  get_position(size_type a_dof) const
  {
    return m_position(a_dof);
  }

  const vector_type&
  get_force() const
  {
    return m_force;
  }

  const_reference
  get_force(size_type a_dof) const
  {
    return m_force(a_dof);
  }

  const_reference
  get_mode(size_type a_mode, size_type a_dof) const
  {
    return m_modes(a_mode, a_dof);
  }

  const matrix_type&
  get_modes() const
  {
    return m_modes;
  }

  void
  set_position(const vector_type& a_position)
  {
    m_position = a_position;
  }

  void
  set_position(size_type a_dof, const_reference a_value)
  {
    m_position(a_dof) = a_value;
  }

  void
  set_force(const vector_type& a_force)
  {
    m_force = a_force;
  }

  void
  set_force(size_type a_dof, const_reference a_value)
  {
    m_force(a_dof) = a_value;
  }

  void
  set_mode(size_type a_mode, const vector_type& a_shape)
  {
    m_modes.row(a_mode) = a_shape;
  }

  void
  set_mode(size_type a_mode, size_type a_dof, const_reference a_value)
  {
    m_modes(a_mode, a_dof) = a_value;
  }

  void
  clear_force()
  {
    m_force.zeros();
  }

  void
  add_force(const vector_type& a_force)
  {
    m_force += a_force;
  }

  void
  add_force(size_type a_dof, const_reference a_value)
  {
    m_force(a_dof) += a_value;
  }

  vector_type
  get_generalized_force(const vector_type& a_active) const
  {
    return m_modes * arma::diagmat(a_active) * m_force;
  }
private:
  node()
  {
    // empty
  }

  key_type m_key;
  vector_type m_position;
  vector_type m_force;
  matrix_type m_modes;
}; // node<T> class

} // yamss namespace

#endif // YAMSS_NODE_HPP
