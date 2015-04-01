#ifndef YAMSS_EVALUATOR_INTERFACE_HPP
#define YAMSS_EVALUATOR_INTERFACE_HPP

#include "yamss/evaluator/evaluator.hpp"

namespace yamss {
namespace evaluator {

template <typename T = double>
class interface : public evaluator<T>
{
public:
  typedef T value_type;
  typedef node<T> node_type;
  typedef arma::Col<T> vector_type;

  interface()
  {
    // empty
  }

  interface(const boost::property_tree::ptree& a_tree)
  {
    // empty
  }

  virtual
  bool
  is_interface() const
  {
    return true;
  }

  virtual
  vector_type
  operator()(const value_type& a_time, const node_type& a_node)
  {
    vector_type vec(6);
    vec.zeros();
    return vec;
  }
}; // interface<T> class

} // evaluator namespace
} // yamss namespace

#endif // YAMSS_EVALUATOR_INTERFACE_HPP
