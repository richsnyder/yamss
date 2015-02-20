#ifndef YAMSS_MATRIX_CAST_HPP
#define YAMSS_MATRIX_CAST_HPP

#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <eigen3/Eigen/Dense>

namespace yamss {

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, 1>
vector_cast(const std::string& a_string)
{
  typedef T value_type;
  typedef size_t size_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> vector_type;
  typedef boost::char_separator<char> separator_type;
  typedef boost::tokenizer<separator_type> tokenizer_type;

  separator_type separator(" ;\t");
  tokenizer_type tokens(a_string, separator);
  tokenizer_type::const_iterator it;

  size_type num_rows = 0;
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    ++num_rows;
  }

  size_type row = 0;
  vector_type vector = vector_type::Zero(num_rows);
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    vector(row) = boost::lexical_cast<value_type>(*it);
    ++row;
  }

  return vector;
}

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
matrix_cast(const std::string& a_string)
{
  typedef T value_type;
  typedef size_t size_type;
  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrix_type;
  typedef boost::char_separator<char> separator_type;
  typedef boost::tokenizer<separator_type> tokenizer_type;

  separator_type separator(" \t", ";");
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

  size_type row = 0;
  size_type col = 0;
  matrix_type matrix = matrix_type::Zero(num_rows, num_cols);
  for (it = tokens.begin(); it != tokens.end(); ++it)
  {
    if (*it == ";")
    {
      row++;
      col = 0;
    }
    else
    {
      matrix(row, col) = boost::lexical_cast<value_type>(*it);
      ++col;
    }
  }

  return matrix;
}

} // yamss namespace

#endif // YAMSS_MATRIX_CAST_HPP
