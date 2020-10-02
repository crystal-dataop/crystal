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

#include <type_traits>
#include <utility>

#include "crystal/dataframe/DataView.h"
#include "crystal/foundation/json.h"

namespace crystal {
namespace op {

template <class Candidate, class Expected>
class IsCompatibleSignature {
  static constexpr bool value = false;
};

template <class Candidate, class ExpectedReturn, class... ArgTypes>
class IsCompatibleSignature<Candidate, ExpectedReturn(ArgTypes...)> {
  template <
      class F,
      class ActualReturn =
          decltype(std::declval<F>()(std::declval<ArgTypes>()...)),
      bool good = std::is_same<ExpectedReturn, ActualReturn>::value>
  static constexpr bool testArgs(int*) {
    return good;
  }

  template <class F>
  static constexpr bool testArgs(...) {
    return false;
  }

 public:
  static constexpr bool value = testArgs<Candidate>(nullptr);
};

template <class Self>
struct Bounded {
  const Self& self() const {
    return *static_cast<const Self*>(this);
  }

  Self& self() {
    return *static_cast<Self*>(this);
  }
};

template <class Self>
class Operator : public Bounded<Self> {
 public:
  template <class Result = void>
  Result apply(DataView& view, uint32_t i) const;

  template <class Result = void>
  Result compose(DataView& view) const;

  dynamic toDynamic() const;

  Operator() = default;
  Operator(Operator&&) noexcept = default;
  Operator(const Operator&) = default;
  Operator& operator=(Operator&&) noexcept = default;
  Operator& operator=(const Operator&) = default;
};

template <class Op>
std::string toJson(const Operator<Op>& op, bool pretty = false) {
  return pretty ? toPrettyJson(op.self().toDynamic())
                : toJson(op.self().toDynamic());
}

template <class Op>
std::string toCson(const Operator<Op>& op, bool pretty = false) {
  return pretty ? toPrettyCson(op.self().toDynamic())
                : toCson(op.self().toDynamic());
}

template <class Op>
auto operator|(DataView& view, const Operator<Op>& op) ->
decltype(op.self().compose(view)) {
  return op.self().compose(view);
}

template <class Udf>
typename std::enable_if<
  IsCompatibleSignature<Udf, void(DataView&)>::value, DataView&>::type
operator|(DataView& view, Udf&& udf) {
  udf(view);
  return view;
}

template <class Udf>
typename std::enable_if<
  IsCompatibleSignature<Udf, void(DataView&, uint32_t)>::value, DataView&>::type
operator|(DataView& view, Udf&& udf) {
  for (auto i : view.docIndex()) {
    udf(view, i);
  }
  return view;
}

namespace detail {

template <class First, class Second>
class Chain : public Operator<Chain<First, Second>> {
  First first_;
  Second second_;

 public:
  Chain(First first, Second second)
      : first_(std::move(first)),
        second_(std::move(second)) {}

  bool apply(DataView& view, uint32_t i) const {
    return first_.self().apply(view, i) &&
          second_.self().apply(view, i);
  }

  DataView& compose(DataView& view) const {
    U32IndexArray index;
    for (auto i : view.docIndex()) {
      if (apply(view, i)) {
        index.push_back(i);
      }
    }
    view.docIndex().swap(index);
    return view;
  }

  dynamic toDynamic() const {
    dynamic out = dynamic::array;
    dynamic a = first_.self().toDynamic();
    dynamic b = second_.self().toDynamic();
    if (a.isArray()) {
      out = a;
    } else {
      out.push_back(a);
    }
    if (b.isArray()) {
      for (auto& i : b) out.push_back(i);
    } else {
      out.push_back(b);
    }
    return out;
  }
};

} // namespace detail

template <class Op1, class Op2, class Chain = detail::Chain<Op1, Op2>>
Chain operator+(const Operator<Op1>& op1, const Operator<Op2>& op2) {
  return Chain(op1.self(), op2.self());
}

template <class Op1, class Op2, class Chain = detail::Chain<Op1, Op2>>
Chain operator+(const Operator<Op1>& op1, Operator<Op2>&& op2) {
  return Chain(op1.self(), std::move(op2.self()));
}

template <class Op1, class Op2, class Chain = detail::Chain<Op1, Op2>>
Chain operator+(Operator<Op1>&& op1, const Operator<Op2>& op2) {
  return Chain(std::move(op1.self()), op2.self());
}

template <class Op1, class Op2, class Chain = detail::Chain<Op1, Op2>>
Chain operator+(Operator<Op1>&& op1, Operator<Op2>&& op2) {
  return Chain(std::move(op1.self()), std::move(op2.self()));
}

} // namespace op
} // namespace crystal
