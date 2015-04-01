#ifndef YAMSS_EVALUATOR_HPP
#define YAMSS_EVALUATOR_HPP

#include <armadillo>
#include <boost/property_tree/ptree.hpp>
#include "yamss/node.hpp"

namespace yamss {
namespace evaluator {

template <typename T>
class evaluator
{
public:
  typedef T value_type;
  typedef node<T> node_type;
  typedef arma::Col<T> vector_type;

  virtual
  bool
  is_interface() const
  {
    return false;
  }

  virtual
  vector_type
  operator()(const value_type& a_time, const node_type& a_node) = 0;
}; // evaluator<T> class

} // evaluator namespace
} // yamss namespace

#endif // YAMSS_EVALUATOR_HPP
