#pragma once

#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include <variant>

namespace tf {

//-----------------------------------------------------------------------------
// Traits
//-----------------------------------------------------------------------------

// Macro to check whether a class has a member function
#define define_has_member(member_name)                                     \
template <typename T>                                                      \
class has_member_##member_name                                             \
{                                                                          \
  typedef char yes_type;                                                   \
  typedef long no_type;                                                    \
  template <typename U> static yes_type test(decltype(&U::member_name));   \
  template <typename U> static no_type  test(...);                         \
  public:                                                                  \
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);  \
}

#define has_member(class_, member_name)  has_member_##member_name<class_>::value

//-----------------------------------------------------------------------------
// Move-On-Copy
//-----------------------------------------------------------------------------

// Struct: MoC
template <typename T>
struct MoC {

  MoC(T&& rhs) : object(std::move(rhs)) {}
  MoC(const MoC& other) : object(std::move(other.object)) {}

  T& get() { return object; }
  
  mutable T object; 
};

template <typename T>
auto make_moc(T&& m) {
  return MoC<T>(std::forward<T>(m));
}

// ----------------------------------------------------------------------------
// Function Traits
// reference: https://github.com/ros2/rclcpp
// ----------------------------------------------------------------------------

template<typename T>
struct tuple_tail;

template<typename Head, typename ... Tail>
struct tuple_tail<std::tuple<Head, Tail ...>> {
  using type = std::tuple<Tail ...>;
};

// std::function
template<typename F>
struct function_traits
{
  using arguments = typename tuple_tail<
    typename function_traits<decltype(&F::operator())>::argument_tuple_type
  >::type;

  static constexpr size_t arity = std::tuple_size<arguments>::value;

  template <size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = std::tuple_element_t<N, arguments>;
  };
  
  template <size_t N>
  using argument_t = typename argument<N>::type;

  using return_type = typename function_traits<decltype(&F::operator())>::return_type;
};

// Free functions
template<typename R, typename... Args>
struct function_traits<R(Args...)> {

  using return_type = R;
  using argument_tuple_type = std::tuple<Args...>;
 
  static constexpr size_t arity = sizeof...(Args);
 
  template <size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = std::tuple_element_t<N, std::tuple<Args...>>;
  };

  template <size_t N>
  using argument_t = typename argument<N>::type;
};

// function pointer
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {
};

// function reference
template<typename R, typename... Args>
struct function_traits<R(&)(Args...)> : function_traits<R(Args...)> {
};

// immutable lambda
template<typename C, typename R, typename ... Args>
struct function_traits<R(C::*)(Args ...) const>
  : function_traits<R(C &, Args ...)>
{};

// mutable lambda
template<typename C, typename R, typename ... Args>
struct function_traits<R(C::*)(Args ...)>
  : function_traits<R(C &, Args ...)>
{};

// decay to the raw type
template <typename F>
struct function_traits<F&> : function_traits<F> {};

template <typename F>
struct function_traits<F&&> : function_traits<F> {};


// ----------------------------------------------------------------------------
// std::variant
// ----------------------------------------------------------------------------
template <typename T, typename>
struct get_index;

template <size_t I, typename... Ts> 
struct get_index_impl {};

template <size_t I, typename T, typename... Ts> 
struct get_index_impl<I, T, T, Ts...> : std::integral_constant<size_t, I>{};

template <size_t I, typename T, typename U, typename... Ts> 
struct get_index_impl<I, T, U, Ts...> : get_index_impl<I+1, T, Ts...>{};

template <typename T, typename... Ts> 
struct get_index<T, std::variant<Ts...>> : get_index_impl<0, T, Ts...>{};

template <typename T, typename... Ts>
constexpr auto get_index_v = get_index<T, Ts...>::value;

// ----------------------------------------------------------------------------
// bit_cast
//-----------------------------------------------------------------------------
template <class To, class From>
typename std::enable_if<
  (sizeof(To) == sizeof(From)) &&
  std::is_trivially_copyable<From>::value &&
  std::is_trivial<To>::value,
  // this implementation requires that To is trivially default constructible
  To
>::type
// constexpr support needs compiler magic
bit_cast(const From &src) noexcept {
  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}

}  // end of namespace tf. ---------------------------------------------------



