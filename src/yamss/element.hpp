#ifndef YAMSS_ELEMENT_HPP
#define YAMSS_ELEMENT_HPP

#include <armadillo>

namespace yamss {

class element
{
public:
  typedef size_t key_type;
  typedef size_t size_type;
  typedef arma::Col<key_type> vector_type;

  enum shape_type
  {
    LINE,
    TRIANGLE,
    QUADRILATERAL
  };

  element(key_type a_key, shape_type a_shape);

  element(const element& a_other);

  ~element();

  element&
  operator=(const element& a_other);

  key_type
  get_key() const;

  shape_type
  get_shape() const;

  size_type
  get_size() const;

  key_type
  get_vertex(size_type a_pos) const;

  const vector_type&
  get_vertices() const;

  void
  set_vertex(size_type a_pos, key_type a_vertex);

  void
  set_vertices(const vector_type& a_vertices);
protected:
  void
  set_shape(shape_type a_shape);
private:
  element();

  key_type m_key;
  shape_type m_shape;
  vector_type m_vertices;
}; // element class

} // yamss namespace

#endif // YAMSS_ELEMENT_HPP
