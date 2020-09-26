/*
 * Copyright 2017-present Yeolar
 */

/*
 * Modified from boost.
 */

//  (C) Copyright David Abrahams, Jeremy Siek, Daryle Walker 1999-2001.
//  (C) Copyright Daniel Frey 2002-2017.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/utility/operators.htm for documentation.

#pragma once

namespace crystal {

namespace operators_detail {

template <typename T> class empty_base {};

} // namespace operators_detail

//  Basic operator classes (contributed by Dave Abrahams) ------------------//

//  Note that friend functions defined in a class are implicitly inline.
//  See the C++ std, 11.4 [class.friend] paragraph 5

template <class T, class U, class B = operators_detail::empty_base<T> >
struct less_than_comparable2 : B
{
     friend bool operator<=(const T& x, const U& y) { return !static_cast<bool>(x > y); }
     friend bool operator>=(const T& x, const U& y) { return !static_cast<bool>(x < y); }
     friend bool operator>(const U& x, const T& y)  { return y < x; }
     friend bool operator<(const U& x, const T& y)  { return y > x; }
     friend bool operator<=(const U& x, const T& y) { return !static_cast<bool>(y < x); }
     friend bool operator>=(const U& x, const T& y) { return !static_cast<bool>(y > x); }
};

template <class T, class B = operators_detail::empty_base<T> >
struct less_than_comparable1 : B
{
     friend bool operator>(const T& x, const T& y)  { return y < x; }
     friend bool operator<=(const T& x, const T& y) { return !static_cast<bool>(y < x); }
     friend bool operator>=(const T& x, const T& y) { return !static_cast<bool>(x < y); }
};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct equality_comparable2 : B
{
     friend bool operator==(const U& y, const T& x) { return x == y; }
     friend bool operator!=(const U& y, const T& x) { return !static_cast<bool>(x == y); }
     friend bool operator!=(const T& y, const U& x) { return !static_cast<bool>(y == x); }
};

template <class T, class B = operators_detail::empty_base<T> >
struct equality_comparable1 : B
{
     friend bool operator!=(const T& x, const T& y) { return !static_cast<bool>(x == y); }
};

// A macro which produces "name_2left" from "name".
#define CRYSTAL_OPERATOR2_LEFT(name) name##2##_##left

//  NRVO-friendly implementation (contributed by Daniel Frey) ---------------//

#define CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( NAME, OP )                 \
template <class T, class U, class B = operators_detail::empty_base<T> > \
struct NAME##2 : B                                                      \
{                                                                       \
  friend T operator OP( const T& lhs, const U& rhs )                    \
    { T nrv( lhs ); nrv OP##= rhs; return nrv; }                        \
  friend T operator OP( const U& lhs, const T& rhs )                    \
    { T nrv( rhs ); nrv OP##= lhs; return nrv; }                        \
};                                                                      \
                                                                        \
template <class T, class B = operators_detail::empty_base<T> >          \
struct NAME##1 : B                                                      \
{                                                                       \
  friend T operator OP( const T& lhs, const T& rhs )                    \
    { T nrv( lhs ); nrv OP##= rhs; return nrv; }                        \
};

#define CRYSTAL_BINARY_OPERATOR_NON_COMMUTATIVE( NAME, OP )             \
template <class T, class U, class B = operators_detail::empty_base<T> > \
struct NAME##2 : B                                                      \
{                                                                       \
  friend T operator OP( const T& lhs, const U& rhs )                    \
    { T nrv( lhs ); nrv OP##= rhs; return nrv; }                        \
};                                                                      \
                                                                        \
template <class T, class U, class B = operators_detail::empty_base<T> > \
struct CRYSTAL_OPERATOR2_LEFT(NAME) : B                                 \
{                                                                       \
  friend T operator OP( const U& lhs, const T& rhs )                    \
    { T nrv( lhs ); nrv OP##= rhs; return nrv; }                        \
};                                                                      \
                                                                        \
template <class T, class B = operators_detail::empty_base<T> >          \
struct NAME##1 : B                                                      \
{                                                                       \
  friend T operator OP( const T& lhs, const T& rhs )                    \
    { T nrv( lhs ); nrv OP##= rhs; return nrv; }                        \
};

CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( multipliable, * )
CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( addable, + )
CRYSTAL_BINARY_OPERATOR_NON_COMMUTATIVE( subtractable, - )
CRYSTAL_BINARY_OPERATOR_NON_COMMUTATIVE( dividable, / )
CRYSTAL_BINARY_OPERATOR_NON_COMMUTATIVE( modable, % )
CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( xorable, ^ )
CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( andable, & )
CRYSTAL_BINARY_OPERATOR_COMMUTATIVE( orable, | )

#undef CRYSTAL_BINARY_OPERATOR_COMMUTATIVE
#undef CRYSTAL_BINARY_OPERATOR_NON_COMMUTATIVE
#undef CRYSTAL_OPERATOR2_LEFT

//  incrementable and decrementable contributed by Jeremy Siek

template <class T, class B = operators_detail::empty_base<T> >
struct incrementable : B
{
  friend T operator++(T& x, int)
  {
    incrementable_type nrv(x);
    ++x;
    return nrv;
  }
private: // The use of this typedef works around a Borland bug
  typedef T incrementable_type;
};

template <class T, class B = operators_detail::empty_base<T> >
struct decrementable : B
{
  friend T operator--(T& x, int)
  {
    decrementable_type nrv(x);
    --x;
    return nrv;
  }
private: // The use of this typedef works around a Borland bug
  typedef T decrementable_type;
};

//  Combined operator classes (contributed by Daryle Walker) ----------------//

template <class T, class U, class B = operators_detail::empty_base<T> >
struct totally_ordered2
    : less_than_comparable2<T, U
    , equality_comparable2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct totally_ordered1
    : less_than_comparable1<T
    , equality_comparable1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct additive2
    : addable2<T, U
    , subtractable2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct additive1
    : addable1<T
    , subtractable1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct multiplicative2
    : multipliable2<T, U
    , dividable2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct multiplicative1
    : multipliable1<T
    , dividable1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct integer_multiplicative2
    : multiplicative2<T, U
    , modable2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct integer_multiplicative1
    : multiplicative1<T
    , modable1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct arithmetic2
    : additive2<T, U
    , multiplicative2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct arithmetic1
    : additive1<T
    , multiplicative1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct integer_arithmetic2
    : additive2<T, U
    , integer_multiplicative2<T, U, B
      > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct integer_arithmetic1
    : additive1<T
    , integer_multiplicative1<T, B
      > > {};

template <class T, class U, class B = operators_detail::empty_base<T> >
struct bitwise2
    : xorable2<T, U
    , andable2<T, U
    , orable2<T, U, B
      > > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct bitwise1
    : xorable1<T
    , andable1<T
    , orable1<T, B
      > > > {};

template <class T, class B = operators_detail::empty_base<T> >
struct unit_steppable
    : incrementable<T
    , decrementable<T, B
      > > {};

//
// Here's where we put it all together, defining the xxxx forms of the templates.
// We also define specializations of is_chained_base<> for
// the xxxx, xxxx1, and xxxx2 templates.
//

namespace operators_detail {

// A type parameter is used instead of a plain bool because Borland's compiler
// didn't cope well with the more obvious non-type template parameter.
struct true_t {};
struct false_t {};

} // namespace operators_detail

// is_chained_base<> - a traits class used to distinguish whether an operator
// template argument is being used for base class chaining, or is specifying a
// 2nd argument type.

// Unspecialized version assumes that most types are not being used for base
// class chaining. We specialize for the operator templates defined in this
// library.
template<class T> struct is_chained_base {
  typedef operators_detail::false_t value;
};

// Provide a specialization of 'is_chained_base<>'
// for a 2-type-argument operator template.
# define CRYSTAL_OPERATOR_TEMPLATE2(template_name2)   \
  template<class T, class U, class B>                 \
  struct is_chained_base< template_name2<T, U, B> > { \
    typedef operators_detail::true_t value;           \
  };

// Provide a specialization of 'is_chained_base<>'
// for a 1-type-argument operator template.
# define CRYSTAL_OPERATOR_TEMPLATE1(template_name1)\
  template<class T, class B>                       \
  struct is_chained_base< template_name1<T, B> > { \
    typedef operators_detail::true_t value;        \
  };

// CRYSTAL_OPERATOR_TEMPLATE(template_name) defines template_name<> such that it
// can be used for specifying both 1-argument and 2-argument forms. Requires the
// existence of two previously defined class templates named '<template_name>1'
// and '<template_name>2' which must implement the corresponding 1- and 2-
// argument forms.
//
// The template type parameter O == is_chained_base<U>::value is used to
// distinguish whether the 2nd argument to <template_name> is being used for
// base class chaining from another crystal operator template or is describing a
// 2nd operand type. O == true_t only when U is actually an another operator
// template from the library. Partial specialization is used to select an
// implementation in terms of either '<template_name>1' or '<template_name>2'.
//

# define CRYSTAL_OPERATOR_TEMPLATE(template_name)                                     \
template <class T                                                                     \
         ,class U = T                                                                 \
         ,class B = operators_detail::empty_base<T>                                   \
         ,class O = typename is_chained_base<U>::value                                \
         >                                                                            \
struct template_name;                                                                 \
                                                                                      \
template<class T, class U, class B>                                                   \
struct template_name<T, U, B, operators_detail::false_t>                              \
  : template_name##2<T, U, B> {};                                                     \
                                                                                      \
template<class T, class U>                                                            \
struct template_name<T, U, operators_detail::empty_base<T>, operators_detail::true_t> \
  : template_name##1<T, U> {};                                                        \
                                                                                      \
template <class T, class B>                                                           \
struct template_name<T, T, B, operators_detail::false_t>                              \
  : template_name##1<T, B> {};                                                        \
                                                                                      \
template<class T, class U, class B, class O>                                          \
struct is_chained_base< template_name<T, U, B, O> > {                                 \
  typedef operators_detail::true_t value;                                             \
};                                                                                    \
                                                                                      \
CRYSTAL_OPERATOR_TEMPLATE2(template_name##2)                                          \
CRYSTAL_OPERATOR_TEMPLATE1(template_name##1)

CRYSTAL_OPERATOR_TEMPLATE(totally_ordered)
CRYSTAL_OPERATOR_TEMPLATE(integer_arithmetic)
CRYSTAL_OPERATOR_TEMPLATE(bitwise)
CRYSTAL_OPERATOR_TEMPLATE1(unit_steppable)

#undef CRYSTAL_OPERATOR_TEMPLATE
#undef CRYSTAL_OPERATOR_TEMPLATE2
#undef CRYSTAL_OPERATOR_TEMPLATE1

template <class T, class U>
struct operators2
    : totally_ordered2<T,U
    , integer_arithmetic2<T,U
    , bitwise2<T,U
      > > > {};

template <class T, class U = T>
struct operators : operators2<T, U> {};

template <class T> struct operators<T, T>
    : totally_ordered<T
    , integer_arithmetic<T
    , bitwise<T
    , unit_steppable<T
      > > > > {};

} // namespace crystal
