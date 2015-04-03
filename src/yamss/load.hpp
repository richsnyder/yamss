#ifndef YAMSS_LOAD_HPP
#define YAMSS_LOAD_HPP

#include <algorithm>
#include <armadillo>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include "yamss/element.hpp"
#include "yamss/evaluator/evaluator.hpp"

namespace yamss {

template <typename T>
class load
{
protected:
  typedef boost::unordered_set<size_t> set_type;
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef size_t size_type;
  typedef node<T> node_type;
  typedef element element_type;
  typedef evaluator::evaluator<T> evaluator_type;
  typedef boost::shared_ptr<evaluator_type> evaluator_pointer;
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
    , m_elements(a_other.m_elements)
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
    m_elements = a_other.m_elements;
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

  size_type
  get_number_of_elements() const
  {
    return m_elements.size();
  }

  size_type
  get_number_of_nodes() const
  {
    return m_nodes.size();
  }

  void
  add_element(const element_type& a_element)
  {
    typedef typename element_type::vector_type vertices_type;
    typedef typename vertices_type::const_iterator const_iterator;

    m_elements.insert(a_element.get_key());
    const vertices_type& vertices = a_element.get_vertices();
    for (const_iterator p = vertices.begin(); p != vertices.end(); ++p)
    {
      m_nodes.insert(*p);
    }
  }

  template <typename Iterator>
  void
  add_elements(Iterator a_begin, Iterator a_end)
  {
    std::for_each(a_begin, a_end, boost::bind(&load<T>::add_element, this, _1));
  }

  const_iterator
  begin_elements() const
  {
    return m_elements.begin();
  }

  const_iterator
  end_elements() const
  {
    return m_elements.end();
  }

  const_iterator
  begin_nodes() const
  {
    return m_nodes.begin();
  }

  const_iterator
  end_nodes() const
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
  set_type m_elements;
  evaluator_pointer m_evaluator;
}; // load<T> class

} // yamss namespace

#endif // YAMSS_LOAD_HPP
