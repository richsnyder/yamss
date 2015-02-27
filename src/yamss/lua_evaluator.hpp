#ifndef YAMSS_LUA_EVALUATOR_HPP
#define YAMSS_LUA_EVALUATOR_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <armadillo>
#include <boost/format.hpp>
#include "yamss/evaluator.hpp"

extern "C" {
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
}

namespace yamss {

template <typename T = double>
class lua_evaluator : public evaluator<T>
{
public:
  typedef T value_type;
  typedef node<T> node_type;
  typedef arma::Col<T> vector_type;

  lua_evaluator()
    : m_state(0)
    , m_references(6, LUA_NOREF)
    , m_expression_formatter("return %1%")
  {
    // empty
  }

  lua_evaluator(const boost::property_tree::ptree& a_tree)
    : m_state(0)
    , m_references(6, LUA_NOREF)
    , m_expression_formatter("return %1%")
  {
    boost::optional<std::string> eq;
    if ((eq = a_tree.get_optional<std::string>("equations.x")))
    {
      set_expression(0, *eq);
    }
    if ((eq = a_tree.get_optional<std::string>("equations.y")))
    {
      set_expression(1, *eq);
    }
    if ((eq = a_tree.get_optional<std::string>("equations.z")))
    {
      set_expression(2, *eq);
    }
    if ((eq = a_tree.get_optional<std::string>("equations.p")))
    {
      set_expression(3, *eq);
    }
    if ((eq = a_tree.get_optional<std::string>("equations.q")))
    {
      set_expression(4, *eq);
    }
    if ((eq = a_tree.get_optional<std::string>("equations.r")))
    {
      set_expression(5, *eq);
    }
  }

  virtual
  ~lua_evaluator()
  {
    close();
  }

  void
  clear_expression(int a_dof)
  {
    luaL_unref(m_state, LUA_REGISTRYINDEX, m_references[a_dof]);
    m_references[a_dof] = LUA_NOREF;
  }

  void
  set_expression(int a_dof, const std::string& a_expression)
  {
    bootstrap();
    clear_expression(a_dof);
    std::string expression = boost::str(m_expression_formatter % a_expression);
    int code = luaL_loadstring(m_state, expression.c_str());
    if (code != LUA_OK)
    {
      close();
      boost::format fmt("Could not save the expression [%1%]");
      throw std::runtime_error(boost::str(fmt % a_expression));
    }
    m_references[a_dof] = luaL_ref(m_state, LUA_REGISTRYINDEX);
  }

  virtual
  vector_type
  operator()(const value_type& a_time, const node_type& a_node)
  {
    bootstrap();
    set_globals(a_time, a_node);
    vector_type vec(6);
    vec.zeros();
    for (size_t n = 0; n < m_references.size(); ++n)
    {
      if (m_references[n] != LUA_NOREF)
      {
        lua_rawgeti(m_state, LUA_REGISTRYINDEX, m_references[n]);
        int code = lua_pcall(m_state, 0, 1, 0);
        if (code != LUA_OK)
        {
          close();
          throw std::runtime_error("Could not evaluate an expression");
        }
        vec(n) = lua_tonumber(m_state, -1);
        lua_pop(m_state, 1);
      }
    }
    return vec;
  }
protected:
  void
  open()
  {
    m_state = luaL_newstate();
    if (!m_state)
    {
      throw std::runtime_error("Could not initialize the Lua evaluator");
    }
    luaL_openlibs(m_state);
  }

  void
  close()
  {
    if (m_state)
    {
      lua_close(m_state);
      m_state = 0;
    }
  }

  void
  bootstrap()
  {
    if (!m_state)
    {
      open();
    }
  }

  void
  set_globals(const value_type& a_time, const node_type& a_node)
  {
    lua_pushnumber(m_state, std::real(a_time));
    lua_setglobal(m_state, "t");
    lua_pushnumber(m_state, std::real(a_node.get_position(0)));
    lua_setglobal(m_state, "x");
    lua_pushnumber(m_state, std::real(a_node.get_position(1)));
    lua_setglobal(m_state, "y");
    lua_pushnumber(m_state, std::real(a_node.get_position(2)));
    lua_setglobal(m_state, "z");
    lua_pushnumber(m_state, std::real(a_node.get_position(3)));
    lua_setglobal(m_state, "p");
    lua_pushnumber(m_state, std::real(a_node.get_position(4)));
    lua_setglobal(m_state, "q");
    lua_pushnumber(m_state, std::real(a_node.get_position(5)));
    lua_setglobal(m_state, "r");
  }
private:
  typedef std::vector<int> references_type;

  lua_State* m_state;
  references_type m_references;
  boost::format m_expression_formatter;
}; // lua_evaluator<T> class

} // yamss namespace

#endif // YAMSS_LUA_EVALUATOR_HPP
