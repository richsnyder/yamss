#ifndef YAMSS_NEWMARK_BETA_HPP
#define YAMSS_NEWMARK_BETA_HPP

#include <armadillo>
#include "yamss/integrator/integrator.hpp"

namespace yamss {
namespace integrator {

template <typename T = double>
class newmark_beta : public integrator<T>
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  newmark_beta()
    : m_beta(0.25)
    , m_gamma(0.5)
  {
    // empty
  }

  newmark_beta(const boost::property_tree::ptree& a_tree)
  {
    m_beta = a_tree.get<value_type>("beta", 0.25);
    m_gamma = a_tree.get<value_type>("gamma", 0.5);
  }

  ~newmark_beta()
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

    value_type a0 = 1.0 / (m_beta * dt * dt);
    value_type a1 = m_gamma / (m_beta * dt);
    value_type a2 = 1.0 / (m_beta * dt);
    value_type a3 = 1.0 / (2.0 * m_beta) - 1.0;
    value_type a4 = m_gamma / m_beta - 1.0;
    value_type a5 = 0.5 * dt * (m_gamma / m_beta - 2.0);
    value_type a6 = dt * (1.0 - m_gamma);
    value_type a7 = dt * m_gamma;

    a_structure.apply_loads(t);
    a_eom.set_force(a_structure.get_generalized_force());
    const vector_type& f = a_eom.get_force(0);

    vector_type v = a0 * u + a2 * du + a3 * ddu;
    vector_type w = a1 * u + a4 * du + a5 * ddu;

    matrix_type k_eff = k + a0 * m + a1 * c;
    vector_type f_eff = f + m * v + c * w;

    vector_type u_new;
    arma::solve(u_new, k_eff, f_eff);
    vector_type ddu_new = a0 * (u_new - u) - a2 * du - a3 * ddu;
    vector_type du_new = du + a6 * ddu + a7 * ddu_new;

    a_eom.set_displacement(u_new);
    a_eom.set_velocity(du_new);
    a_eom.set_acceleration(ddu_new);
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

  newmark_beta(const newmark_beta& a_other)
    : m_beta(a_other.m_beta)
    , m_gamma(a_other.m_gamma)
  {
    // empty
  }

  newmark_beta&
  operator=(const newmark_beta& a_other)
  {
    m_beta = a_other.m_beta;
    m_gamma = a_other.m_gamma;
    return *this;
  }

  value_type m_beta;
  value_type m_gamma;
}; // newmark_beta<T> class

} // integrator namespace
} // yamss namespace

#endif // YAMSS_NEWMARK_BETA_HPP
