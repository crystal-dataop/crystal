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

#include <limits>
#include <type_traits>

#define CRYSTAL_CREATE_HAS_MEMBER_TYPE_TRAITS(classname, type_name)            \
  template <typename TTheClass_>                                               \
  struct classname##__crystal_traits_impl__ {                                  \
    template <typename UTheClass_>                                             \
    static constexpr bool test(typename UTheClass_::type_name*) {              \
      return true;                                                             \
    }                                                                          \
    template <typename>                                                        \
    static constexpr bool test(...) {                                          \
      return false;                                                            \
    }                                                                          \
  };                                                                           \
  template <typename TTheClass_>                                               \
  using classname = typename std::conditional<                                 \
      classname##__crystal_traits_impl__<TTheClass_>::                         \
          template test<TTheClass_>(nullptr),                                  \
      std::true_type,                                                          \
      std::false_type>::type

#define CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, cv_qual)\
  template <typename TTheClass_, typename RTheReturn_, typename... TTheArgs_>  \
  struct classname##__crystal_traits_impl__<                                   \
      TTheClass_,                                                              \
      RTheReturn_(TTheArgs_...) cv_qual> {                                     \
    template <                                                                 \
        typename UTheClass_,                                                   \
        RTheReturn_ (UTheClass_::*)(TTheArgs_...) cv_qual>                     \
    struct sfinae {};                                                          \
    template <typename UTheClass_>                                             \
    static std::true_type test(sfinae<UTheClass_, &UTheClass_::func_name>*);   \
    template <typename>                                                        \
    static std::false_type test(...);                                          \
  }

/*
 * The CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS is used to create traits
 * classes that check for the existence of a member function with
 * a given name and signature. It currently does not support
 * checking for inherited members.
 *
 * Such classes receive two template parameters: the class to be checked
 * and the signature of the member function. A static boolean field
 * named `value` (which is also constexpr) tells whether such member
 * function exists.
 *
 * Each traits class created is bound only to the member name, not to
 * its signature nor to the type of the class containing it.
 *
 * Say you need to know if a given class has a member function named
 * `test` with the following signature:
 *
 *    int test() const;
 *
 * You'd need this macro to create a traits class to check for a member
 * named `test`, and then use this traits class to check for the signature:
 *
 * namespace {
 *
 * CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS(has_test_traits, test);
 *
 * } // unnamed-namespace
 *
 * void some_func() {
 *   cout << "Does class Foo have a member int test() const? "
 *     << boolalpha << has_test_traits<Foo, int() const>::value;
 * }
 *
 * You can use the same traits class to test for a completely different
 * signature, on a completely different class, as long as the member name
 * is the same:
 *
 * void some_func() {
 *   cout << "Does class Foo have a member int test()? "
 *     << boolalpha << has_test_traits<Foo, int()>::value;
 *   cout << "Does class Foo have a member int test() const? "
 *     << boolalpha << has_test_traits<Foo, int() const>::value;
 *   cout << "Does class Bar have a member double test(const string&, long)? "
 *     << boolalpha << has_test_traits<Bar, double(const string&, long)>::value;
 * }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS(classname, func_name)              \
  template <typename, typename>                                                \
  struct classname##__crystal_traits_impl__;                                   \
  CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, );            \
  CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, const);       \
  CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(                                    \
      classname, func_name, /* nolint */ volatile);                            \
  CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(                                    \
      classname, func_name, /* nolint */ volatile const);                      \
  template <typename TTheClass_, typename TTheSignature_>                      \
  using classname =                                                            \
      decltype(classname##__crystal_traits_impl__<TTheClass_, TTheSignature_>::\
                   template test<TTheClass_>(nullptr))

namespace crystal {

template <typename T>
using _t = typename T::type;

namespace detail {

template <typename T, bool>
struct is_negative_impl {
  constexpr static bool check(T x) { return x < 0; }
};

template <typename T>
struct is_negative_impl<T, false> {
  constexpr static bool check(T) { return false; }
};

template <typename RHS, RHS rhs, typename LHS>
bool less_than_impl(LHS const lhs) {
  return
    rhs > std::numeric_limits<LHS>::max() ? true :
    rhs <= std::numeric_limits<LHS>::min() ? false :
    lhs < rhs;
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than_impl(LHS const lhs) {
  return
    rhs > std::numeric_limits<LHS>::max() ? false :
    rhs < std::numeric_limits<LHS>::min() ? true :
    lhs > rhs;
}

}  // namespace detail

template <typename T>
constexpr bool is_negative(T x) {
  return detail::is_negative_impl<T, std::is_signed<T>::value>::check(x);
}

template <typename T>
constexpr bool is_non_positive(T x) { return !x || is_negative(x); }

template <typename T>
constexpr bool is_positive(T x) { return !is_non_positive(x); }

template <typename T>
constexpr bool is_non_negative(T x) {
  return !x || is_positive(x);
}

template <typename RHS, RHS rhs, typename LHS>
bool less_than(LHS const lhs) {
  return detail::less_than_impl<
    RHS, rhs, typename std::remove_reference<LHS>::type
  >(lhs);
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than(LHS const lhs) {
  return detail::greater_than_impl<
    RHS, rhs, typename std::remove_reference<LHS>::type
  >(lhs);
}

} // namespace crystal
