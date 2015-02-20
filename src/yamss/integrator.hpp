#ifndef YAMSS_INTEGRATOR_HPP
#define YAMSS_INTEGRATOR_HPP

#include "yamss/eom.hpp"

namespace yamss {

template <typename T>
class integrator
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef eom<T> eom_type;

  virtual
  void
  operator()(eom_type& a_eom) = 0;

  virtual
  size_type
  stencil_size() const = 0;
}; // integrator<Types> class

} // yamss namespace

#endif // YAMSS_INTEGRATOR_HPP
