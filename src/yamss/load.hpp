#ifndef YAMSS_LOAD_HPP
#define YAMSS_LOAD_HPP

#include <algorithm>
#include <armadillo>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include "yamss/evaluator/evaluator.hpp"

namespace yamss {

template <typename T>
class load
{
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef node<T> node_type;
  typedef evaluator::evaluator<T> evaluator_type;
  typedef boost::shared_ptr<evaluator_type> evaluator_pointer;
  typedef boost::unordered_set<size_t> set_type;
  typedef arma::Col<T> vector_type;
  typedef typename set_type::const_iterator const_iterator;

  load(key_type a_key, const boost::shared_ptr<evaluator_type>& a_evaluator)
    : m_key(a_key)
    , m_evaluator(a_evaluator)
  {
    // empty
  }

  load(const load& a_other)
    : m_key(a_other.m_key)
    , m_nodes(a_other.m_nodes)
    , m_evaluator(a_other.m_evaluator)
  {
    // empty
  }

  ~load()
  {
    // empty
  }

  load&
  operator=(const load& a_other)
  {
    m_key = a_other.m_key;
    m_nodes = a_other.m_nodes;
    m_evaluator = a_other.m_evaluator;
    return *this;
  }

  key_type
  get_key() const
  {
    return m_key;
  }

  evaluator_pointer
  get_evaluator() const
  {
    return m_evaluator;
  }

  void
  add_node(key_type a_key)
  {
    m_nodes.insert(a_key);
  }

  template <typename Iterator>
  void
  add_nodes(Iterator a_begin, Iterator a_end)
  {
    std::copy(a_begin, a_end, m_nodes.begin());
  }

  const_iterator
  begin() const
  {
    return m_nodes.begin();
  }

  const_iterator
  end() const
  {
    return m_nodes.end();
  }

  vector_type
  apply(const value_type& a_time, const node_type& a_node) const
  {
    return m_evaluator->operator()(a_time, a_node);
  }
private:
  load()
  {
    // empty
  }

  key_type m_key;
  set_type m_nodes;
  evaluator_pointer m_evaluator;
}; // load<T> class

} // yamss namespace

#endif // YAMSS_LOAD_HPP
