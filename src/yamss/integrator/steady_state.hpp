#ifndef YAMSS_STEADY_STATE_HPP
#define YAMSS_STEADY_STATE_HPP

#include <armadillo>
#include "yamss/integrator/integrator.hpp"

namespace yamss {
namespace integrator {

template <typename T = double>
class steady_state : public integrator<T>
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  steady_state()
  {
    // empty
  }

  steady_state(const boost::property_tree::ptree& a_tree)
  {
    // empty
  }

  ~steady_state()
  {
    // empty
  }

  virtual
  void
  operator()(eom_type& a_eom, structure_type& a_structure)
  {
    const value_type t = a_eom.get_time(0);
    const matrix_type& k = a_eom.get_stiffness();

    a_structure.apply_loads(t);
    a_eom.set_force(a_structure.get_generalized_force());
    const vector_type& f = a_eom.get_force(0);

    vector_type u;
    arma::solve(u, k, f);
    a_eom.set_displacement(u);

    u.zeros();
    a_eom.set_velocity(u);
    a_eom.set_acceleration(u);
  }

  virtual
  size_type
  stencil_size() const
  {
    return 1;
  }
private:
  typedef arma::Col<T> vector_type;
  typedef arma::Mat<T> matrix_type;
}; // steady_state<T> class

} // integrator namespace
} // yamss namespace

#endif // YAMSS_STEADY_STATE_HPP
