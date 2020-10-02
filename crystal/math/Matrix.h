/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <ostream>

#include "crystal/math/Span.h"

namespace crystal {

template <class T>
class Matrix {
 public:
  typedef T element_type;
  typedef std::remove_cv_t<T> value_type;
  typedef size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  Matrix(const_pointer data, size_type x, size_type y)
      : data_(data), x_(x), y_(y) {}
  Matrix(const Span<T> data, size_type dimension)
      : data_(data.data()), x_(data.size() / dimension), y_(dimension) {}

  size_type x() const { return x_; }
  size_type y() const { return y_; }

  Span<T> operator[](size_type i) const {
    return Span<T>(data_ + y_ * i, y_);
  }
  const_reference operator()(size_type i, size_type j) const {
    return data_[y_ * i + j];
  }

  const_pointer data() const { return data_; }

  size_type size() const { return x_ * y_; }
  size_type size_bytes() const { return x_ * y_ * sizeof(element_type); }

  bool empty() const { return x_ * y_ == 0; }

  Span<T> toSpan() const;
  std::string toString() const;

 private:
  const_pointer data_;
  size_type x_, y_;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix);

//////////////////////////////////////////////////////////////////////

template <class T>
Span<T> Matrix<T>::toSpan() const {
  return Span<T>(data_, x_ * y_);
}

template <class T>
std::string Matrix<T>::toString() const {
  return toSpan().toString();
}

template <class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
  os << "[" << std::endl;
  for (size_t i = 0; i < matrix.x(); ++i) {
    for (size_t j = 0; j < matrix.y(); ++j) {
      os << matrix(i, j) << ",";
    }
    os << std::endl;
  }
  os << "]";
  return os;
}

} // namespace crystal
