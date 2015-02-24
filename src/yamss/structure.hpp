#ifndef YAMSS_STRUCTURE_HPP
#define YAMSS_STRUCTURE_HPP

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/unordered_map.hpp>
#include "yamss/load.hpp"
#include "yamss/node.hpp"

namespace yamss {

template <typename T = double>
class structure
{
public:
  typedef T value_type;
  typedef size_t key_type;
  typedef size_t size_type;
  typedef const T& const_reference;
  typedef node<T> node_type;
  typedef load<T> load_type;
  typedef arma::Col<T> vector_type;

  structure(size_type a_number_of_modes)
    : m_number_of_modes(a_number_of_modes)
    , m_active_dofs(6)
  {
    m_active_dofs.ones();
  }

  void
  activate_dof(size_type a_dof)
  {
    m_active_dofs(a_dof) = 1.0;
  }

  void
  deactivate_dof(size_type a_dof)
  {
    m_active_dofs(a_dof) = 0.0;
  }

  bool
  is_active(size_type a_dof) const
  {
    return m_active_dofs(a_dof) > 0.5 ? true : false;
  }

  node_type&
  add_node(key_type a_key)
  {
    typedef typename nodes_type::iterator iterator;

    std::pair<iterator, bool> result = m_nodes.insert(std::make_pair(
        a_key, node_type(a_key, m_number_of_modes)));
    if (result.second)
    {
      return result.first->second;
    }
    else
    {
      boost::format fmt("Failed to add node %1% to the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
  }

  node_type&
  get_node(key_type a_key)
  {
    typedef typename nodes_type::iterator iterator;

    iterator result = m_nodes.find(a_key);
    if (result == m_nodes.end())
    {
      boost::format fmt("Failed to find node %1% in the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  const node_type&
  get_node(key_type a_key) const
  {
    typedef typename nodes_type::const_iterator const_iterator;

    const_iterator result = m_nodes.find(a_key);
    if (result == m_nodes.end())
    {
      boost::format fmt("Failed to find node %1% in the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  template <typename Fn>
  load_type&
  add_load(key_type a_key, Fn a_function)
  {
    typedef typename loads_type::iterator iterator;

    std::pair<iterator, bool> result = m_loads.insert(std::make_pair(
        a_key, load_type(a_key, a_function)));
    if (result.second)
    {
      return result.first->second;
    }
    else
    {
      boost::format fmt("Failed to add load %1% to the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
  }

  load_type&
  get_load(key_type a_key)
  {
    typedef typename loads_type::iterator iterator;

    iterator result = m_nodes.find(a_key);
    if (result == m_nodes.end())
    {
      boost::format fmt("Failed to find load %1% on the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  const load_type&
  get_load(key_type a_key) const
  {
    typedef typename load_type::const_iterator const_iterator;

    const_iterator result = m_loads.find(a_key);
    if (result == m_nodes.end())
    {
      boost::format fmt("Failed to find load %1% on the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  void
  clear_loads()
  {
    typedef typename nodes_type::iterator iterator;

    iterator node_iterator = m_nodes.begin();
    while (node_iterator != m_nodes.end())
    {
      node_iterator->second.clear_force();
      ++node_iterator;
    }
  }

  void
  apply_loads(const_reference a_time)
  {
    typename load_type::const_iterator key_iter;
    typename loads_type::const_iterator load_iter;
    typename nodes_type::iterator node_iter;

    clear_loads();
    for (load_iter = m_loads.begin(); load_iter != m_loads.end(); ++load_iter)
    {
      const load_type& load_ = load_iter->second;
      for (key_iter = load_.begin(); key_iter != load_.end(); ++key_iter)
      {
        node_iter = m_nodes.find(*key_iter);
        if (node_iter != m_nodes.end())
        {
          node_type& node_ = node_iter->second;
          node_.add_force(load_.apply(a_time, node_));
        }
      }
    }
  }

  vector_type
  get_generalized_force()
  {
    typedef typename nodes_type::const_iterator const_iterator;

    vector_type f(m_number_of_modes);
    f.zeros();
    const_iterator node_iterator = m_nodes.begin();
    while (node_iterator != m_nodes.end())
    {
      f += node_iterator->second.get_generalized_force(m_active_dofs);
      ++node_iterator;
    }
    return f;
  }
private:
  typedef boost::unordered_map<key_type, node_type> nodes_type;
  typedef boost::unordered_map<key_type, load_type> loads_type;

  structure()
  {
    // empty
  }

  size_type m_number_of_modes;
  vector_type m_active_dofs;
  nodes_type m_nodes;
  loads_type m_loads;
}; // structure<T> class

} // yamss namespace

#endif // YAMSS_STRUCTURE_HPP
