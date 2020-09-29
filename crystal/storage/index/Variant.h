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

#include <variant>

namespace crystal {

struct IsBlank {
  bool operator()(std::monostate&) const {
    return true;
  }
  template <class T>
  bool operator()(T&) const {
    return false;
  }
};

template <class Variant>
inline bool isBlank(Variant& v) {
   return std::visit(IsBlank(), v);
}

template <class Base>
struct BaseGetter {
  Base* operator()(std::monostate&) const {
    return nullptr;
  }
  template <class T>
  Base* operator()(T& i) const {
    return &i;
  }
};

template <class Base, class Variant>
inline Base* getBase(Variant& v) {
  return std::visit(BaseGetter<Base>(), v);
}

} // namespace crystal
