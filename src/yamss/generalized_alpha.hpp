#ifndef YAMSS_GENERALIZED_ALPHA_HPP
#define YAMSS_GENERALIZED_ALPHA_HPP

#include <armadillo>
#include "yamss/integrator.hpp"

namespace yamss {

template <typename T = double>
class generalized_alpha : public integrator<T>
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  generalized_alpha()
    : m_alpha_m(2.0 / 7.0)
    , m_alpha_f(3.0 / 7.0)
    , m_beta(1.0 / 49.0)
    , m_gamma(9.0 / 14.0)
  {
    // empty
  }

  generalized_alpha(const_reference a_alpha_m,
                    const_reference a_alpha_f,
                    const_reference a_beta,
                    const_reference a_gamma)
    : m_alpha_m(a_alpha_m)
    , m_alpha_f(a_alpha_f)
    , m_beta(a_beta)
    , m_gamma(a_gamma)
  {
    // empty
  }

  ~generalized_alpha()
  {
    // empty
  }

  virtual
  void
  operator()(eom_type& a_eom, structure_type& a_structure)
  {
    const value_type t = a_eom.get_time(0);
    const value_type dt = a_eom.get_time_step(0);
    const matrix_type& m = a_eom.get_mass();
    const matrix_type& c = a_eom.get_damping();
    const matrix_type& k = a_eom.get_stiffness();
    const vector_type& u = a_eom.get_displacement(1);
    const vector_type& du = a_eom.get_velocity(1);
    const vector_type& ddu = a_eom.get_acceleration(1);

    value_type k0 = 1.0 / (1.0 - m_alpha_f);
    value_type k1 = k0 * m_alpha_m;
    value_type k2 = k0 * m_alpha_f;
    value_type k3 = k0 * (1.0 - m_alpha_m);
    value_type a0 = dt * (1.0 - m_gamma);
    value_type a1 = dt - a0;
    value_type b0 = 1.0 / (m_beta * dt * dt);
    value_type b1 = dt * b0;
    value_type b2 = 1.0 / (2.0 * m_beta) - 1.0;
    value_type c0 = k3 * b0;
    value_type c1 = m_gamma * b1;
    value_type c2 = k3 * b1;
    value_type c3 = k3 * b2 - k1;
    value_type c4 = m_gamma / m_beta - 1.0 - k2;
    value_type c5 = 0.5 * dt * (m_gamma / m_beta - 2.0);
    value_type c6 = -k2;
    value_type c7 = k0;

    a_structure.apply_loads(t - m_alpha_f * dt);
    vector_type f = c7 * a_structure.get_generalized_force();

    vector_type p = c0 * u + c2 * du + c3 * ddu;
    vector_type q = c1 * u + c4 * du + c5 * ddu;
    vector_type r = c6 * u;

    matrix_type k_eff = k + c0 * m + c1 * c;
    vector_type f_eff = f + m * p + c * q + k * r;

    vector_type u_new;
    arma::solve(u_new, k_eff, f_eff);
    vector_type ddu_new = b0 * (u_new - u) - b1 * du - b2 * ddu;
    vector_type du_new = du + a0 * ddu + a1 * ddu_new;

    a_eom.set_displacement(u_new);
    a_eom.set_velocity(du_new);
    a_eom.set_acceleration(ddu_new);
    a_eom.compute_force();
  }

  virtual
  size_t
  stencil_size() const
  {
    return 2;
  }
private:
  typedef arma::Col<T> vector_type;
  typedef arma::Mat<T> matrix_type;

  generalized_alpha(const generalized_alpha& a_other)
    : m_alpha_m(a_other.m_alpha_m)
    , m_alpha_f(a_other.m_alpha_f)
    , m_beta(a_other.m_beta)
    , m_gamma(a_other.m_gamma)
  {
    // empty
  }

  generalized_alpha&
  operator=(const generalized_alpha& a_other)
  {
    m_alpha_m = a_other.m_alpha_m;
    m_alpha_f = a_other.m_alpha_f;
    m_beta = a_other.m_beta;
    m_gamma = a_other.m_gamma;
    return *this;
  }

  value_type m_alpha_m;
  value_type m_alpha_f;
  value_type m_beta;
  value_type m_gamma;
}; // generalized_alpha<T> class

} // yamss namespace

#endif // YAMSS_GENERALIZED_ALPHA_HPP
