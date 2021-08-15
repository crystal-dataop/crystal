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

/*
 * Modified from folly.
 */

#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace crystal {

/**
 * Small internal helper - returns the value just before an iterator.
 */
namespace detail {

/**
 * For random-access iterators, the value before is simply i[-1].
 */
template <class Iter>
typename std::enable_if<
    std::is_same<
        typename std::iterator_traits<Iter>::iterator_category,
        std::random_access_iterator_tag>::value,
    typename std::iterator_traits<Iter>::reference>::type
value_before(Iter i) {
  return i[-1];
}

/**
 * For all other iterators, we need to use the decrement operator.
 */
template <class Iter>
typename std::enable_if<
    !std::is_same<
        typename std::iterator_traits<Iter>::iterator_category,
        std::random_access_iterator_tag>::value,
    typename std::iterator_traits<Iter>::reference>::type
value_before(Iter i) {
  return *--i;
}

} // namespace detail

template <class Iter>
class Range {
 public:
  typedef std::size_t size_type;
  typedef Iter iterator;
  typedef Iter const_iterator;
  typedef typename std::remove_reference<
      typename std::iterator_traits<Iter>::reference>::type value_type;
  using difference_type = typename std::iterator_traits<Iter>::difference_type;
  typedef typename std::iterator_traits<Iter>::reference reference;

  static const size_type npos;

  // Works for all iterators
  constexpr Range() : b_(), e_() {}

  constexpr Range(const Range&) = default;
  constexpr Range(Range&&) = default;

 public:
  // Works for all iterators
  constexpr Range(Iter start, Iter end) : b_(start), e_(end) {}

  // Works only for random-access iterators
  constexpr Range(Iter start, size_t size) : b_(start), e_(start + size) {}

  Range(const Range& other, size_type first, size_type length = npos)
      : Range(other.subpiece(first, length)) {}

  template <
      class Container,
      class = typename std::enable_if<
          std::is_same<Iter, typename Container::const_pointer>::value>::type,
      class = decltype(
          Iter(std::declval<Container const&>().data()),
          Iter(
              std::declval<Container const&>().data() +
              std::declval<Container const&>().size()))>
  /* implicit */ constexpr Range(Container const& container)
      : b_(container.data()), e_(b_ + container.size()) {}

  template <
      class Container,
      class = typename std::enable_if<
          std::is_same<Iter, typename Container::const_pointer>::value>::type,
      class = decltype(
          Iter(std::declval<Container const&>().data()),
          Iter(
              std::declval<Container const&>().data() +
              std::declval<Container const&>().size()))>
  Range(Container const& container, typename Container::size_type startFrom) {
    auto const cdata = container.data();
    auto const csize = container.size();
    if (UNLIKELY(startFrom > csize)) {
      throw std::out_of_range("index out of range");
    }
    b_ = cdata + startFrom;
    e_ = cdata + csize;
  }

  template <
      class Container,
      class = typename std::enable_if<
          std::is_same<Iter, typename Container::const_pointer>::value>::type,
      class = decltype(
          Iter(std::declval<Container const&>().data()),
          Iter(
              std::declval<Container const&>().data() +
              std::declval<Container const&>().size()))>
  Range(
      Container const& container,
      typename Container::size_type startFrom,
      typename Container::size_type size) {
    auto const cdata = container.data();
    auto const csize = container.size();
    if (UNLIKELY(startFrom > csize)) {
      throw std::out_of_range("index out of range");
    }
    b_ = cdata + startFrom;
    if (csize - startFrom < size) {
      e_ = cdata + csize;
    } else {
      e_ = b_ + size;
    }
  }

  // Allow implicit conversion from Range<From> to Range<To> if From is
  // implicitly convertible to To.
  template <
      class OtherIter,
      typename std::enable_if<
          (!std::is_same<Iter, OtherIter>::value &&
           std::is_convertible<OtherIter, Iter>::value),
          int>::type = 0>
  constexpr /* implicit */ Range(const Range<OtherIter>& other)
      : b_(other.begin()), e_(other.end()) {}

  // Allow explicit conversion from Range<From> to Range<To> if From is
  // explicitly convertible to To.
  template <
      class OtherIter,
      typename std::enable_if<
          (!std::is_same<Iter, OtherIter>::value &&
           !std::is_convertible<OtherIter, Iter>::value &&
           std::is_constructible<Iter, const OtherIter&>::value),
          int>::type = 0>
  constexpr explicit Range(const Range<OtherIter>& other)
      : b_(other.begin()), e_(other.end()) {}

  /**
   * Allow explicit construction of Range() from a std::array of a
   * convertible type.
   *
   * For instance, this allows constructing StringPiece from a
   * std::array<char, N> or a std::array<const char, N>
   */
  template <
      class T,
      size_t N,
      typename = typename std::enable_if<
          std::is_convertible<const T*, Iter>::value>::type>
  constexpr explicit Range(const std::array<T, N>& array)
      : b_{array.empty() ? nullptr : &array.at(0)},
        e_{array.empty() ? nullptr : &array.at(0) + N} {}
  template <
      class T,
      size_t N,
      typename =
          typename std::enable_if<std::is_convertible<T*, Iter>::value>::type>
  constexpr explicit Range(std::array<T, N>& array)
      : b_{array.empty() ? nullptr : &array.at(0)},
        e_{array.empty() ? nullptr : &array.at(0) + N} {}

  Range& operator=(const Range& rhs) & = default;
  Range& operator=(Range&& rhs) & = default;

  void clear() {
    b_ = Iter();
    e_ = Iter();
  }

  void assign(Iter start, Iter end) {
    b_ = start;
    e_ = end;
  }

  void reset(Iter start, size_type size) {
    b_ = start;
    e_ = start + size;
  }

  constexpr size_type size() const {
    // It would be nice to assert(b_ <= e_) here.  This can be achieved even
    // in a C++11 compatible constexpr function:
    // http://ericniebler.com/2014/09/27/assert-and-constexpr-in-cxx11/
    // Unfortunately current gcc versions have a bug causing it to reject
    // this check in a constexpr function:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71448
    return size_type(e_ - b_);
  }
  constexpr size_type walk_size() const {
    return size_type(std::distance(b_, e_));
  }
  constexpr bool empty() const {
    return b_ == e_;
  }
  constexpr Iter data() const {
    return b_;
  }
  constexpr Iter start() const {
    return b_;
  }
  constexpr Iter begin() const {
    return b_;
  }
  constexpr Iter end() const {
    return e_;
  }
  constexpr Iter cbegin() const {
    return b_;
  }
  constexpr Iter cend() const {
    return e_;
  }
  value_type& front() {
    assert(b_ < e_);
    return *b_;
  }
  value_type& back() {
    assert(b_ < e_);
    return detail::value_before(e_);
  }
  const value_type& front() const {
    assert(b_ < e_);
    return *b_;
  }
  const value_type& back() const {
    assert(b_ < e_);
    return detail::value_before(e_);
  }

  /// explicit operator conversion to any compatible type
  ///
  /// A compatible type is one which is constructible with an iterator and a
  /// size (preferred), or a pair of iterators (fallback), passed by const-ref.
  ///
  /// Participates in overload resolution precisely when the target type is
  /// compatible. This allows std::is_constructible compile-time checks to work.
  template <
      typename Tgt,
      std::enable_if_t<
          std::is_constructible<Tgt, Iter const&, size_type>::value,
          int> = 0>
  constexpr explicit operator Tgt() const noexcept(
      std::is_nothrow_constructible<Tgt, Iter const&, size_type>::value) {
    return Tgt(b_, walk_size());
  }
  template <
      typename Tgt,
      std::enable_if_t<
          !std::is_constructible<Tgt, Iter const&, size_type>::value &&
              std::is_constructible<Tgt, Iter const&, Iter const&>::value,
          int> = 0>
  constexpr explicit operator Tgt() const noexcept(
      std::is_nothrow_constructible<Tgt, Iter const&, Iter const&>::value) {
    return Tgt(b_, e_);
  }

  /// explicit non-operator conversion to any compatible type
  ///
  /// A compatible type is one which is constructible with an iterator and a
  /// size (preferred), or a pair of iterators (fallback), passed by const-ref.
  ///
  /// Participates in overload resolution precisely when the target type is
  /// compatible. This allows is_invocable compile-time checks to work.
  ///
  /// Provided in addition to the explicit operator conversion to permit passing
  /// additional arguments to the target type constructor. A canonical example
  /// of an additional argument might be an allocator, where the target type is
  /// some specialization of std::vector or std::basic_string in a context which
  /// requires a non-default-constructed allocator.
  template <typename Tgt, typename... Args>
  constexpr std::enable_if_t<
      std::is_constructible<Tgt, Iter const&, size_type>::value,
      Tgt>
  to(Args&&... args) const noexcept(
      std::is_nothrow_constructible<Tgt, Iter const&, size_type, Args&&...>::
          value) {
    return Tgt(b_, walk_size(), static_cast<Args&&>(args)...);
  }
  template <typename Tgt, typename... Args>
  constexpr std::enable_if_t<
      !std::is_constructible<Tgt, Iter const&, size_type>::value &&
          std::is_constructible<Tgt, Iter const&, Iter const&>::value,
      Tgt>
  to(Args&&... args) const noexcept(
      std::is_nothrow_constructible<Tgt, Iter const&, Iter const&, Args&&...>::
          value) {
    return Tgt(b_, e_, static_cast<Args&&>(args)...);
  }

  Range<Iter> castToConst() const {
    return Range<Iter>(*this);
  }

  value_type& operator[](size_t i) {
    assert(i < size());
    return b_[i];
  }

  const value_type& operator[](size_t i) const {
    assert(i < size());
    return b_[i];
  }

  value_type& at(size_t i) {
    if (i >= size()) {
      throw std::out_of_range("index out of range");
    }
    return b_[i];
  }

  const value_type& at(size_t i) const {
    if (i >= size()) {
      throw std::out_of_range("index out of range");
    }
    return b_[i];
  }

  void advance(size_type n) {
    if (UNLIKELY(n > size())) {
      throw std::out_of_range("index out of range");
    }
    b_ += n;
  }

  void subtract(size_type n) {
    if (UNLIKELY(n > size())) {
      throw std::out_of_range("index out of range");
    }
    e_ -= n;
  }

  Range subpiece(size_type first, size_type length = npos) const {
    if (UNLIKELY(first > size())) {
      throw std::out_of_range("index out of range");
    }

    return Range(b_ + first, std::min(length, size() - first));
  }

  // unchecked versions
  void uncheckedAdvance(size_type n) {
    assert(n <= size());
    b_ += n;
  }

  void uncheckedSubtract(size_type n) {
    assert(n <= size());
    e_ -= n;
  }

  Range uncheckedSubpiece(size_type first, size_type length = npos) const {
    assert(first <= size());
    return Range(b_ + first, std::min(length, size() - first));
  }

  void pop_front() {
    assert(b_ < e_);
    ++b_;
  }

  void pop_back() {
    assert(b_ < e_);
    --e_;
  }

  void swap(Range& rhs) {
    std::swap(b_, rhs.b_);
    std::swap(e_, rhs.e_);
  }

  /**
   * Does this Range start with another range?
   */
  bool startsWith(const Range<Iter>& other) const {
    return size() >= other.size() &&
        castToConst().subpiece(0, other.size()) == other;
  }
  bool startsWith(value_type c) const {
    return !empty() && front() == c;
  }

  template <class Comp>
  bool startsWith(const Range<Iter>& other, Comp&& eq) const {
    if (size() < other.size()) {
      return false;
    }
    auto const trunc = subpiece(0, other.size());
    return std::equal(
        trunc.begin(), trunc.end(), other.begin(), std::forward<Comp>(eq));
  }

  /**
   * Does this Range end with another range?
   */
  bool endsWith(const Range<Iter>& other) const {
    return size() >= other.size() &&
        castToConst().subpiece(size() - other.size()) == other;
  }
  bool endsWith(value_type c) const {
    return !empty() && back() == c;
  }

  template <class Comp>
  bool endsWith(const Range<Iter>& other, Comp&& eq) const {
    if (size() < other.size()) {
      return false;
    }
    auto const trunc = subpiece(size() - other.size());
    return std::equal(
        trunc.begin(), trunc.end(), other.begin(), std::forward<Comp>(eq));
  }

  template <class Comp>
  bool equals(const Range<Iter>& other, Comp&& eq) const {
    return size() == other.size() &&
        std::equal(begin(), end(), other.begin(), std::forward<Comp>(eq));
  }

  /**
   * Remove the given prefix and return true if the range starts with the given
   * prefix; return false otherwise.
   */
  bool removePrefix(const Range<Iter>& prefix) {
    return startsWith(prefix) && (b_ += prefix.size(), true);
  }
  bool removePrefix(value_type prefix) {
    return startsWith(prefix) && (++b_, true);
  }

  /**
   * Remove the given suffix and return true if the range ends with the given
   * suffix; return false otherwise.
   */
  bool removeSuffix(const Range<Iter>& suffix) {
    return endsWith(suffix) && (e_ -= suffix.size(), true);
  }
  bool removeSuffix(value_type suffix) {
    return endsWith(suffix) && (--e_, true);
  }

 private:
  Iter b_, e_;
};

template <class Iter>
const typename Range<Iter>::size_type Range<Iter>::npos = -1;

template <class Iter>
void swap(Range<Iter>& lhs, Range<Iter>& rhs) {
  lhs.swap(rhs);
}

/**
 * Create a range from two iterators, with type deduction.
 */
template <class Iter>
constexpr Range<Iter> range(Iter first, Iter last) {
  return Range<Iter>(first, last);
}

/*
 * Creates a range to reference the contents of a contiguous-storage container.
 */
// Use pointers for types with '.data()' member
template <class Collection>
constexpr auto range(Collection& v) -> Range<decltype(v.data())> {
  return Range<decltype(v.data())>(v.data(), v.data() + v.size());
}
template <class Collection>
constexpr auto range(Collection const& v) -> Range<decltype(v.data())> {
  return Range<decltype(v.data())>(v.data(), v.data() + v.size());
}
template <class Collection>
constexpr auto crange(Collection const& v) -> Range<decltype(v.data())> {
  return Range<decltype(v.data())>(v.data(), v.data() + v.size());
}

template <class T, size_t n>
constexpr Range<T*> range(T (&array)[n]) {
  return Range<T*>(array, array + n);
}
template <class T, size_t n>
constexpr Range<T const*> range(T const (&array)[n]) {
  return Range<T const*>(array, array + n);
}
template <class T, size_t n>
constexpr Range<T const*> crange(T const (&array)[n]) {
  return Range<T const*>(array, array + n);
}

template <class T, size_t n>
constexpr Range<T*> range(std::array<T, n>& array) {
  return Range<T*>{array};
}
template <class T, size_t n>
constexpr Range<T const*> range(std::array<T, n> const& array) {
  return Range<T const*>{array};
}
template <class T, size_t n>
constexpr Range<T const*> crange(std::array<T, n> const& array) {
  return Range<T const*>{array};
}

} // namespace crystal
