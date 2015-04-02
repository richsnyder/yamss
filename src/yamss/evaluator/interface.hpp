#ifndef YAMSS_EVALUATOR_INTERFACE_HPP
#define YAMSS_EVALUATOR_INTERFACE_HPP

#include <boost/unordered_map.hpp>
#include "yamss/evaluator/evaluator.hpp"

namespace yamss {
namespace evaluator {

template <typename T = double>
class interface : public evaluator<T>
{
public:
  typedef T value_type;
  typedef size_t key_type;
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
  vector_type
  operator()(const value_type& a_time, const node_type& a_node)
  {
    key_type key = a_node.get_key();
    const_iterator p = m_loads.find(key);
    if (p == m_loads.end())
    {
      return vector_type(6, arma::fill::zeros);
    }
    else
    {
      return p->second;
    }
  }

  void
  insert(const key_type& a_key, const vector_type& a_load)
  {
    m_loads[a_key] = a_load;
  }
private:
  typedef boost::unordered_map<key_type, vector_type> map_type;
  typedef typename map_type::const_iterator const_iterator;

  map_type m_loads;
}; // interface<T> class

} // evaluator namespace
} // yamss namespace

#endif // YAMSS_EVALUATOR_INTERFACE_HPP
