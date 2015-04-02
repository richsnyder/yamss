#include "yamss/element.hpp"

namespace yamss {

element::element()
{
  // empty
}

element::element(key_type a_key, shape_type a_shape)
  : m_key(a_key)
  , m_vertices()
{
  set_shape(a_shape);
}

element::element(const element& a_other)
  : m_key(a_other.m_key)
  , m_shape(a_other.m_shape)
  , m_vertices(a_other.m_vertices)
{
  // empty
}

element::~element()
{
  // empty
}

element&
element::operator=(const element& a_other)
{
  m_key = a_other.m_key;
  m_shape = a_other.m_shape;
  m_vertices = a_other.m_vertices;
  return *this;
}

element::key_type
element::get_key() const
{
  return m_key;
}

element::shape_type
element::get_shape() const
{
  return m_shape;
}

element::size_type
element::get_size() const
{
  return m_vertices.size();
}

element::key_type
element::get_vertex(size_type a_pos) const
{
  return m_vertices[a_pos];
}

const element::vector_type&
element::get_vertices() const
{
  return m_vertices;
}

void
element::set_vertex(size_type a_pos, key_type a_vertex)
{
  m_vertices[a_pos] = a_vertex;
}

void
element::set_vertices(const vector_type& a_vertices)
{
  m_vertices = a_vertices;
}

void
element::set_shape(shape_type a_shape)
{
  m_shape = a_shape;
  switch (m_shape)
  {
    case POINT:
      m_vertices.resize(1);
      break;
    case LINE:
      m_vertices.resize(2);
      break;
    case TRIANGLE:
      m_vertices.resize(3);
      break;
    case QUADRILATERAL:
      m_vertices.resize(4);
      break;
  }
}

} // yamss namespace
