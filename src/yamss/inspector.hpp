#ifndef YAMSS_INSPECTOR_HPP
#define YAMSS_INSPECTOR_HPP

#include "yamss/eom.hpp"
#include "yamss/structure.hpp"

namespace yamss {

template <typename T>
class inspector
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;

  virtual
  void
  initialize(const eom_type& a_eom, const structure_type& a_structure) = 0;

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure) = 0;

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure) = 0;
}; // inspector<Types> class

} // yamss namespace

#endif // YAMSS_INSPECTOR_HPP
