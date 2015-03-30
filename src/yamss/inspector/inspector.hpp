#ifndef YAMSS_INSPECTOR_HPP
#define YAMSS_INSPECTOR_HPP

#include <set>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include "yamss/eom.hpp"
#include "yamss/structure.hpp"

namespace yamss {
namespace inspector {

template <typename T>
class inspector
{
public:
  typedef T value_type;
  typedef eom<T> eom_type;
  typedef structure<T> structure_type;
  typedef boost::filesystem::path path_type;

  virtual
  void
  initialize(const eom_type& a_eom,
             const structure_type& a_structure,
             const path_type& a_directory) = 0;

  virtual
  void
  update(const eom_type& a_eom, const structure_type& a_structure) = 0;

  virtual
  void
  finalize(const eom_type& a_eom, const structure_type& a_structure) = 0;

  virtual
  void
  get_files(std::set<path_type>& a_set) const
  {
    // empty
  }
}; // inspector<T> class

} // inspector namespace
} // yamss namespace

#endif // YAMSS_INSPECTOR_HPP
