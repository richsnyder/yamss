#ifndef YAMSS_COMPLEX_HPP
#define YAMSS_COMPLEX_HPP

#include <complex>

namespace yamss {

template <typename T>
T
real(const T& a_x)
{
  return a_x;
}

template <typename T>
T
real(const std::complex<T>& a_x)
{
  return a_x.real();
}

} // yamss namespace

#endif // YAMSS_COMPLEX_HPP
