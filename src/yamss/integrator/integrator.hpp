#ifndef YAMSS_INTEGRATOR_HPP
#define YAMSS_INTEGRATOR_HPP

#include <boost/property_tree/ptree.hpp>
#include "yamss/eom.hpp"
#include "yamss/structure.hpp"

namespace yamss {
namespace integrator {

template <typename T>
class integrator
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  virtual
  void
  operator()(eom_type& a_eom, structure_type& a_structure) = 0;

  virtual
  size_type
  stencil_size() const = 0;
}; // integrator<T> class

} // integrator namespace
} // yamss namespace

#endif // YAMSS_INTEGRATOR_HPP
