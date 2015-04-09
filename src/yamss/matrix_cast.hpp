#ifndef YAMSS_MATRIX_CAST_HPP
#define YAMSS_MATRIX_CAST_HPP

#include <iostream>
#include <string>
#include <armadillo>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

namespace yamss {

template <typename T>
arma::Col<T>
vector_cast(const std::string& a_string)
{
  typedef T value_type;
  typedef size_t size_type;
  typedef arma::Col<T> vector_type;
  typedef boost::char_separator<char> separator_type;
  typedef boost::tokenizer<separator_type> tokenizer_type;

  separator_type separator(" ;\t\r\n");
  tokenizer_type tokens(a_string, separator);
  tokenizer_type::const_iterator it;

  size_type num_rows = 0;
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    ++num_rows;
  }

  vector_type vec(num_rows);
  vec.zeros();

  size_type row = 0;
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    vec(row) = boost::lexical_cast<value_type>(*it);
    ++row;
  }

  return vec;
}

template <typename T>
arma::Mat<T>
matrix_cast(const std::string& a_string)
{
  typedef T value_type;
  typedef size_t size_type;
  typedef arma::Mat<T> matrix_type;
  typedef boost::char_separator<char> separator_type;
  typedef boost::tokenizer<separator_type> tokenizer_type;

  boost::smatch match;
  boost::regex re("^\\s*diag\\(\\s*(?<vector>.*)\\s*\\)\\s*$");
  if (boost::regex_match(a_string, match, re))
  {
    return arma::diagmat(vector_cast<T>(match["vector"]));
  }

  separator_type separator(" \t\r\n", ";");
  tokenizer_type tokens(a_string, separator);
  tokenizer_type::const_iterator it;

  size_type num_rows = 1;
  size_type num_cols = 0;
  size_type cells_in_row = 0;
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    if (*it == ";")
    {
      ++num_rows;
      num_cols = std::max(num_cols, cells_in_row);
      cells_in_row = 0;
    }
    else
    {
      ++cells_in_row;
    }
  }
  num_cols = std::max(num_cols, cells_in_row);

  matrix_type mat(num_rows, num_cols);
  mat.zeros();

  size_type row = 0;
  size_type col = 0;
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    if (*it == ";")
    {
      row++;
      col = 0;
    }
    else
    {
      mat(row, col) = boost::lexical_cast<value_type>(*it);
      ++col;
    }
  }

  return mat;
}

} // yamss namespace

#endif // YAMSS_MATRIX_CAST_HPP
