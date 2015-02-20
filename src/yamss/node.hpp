#ifndef YAMSS_NODE_HPP
#define YAMSS_NODE_HPP

#include <eigen3/Eigen/Dense>

namespace yamss {

template <typename T>
class node
{
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> vector_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrix_type;
  typedef Eigen::Matrix<T, 6, 1> dof_vector_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 6> dof_matrix_type;

  node(key_type a_key, size_type a_number_of_modes)
    : m_key(a_key)
    , m_position(dof_vector_type::Zero())
    , m_force(dof_vector_type::Zero())
    , m_modes(dof_matrix_type::Zero(a_number_of_modes, 6))
  {
    // empty
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

  const dof_vector_type&
  get_position() const
  {
    return m_position;
  }

  const_reference
  get_position(size_type a_dof) const
  {
    return m_position(a_dof);
  }

  const dof_vector_type&
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

  const dof_matrix_type&
  get_modes() const
  {
    return m_modes;
  }

  template <typename Derived>
  void
  set_position(const Eigen::MatrixBase<Derived>& a_position)
  {
    m_position = a_position;
  }

  void
  set_position(size_type a_dof, const_reference a_value)
  {
    m_position(a_dof) = a_value;
  }

  template <typename Derived>
  void
  set_force(const Eigen::MatrixBase<Derived>& a_force)
  {
    m_force = a_force;
  }

  void
  set_force(size_type a_dof, const_reference a_value)
  {
    m_force(a_dof) = a_value;
  }

  template <typename Derived>
  void
  set_mode(size_type a_mode, const Eigen::MatrixBase<Derived>& a_shape)
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
    m_force = dof_vector_type::Zero();
  }

  template <typename Derived>
  void
  add_force(const Eigen::MatrixBase<Derived>& a_force)
  {
    m_force += a_force;
  }

  void
  add_force(size_type a_dof, const_reference a_value)
  {
    m_force(a_dof) += a_value;
  }

  template <typename Derived>
  vector_type
  get_generalized_force(const Eigen::MatrixBase<Derived>& a_active) const
  {
    return m_modes * a_active.asDiagonal() * m_force;
  }
private:
  node()
  {
    // empty
  }

  key_type m_key;
  dof_vector_type m_position;
  dof_vector_type m_force;
  dof_matrix_type m_modes;
}; // node<T> class

} // yamss namespace

#endif // YAMSS_NODE_HPP
