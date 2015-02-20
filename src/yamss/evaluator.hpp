#ifndef YAMSS_EVALUATOR_HPP
#define YAMSS_EVALUATOR_HPP

#include <eigen3/Eigen/Dense>
#include "yamss/node.hpp"

namespace yamss {

template <typename T>
class evaluator
{
public:
  typedef T value_type;
  typedef node<T> node_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> vector_type;

  virtual
  vector_type
  operator()(const value_type& a_time, const node_type& a_node) = 0;
}; // evaluator<T> class

} // yamss namespace

#endif // YAMSS_EVALUATOR_HPP
