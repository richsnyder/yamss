#ifndef YAMSS_STRUCTURE_HPP
#define YAMSS_STRUCTURE_HPP

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/unordered_map.hpp>
#include "yamss/element.hpp"
#include "yamss/load.hpp"
#include "yamss/map_values.hpp"
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
  typedef map_values_iterator<key_type, node_type> node_iterator;
  typedef map_values_iterator<key_type, const node_type> const_node_iterator;
  typedef element element_type;
  typedef map_values_iterator<key_type, element_type> element_iterator;
  typedef map_values_iterator<key_type, const element_type> const_element_iterator;
  typedef load<T> load_type;
  typedef map_values_iterator<key_type, load_type> load_iterator;
  typedef map_values_iterator<key_type, const load_type> const_load_iterator;
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

  std::vector<bool>
  get_active_dofs() const
  {
    std::vector<bool> active(6, false);
    for (size_type n = 0; n < 6; ++n)
    {
      if (is_active(n))
      {
        active[n] = true;
      }
    }
    return active;
  }

  size_type
  get_number_of_active_dofs() const
  {
    size_type result = 0;
    for (size_type n = 0; n < 6; ++n)
    {
      result += m_active_dofs(n) > 0.5 ? 1 : 0;
    }
    return result;
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

  size_type
  get_number_of_nodes() const
  {
    return m_nodes.size();
  }

  node_iterator
  begin_nodes()
  {
    return node_iterator(m_nodes.begin());
  }

  node_iterator
  end_nodes()
  {
    return node_iterator(m_nodes.end());
  }

  const_node_iterator
  begin_nodes() const
  {
    return const_node_iterator(m_nodes.begin());
  }

  const_node_iterator
  end_nodes() const
  {
    return const_node_iterator(m_nodes.end());
  }

  element_type&
  add_element(key_type a_key, element::shape_type a_shape)
  {
    typedef typename elements_type::iterator iterator;

    std::pair<iterator, bool> result = m_elements.insert(std::make_pair(
        a_key, element_type(a_key, a_shape)));
    if (result.second)
    {
      return result.first->second;
    }
    else
    {
      boost::format fmt("Failed to add element %1% to the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
  }

  element_type&
  get_element(key_type a_key)
  {
    typedef typename elements_type::iterator iterator;

    iterator result = m_elements.find(a_key);
    if (result == m_elements.end())
    {
      boost::format fmt("Failed to find element %1% in the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  vector_type
  get_element_normal(const element_type& a_element)
  {
    auto n_vertices = a_element.get_size();
    vector_type normal = {0.0, 0.0, 1.0};

    if (n_vertices > 2)
    {
      const auto& node_0 = get_node(a_element.get_vertex(0));
      const auto& node_1 = get_node(a_element.get_vertex(1));
      const auto& node_n = get_node(a_element.get_vertex(n_vertices - 1));
      const auto& x_0 = node_0.get_position();
      const auto& x_1 = node_1.get_position();
      const auto& x_n = node_n.get_position();
      auto v_0 = x_0.head(3);
      auto v_1 = x_1.head(3);
      auto v_n = x_n.head(3);

      normal = ::arma::normalise(::arma::cross(v_1 - v_0, v_n - v_0));
    }

    return normal;
  }

  vector_type
  get_element_normal(key_type a_key)
  {
    return get_element_normal(get_element(a_key));
  }

  value_type
  get_element_area(const element_type& a_element)
  {
    auto n_vertices = a_element.get_size();
    auto area = 0.0;

    if (n_vertices > 2)
    {
      vector_type v_a;
      vector_type v_b;
      vector_type summed = ::arma::zeros<vector_type>(3);
      auto normal = get_element_normal(a_element);

      v_b = get_node(a_element.get_vertex(0)).get_position().head(3);
      for (auto n = 1; n < n_vertices; ++n)
      {
        v_a = v_b;
        v_b = get_node(a_element.get_vertex(n)).get_position().head(3);
        summed += ::arma::cross(v_a, v_b);
      }
      v_a = v_b;
      v_b = get_node(a_element.get_vertex(0)).get_position().head(3);
      summed += ::arma::cross(v_a, v_b);
      area = 0.5 * ::arma::dot(normal, summed);
    }

    return area;
  }

  value_type
  get_element_area(key_type a_key)
  {
    return get_element_area(get_element(a_key));
  }

  const element_type&
  get_element(key_type a_key) const
  {
    typedef typename elements_type::const_iterator const_iterator;

    const_iterator result = m_elements.find(a_key);
    if (result == m_elements.end())
    {
      boost::format fmt("Failed to find element %1% in the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  size_type
  get_number_of_elements() const
  {
    return m_elements.size();
  }

  element_iterator
  begin_elements()
  {
    return element_iterator(m_elements.begin());
  }

  element_iterator
  end_elements()
  {
    return element_iterator(m_elements.end());
  }

  const_element_iterator
  begin_elements() const
  {
    return const_element_iterator(m_elements.begin());
  }

  const_element_iterator
  end_elements() const
  {
    return const_element_iterator(m_elements.end());
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

    iterator result = m_loads.find(a_key);
    if (result == m_loads.end())
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
    if (result == m_loads.end())
    {
      boost::format fmt("Failed to find load %1% on the structure.");
      throw std::runtime_error(boost::str(fmt % a_key));
    }
    else
    {
      return result->second;
    }
  }

  load_iterator
  begin_loads()
  {
    return load_iterator(m_loads.begin());
  }

  load_iterator
  end_loads()
  {
    return load_iterator(m_loads.end());
  }

  const_load_iterator
  begin_loads() const
  {
    return const_load_iterator(m_loads.begin());
  }

  const_load_iterator
  end_loads() const
  {
    return const_load_iterator(m_loads.end());
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
      for (key_iter = load_.begin_nodes();
           key_iter != load_.end_nodes();
           ++key_iter)
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
  typedef boost::unordered_map<key_type, element_type> elements_type;

  structure()
  {
    // empty
  }

  size_type m_number_of_modes;
  vector_type m_active_dofs;
  nodes_type m_nodes;
  loads_type m_loads;
  elements_type m_elements;
}; // structure<T> class

} // yamss namespace

#endif // YAMSS_STRUCTURE_HPP
