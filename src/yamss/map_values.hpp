#ifndef YAMSS_MAP_VALUES_HPP
#define YAMSS_MAP_VALUES_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/unordered_map.hpp>

namespace yamss {

template <typename Key, typename Value>
class map_values_iterator
  : public boost::iterator_facade<
        map_values_iterator<Key, Value>
      , Value
      , boost::bidirectional_traversal_tag
    >
{
public:
  typedef boost::unordered_map<
        Key
      , typename boost::remove_const<Value>::type
    > map_type;
  typedef typename boost::mpl::if_<
        typename boost::is_const<Value>::type
      , typename map_type::const_iterator
      , typename map_type::iterator
    >::type iterator_type;

  map_values_iterator()
    : m_iterator(0)
  {
    // empty
  }

  explicit map_values_iterator(iterator_type a_iterator)
    : m_iterator(a_iterator)
  {
    // empty
  }
private:
  friend class boost::iterator_core_access;

  void
  decrement()
  {
    --m_iterator;
  }

  void
  increment()
  {
    ++m_iterator;
  }

  template <typename OtherValue>
  bool
  equal(const map_values_iterator<Key, OtherValue>& a_other) const
  {
    return m_iterator == a_other.m_iterator;
  }

  Value&
  dereference() const
  {
    return m_iterator->second;
  }

  iterator_type m_iterator;
}; // map_values_iterator<Key, Value> class

} // yamss namespace

#endif // YAMSS_MAP_VALUES_HPP
