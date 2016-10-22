// Copyright (c) 2016 Christoph Diegelmann
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Three Pivot Quicksort
//  a three pivot quicksort implementation switching to three way 
//  partitioning if usefull

#ifndef SORT_INPLACE_H
#define SORT_INPLACE_H

#ifdef __INTEL_COMPILER
#pragma warning disable 3373
#endif

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace sort {

namespace detail {

namespace misc {

constexpr const int INSERTION_MAX =   32;  // When to switch to insertion sort
constexpr const int MEDIAN21      =  256;  // When to switch to pseudo median of 21
constexpr const int MEDIAN65      = 8192;  // When to switch to pseudo median of 65
constexpr const int NOCB          =    2;  // Don't use the callback on equal ranges (implies LR)
constexpr const int LR            =    1;  // Direction: left to right
constexpr const int RL            =    0;  // Direction: right to left

template <class T> int ilogb(T v) {
#ifdef __GNUC__
  return 3 * (31 - __builtin_clz(v)) >> 1;
#else
  int r = 0;
  while (v >>= 1)
    ++r;
  return 3 * r >> 1;
#endif
}

template <typename, typename = void>
struct has_xor_operator 
    : std::false_type {};

template <typename T>
struct has_xor_operator<T, decltype(void(std::declval<T>() ^ std::declval<T>()))>
    : std::true_type {};

template <class V> inline void cswap(V &a, V &b) {
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
  if (has_xor_operator<V>::value) {
    std::remove_reference_t<V> da = a, db = b, tmp;
    tmp = a = da < db ? da : db;
    b ^= da ^ tmp;
  } else {
    if (b < a) std::swap(a, b);
  }
#else
  // clang+icc nicely optimize this into cmoves
  if (b < a) std::swap(a, b);
#endif
}

template <class V> inline void cmovl(V &a, V b) {
  if (b < a) a = b;
}

template <class V> inline void cmovg(V a, V &b) {
  if (b < a) b = a;
}

template <class V> inline V median3(V a, V b, V c) {
  // Get median by sorting 3 elements and return the middle element
  // using a sorting network

  cswap(a, c); cmovg(a, b);
  cmovl(b, c);

  return b;
}

template <class V> inline V median5(V a, V b, V c, V d, V e) {
  // Get median by sorting 5 elements and return the middle element
  // using a sorting network

  cswap(a, b); cswap(d, e);
  cswap(c, e); cswap(c, d);
  cswap(a, d); cmovg(a, c);
  cmovl(b, e); cmovl(b, d);
  cmovg(b, c);

  return c;
}

template <class V, class T, class I>
inline std::tuple<V, V, V> median7(T first, I index) {
  // Get 3 pivots by sorting 7 elements and returning element 1, 3 and 5
  // using a sorting network

  V a = index(first[0]);
  V c = index(first[2]);
  V e = index(first[4]);
  V g = index(first[6]);

  cswap(c, g); cswap(a, e);
  V b = index(first[1]);
  V f = index(first[5]);
  cswap(b, f); cswap(e, g);
  cswap(a, c);
  V d = index(first[3]);
  cswap(b, d); cswap(c, e);
  cswap(d, f); cmovg(a, b);
  cswap(e, f); cswap(c, d);
  cswap(b, e); cswap(d, g);
  cmovl(b, c); cmovl(d, e);
  cmovl(f, g);

  return std::make_tuple(b, d, f);
}

template <class V, class T, class I>
static std::tuple<V, V, V> median15(T first, I index) {
  // Get 3 pivots by sorting 15 elements and returning element 4, 8 and 12

  V a = index(first[ 0]);
  V b = index(first[ 1]);
  V c = index(first[ 2]);
  V d = index(first[ 3]);
  cswap(a, b); cswap(c, d);
  V e = index(first[ 4]);
  V f = index(first[ 5]);
  V g = index(first[ 6]);
  V h = index(first[ 7]);
  cswap(e, f); cswap(g, h);
  V i = index(first[ 8]);
  V j = index(first[ 9]);
  V k = index(first[10]);
  V l = index(first[11]);
  cswap(i, j); cswap(k, l);
  V m = index(first[12]);
  V n = index(first[13]);
  cswap(m, n); cswap(a, c);
  cswap(e, g); cswap(i, k);
  V o = index(first[14]);
  cswap(m, o); cswap(b, d);
  cswap(f, h); cswap(j, l);
  cswap(a, e); cswap(i, m);
  cswap(b, f); cswap(j, n);
  cswap(c, g); cswap(k, o);
  cswap(d, h); cmovg(a, i);
  cswap(b, j); cswap(c, k);
  cswap(d, l); cswap(e, m);
  cswap(f, n); cswap(g, o);
  cswap(f, k); cswap(g, j);
  cswap(d, m); cswap(n, o);
  cswap(h, l); cswap(b, c);
  cswap(e, i); cmovg(b, e);
  cswap(h, n); cswap(c, i);
  cmovl(l, o); cmovg(c, e);
  cswap(f, g); cswap(j, k);
  cmovl(l, n); cswap(d, i);
  cswap(h, m); cswap(g, i);
  cmovg(k, m); cswap(d, f);
  cmovl(h, j); cmovl(d, e);
  cmovg(f, g); cmovl(h, i);
  cmovl(l, m); cmovg(g, h);

  return std::make_tuple(d, h, l);
}

template <class T, class I, class V>
inline std::pair<T, T> partition(T first, T last, I index, V pa) {
  using W = std::remove_reference_t<decltype(*first)>;
  // Doesn't assume that pa is in [first, last)
  // see "Engineering a Sort Function" - BENTLEY, McILROY

  auto a = first, c = std::prev(last);
  auto b = first, d = last;

  W bv, cv;
  while (true) {
    for (V v; b <= c && (v = index(bv = *b)) <= pa; ++b)
      if (v == pa) *b = *a, *a++ = bv;

    for (V v; b <= c && pa <= (v = index(cv = *c)); --c)
      if (v == pa) *c = *--d, *d = cv;

    if (b > c) break;

    *c-- = bv; *b++ = cv;
  }

  auto s = std::min(first + (b - a), a);
  std::swap_ranges(first, s, b - (s - first));

  auto t = std::min(last + (b - d), d);
  std::swap_ranges(b, t, last - (t - b));

  return std::make_pair(first + (b - a), last + (b - d));
}

template <class T, class I, class V>
inline std::pair<T, T> exchange1(T a, T f, I index, V pa) {
  using W = std::remove_reference_t<decltype(*a)>;
  // Assumes pa to be at least the median of 3 elements in [a, f)
  // see "Engineering a Sort Function" - BENTLEY, McILROY

  // [a, b) == pa, [b, c) <  pa
  // [c, d) -> unknown elements
  // [d, e) >  pa, [e, f) == pa

  auto b = a;
  while (b != f && index(*b) == pa) ++b;
  if (b == f) return std::make_pair(a, f);
  b -= a != b; // we need at least one pivot element in the range

  W cv, dv;
  auto c = b, d = f, e = f;
  while (true) {
    V v1, v2;
    while ((v1 = index(cv = *c++)) < pa);
    while (pa < (v2 = index(dv = *--d)));

    if (--c >= d) break;

    if (v1 == pa) *d = *--e, *e = cv; else *d = cv;
    if (v2 == pa) *c = *b, *b++ = dv; else *c = dv;
    ++c;
  }
  d = c + (c == d); // c == d means center element is equal to pa, keep it there

  auto s = std::min(a + (c - b), b);
  std::swap_ranges(a, s, c - (s - a));

  auto t = std::min(d + (f - e), e);
  std::swap_ranges(d, t, f - (t - d));

  return std::make_pair(a + (c - b), d + (f - e));
}

template <class T, class I, class V>
inline auto exchange3(T first, T last, I index, V pa, V pb, V pc) {
  using W = std::remove_reference_t<decltype(*first)>;
  // Assumes pa < pb < pc and all exists within [first, last)
  // see "How Good is Multi-Pivot Quicksort?" - Aumueller, Dietzfelbinger, Klaue
  auto a = first, c = last - 1;
  auto b = first, d = last;

  W bv, cv;
  V v1, v2;
  while (true) {
    for (; !(pb < (v1 = index(bv = *b))); ++b)
      if (v1 < pa) *b = *a, *a++ = bv;

    for (;  (pb < (v2 = index(cv = *c))); --c)
      if (v2 > pc) *c = *--d, *d = cv;

    if (b > c)
      break;

    if (v2 < pa) *b = *a, *a++ = cv; else *b = cv;
    if (v1 > pc) *c = *--d, *d = bv; else *c = bv;
    ++b; c--;
  }

  return std::make_tuple(a, b, d);
}

// I refactored this out to always give them the same name
template <class I> inline auto compare(I index) {
  return [index](auto a, auto b) { return index(a) < index(b); };
}

template <int LR, class T, class I, class C>
inline void call_range(T first, T last, I index, C cb) {
  // Callbacks in LR or RL
  if (LR != detail::misc::NOCB) {
    if (LR) for (auto itf = first; itf < last;) {
      auto itl = itf + 1;
      while (itl < last && index(*itf) == index(*itl)) ++itl;
      cb(itf, itl);
      itf = itl;
    } else for (auto itl = last; itl > first;) {
      auto itf = itl - 1;
      while (itf > first && index(itf[-1]) == index(itl[-1])) --itf;
      cb(itf, itl);
      itl = itf;
    }
  }
}

} // misc

namespace inplace {

template <int LR, class T, class I, class C>
inline void insertion(T first, T last, I index, C cb) {
  // Insertion sort
  for (auto i = first + 1, j = i; i < last; ++i) {
    auto tmp = *i;
    auto val = index(tmp);
    for (j = i; j > first && val < index(j[-1]); --j)
      *j = j[-1];
    *j = tmp;
  }

  // Callbacks
  return detail::misc::call_range<LR>(first, last, index, cb);
}

template <int LR, class T, class I, class C>
static void quick(T first, T last, I index, C &&cb, int budget) {
  using V = std::remove_reference_t<decltype(index(*first))>;

  while (1) {
    // Simple insertion sort on small groups
    if (std::distance(first, last) <= detail::misc::INSERTION_MAX)
      return detail::inplace::insertion<LR>(first, last, index, cb);

    // Switch to heap sort when quicksort degenerates
    if (budget-- == 0) {
      auto cmp = detail::misc::compare(index);
      std::make_heap(first, last, cmp);
      std::sort_heap(first, last, cmp);
      return detail::misc::call_range<LR>(first, last, index, cb);
    }

    V a, b, c;
    if (std::distance(first, last) < detail::misc::MEDIAN21) {
      // Get 3 pivots using median of 7
      std::tie(a, b, c) = detail::misc::median7<V>(first, index);
    } else if (std::distance(first, last) < detail::misc::MEDIAN65) {
      // Get 3 pivots using pseudo median of 21
      auto middle = first + std::distance(first, last) / 2;
      V a1, b1, c1;
      V a2, b2, c2;
      V a3, b3, c3;
      std::tie(a1, b1, c1) = detail::misc::median7<V>(first  - 0, index);
      std::tie(a2, b2, c2) = detail::misc::median7<V>(middle - 4, index);
      std::tie(a3, b3, c3) = detail::misc::median7<V>(last   - 8, index);
      a = detail::misc::median3(a1, a2, a3);
      b = detail::misc::median3(b1, b2, b3);
      c = detail::misc::median3(c1, c2, c3);
    } else {
      // Get 3 pivots using pseudo median of 65
      auto lower  = first + std::distance(first, last) * 1 / 4;
      auto middle = first + std::distance(first, last) * 2 / 4;
      auto upper  = first + std::distance(first, last) * 3 / 4;
      V a1, b1, c1;
      V a2, b2, c2;
      V a3, b3, c3;
      V a4, b4, c4;
      V a5, b5, c5;
      std::tie(a1, b1, c1) = detail::misc::median15<V>(first  -  0, index);
      std::tie(a2, b2, c2) = detail::misc::median15<V>(lower  -  8, index);
      std::tie(a3, b3, c3) = detail::misc::median15<V>(middle -  8, index);
      std::tie(a4, b4, c4) = detail::misc::median15<V>(upper  -  8, index);
      std::tie(a5, b5, c5) = detail::misc::median15<V>(last   - 16, index);
      a = detail::misc::median5(a1, a2, a3, a4, a5);
      b = detail::misc::median5(b1, b2, b3, b4, b5);
      c = detail::misc::median5(c1, c2, c3, c4, c5);
    }

    if (a == b || b == c) {
      // At least 3 out of 7 were equal to the pivot so switch 
      // to three way quicksort
      T d, e;
      std::tie(d, e) = detail::misc::exchange1(first, last, index, b);

      if (LR) {
        quick<LR>(first, d, index, cb, budget);
        if (LR != detail::misc::NOCB)
          cb(d, e); // equal range callback - must exist
        first = e;  // tail recursion
      } else {
        quick<LR>(e, last, index, cb, budget);
        if (LR != detail::misc::NOCB)
          cb(d, e); // equal range callback - must exist
        last = d;   // tail recursion
      }
    } else {
      // Three pivot quicksort
      T d, e, f;
      std::tie(d, e, f) = detail::misc::exchange3(first, last, index, a, b, c);

      if (LR) {
        quick<LR>(first, d, index, cb, budget);
        quick<LR>(d, e, index, cb, budget);
        quick<LR>(e, f, index, cb, budget);
        first = f; // tail recursion
      } else {
        quick<LR>(f, last, index, cb, budget);
        quick<LR>(e, f, index, cb, budget);
        quick<LR>(d, e, index, cb, budget);
        last = d; // tail recursion
      }
    }
  }
}

} // inplace

} // detail

namespace inplace {

// Fast general purpose multi pivot introsort
// with index function and optimal callback on each equal ranges.
// If you don't need an index function or a callback you should probably use
// pdqsort.
// Anyway this implementation should already be faster than std::sort for random
// data
// and especially for highly repetitive data but lacks the worst case guarantee

// Average runtime is O(n * log(m))
// Worst case is O(n * log(n))
// where m is the number of destinct values
template <int LR = detail::misc::LR, class T, class I, class C>
inline void quick(T first, T last, I index, C cb) {
  int budget = detail::misc::ilogb(last - first + 1);
  detail::inplace::quick<LR>(first, last, index, cb, budget);
}

template <int LR = detail::misc::LR, class T, class I>
inline void quick(T first, T last, I index) {
  int budget = detail::misc::ilogb(last - first + 1);
  detail::inplace::quick<LR>(first, last, index, [](auto a, auto b) {
    (void)a; (void)b;
  }, budget);
}

} // inplace

} // sort

#endif  // SORT_INPLACE_H
