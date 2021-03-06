#ifndef YAMSS_ITERATE_HPP
#define YAMSS_ITERATE_HPP

#include <armadillo>

namespace yamss {

template <typename T>
class iterate
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef arma::Col<T> vector_type;
  typedef arma::Mat<T> matrix_type;

  iterate()
    : m_step(0)
    , m_time(0)
    , m_time_step(0)
    , m_displacement()
    , m_velocity()
    , m_acceleration()
    , m_force()
  {
    // empty
  }

  iterate(size_type a_size)
    : m_step(0)
    , m_time(0)
    , m_time_step(0)
    , m_displacement(a_size)
    , m_velocity(a_size)
    , m_acceleration(a_size)
    , m_force(a_size)
  {
    m_displacement.zeros();
    m_velocity.zeros();
    m_acceleration.zeros();
    m_force.zeros();
  }

  iterate(const iterate& a_other)
    : m_step(a_other.m_step)
    , m_time(a_other.m_time)
    , m_time_step(a_other.m_time_step)
    , m_displacement(a_other.m_displacement)
    , m_velocity(a_other.m_velocity)
    , m_acceleration(a_other.m_acceleration)
    , m_force(a_other.m_force)
  {
    // empty
  }

  ~iterate()
  {
    // empty
  }

  iterate&
  operator=(const iterate& a_other)
  {
    m_step = a_other.m_step;
    m_time = a_other.m_time;
    m_time_step = a_other.m_time_step;
    m_displacement = a_other.m_displacement;
    m_velocity = a_other.m_velocity;
    m_acceleration = a_other.m_acceleration;
    m_force = a_other.m_force;
    return *this;
  }

  size_type
  get_step() const
  {
    return m_step;
  }

  const_reference
  get_time() const
  {
    return m_time;
  }

  const_reference
  get_time_step() const
  {
    return m_time_step;
  }

  const vector_type&
  get_displacement() const
  {
    return m_displacement;
  }

  const_reference
  get_displacement(size_type a_pos) const
  {
    return m_displacement(a_pos);
  }

  const vector_type&
  get_velocity() const
  {
    return m_velocity;
  }

  const_reference
  get_velocity(size_type a_pos) const
  {
    return m_velocity(a_pos);
  }

  const vector_type&
  get_acceleration() const
  {
    return m_acceleration;
  }

  const_reference
  get_acceleration(size_type a_pos) const
  {
    return m_acceleration;
  }

  const vector_type&
  get_force() const
  {
    return m_force;
  }

  const_reference
  get_force(size_type a_pos) const
  {
    return m_force(a_pos);
  }

  void
  increment_step()
  {
    m_step++;
  }

  void
  set_step(size_type a_step)
  {
    m_step = a_step;
  }

  void
  set_time(const_reference a_time)
  {
    m_time = a_time;
  }

  void
  set_time_step(const_reference a_time_step)
  {
    m_time_step = a_time_step;
  }

  void
  set_displacement(const vector_type& a_displacement)
  {
    m_displacement = a_displacement;
  }

  void
  set_displacement(size_type a_pos, const_reference a_value)
  {
    m_displacement(a_pos) = a_value;
  }

  void
  set_velocity(const vector_type& a_velocity)
  {
    m_velocity = a_velocity;
  }

  void
  set_velocity(size_type a_pos, const_reference a_value)
  {
    m_velocity(a_pos) = a_value;
  }

  void
  set_acceleration(const vector_type& a_acceleration)
  {
    m_acceleration = a_acceleration;
  }

  void
  set_acceleration(size_type a_pos, const_reference a_value)
  {
    m_acceleration(a_pos) = a_value;
  }

  void
  set_force(const vector_type& a_force)
  {
    m_force = a_force;
  }

  void
  set_force(size_type a_pos, const_reference a_value)
  {
    m_force(a_pos) = a_value;
  }
private:
  size_type m_step;
  value_type m_time;
  value_type m_time_step;
  vector_type m_displacement;
  vector_type m_velocity;
  vector_type m_acceleration;
  vector_type m_force;
}; // iterate<T> class

} // yamss namespace

#endif // YAMSS_ITERATE_HPP
