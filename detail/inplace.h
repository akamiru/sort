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

#ifndef SORT_DETAIL_INPLACE_H
#define SORT_DETAIL_INPLACE_H

#ifdef __INTEL_COMPILER
#pragma warning disable 3373
#endif

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <type_traits>

#include "misc.h"

namespace sort {

namespace detail {

namespace inplace {

template<class V, class T, class I>
inline std::tuple<V, V, V> pivot(T first, T last, I index) {
  V a, b, c;
  if (std::distance(first, last) < detail::misc::MEDIAN21) {
    // Get 3 pivots using median of 7
    std::tie(a, b, c) = detail::misc::median7<V>(first, index);
  } else if (std::distance(first, last) < detail::misc::MEDIAN65) {
    // Get 3 pivots using pseudo median of 21
    auto middle = first + std::distance(first, last) / 2;
    V a1, b1, c1; V a2, b2, c2; V a3, b3, c3;
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
    V a1, b1, c1; V a2, b2, c2; V a3, b3, c3; V a4, b4, c4; V a5, b5, c5;
    std::tie(a1, b1, c1) = detail::misc::median15<V>(first  -  0, index);
    std::tie(a2, b2, c2) = detail::misc::median15<V>(lower  -  8, index);
    std::tie(a3, b3, c3) = detail::misc::median15<V>(middle -  8, index);
    std::tie(a4, b4, c4) = detail::misc::median15<V>(upper  -  8, index);
    std::tie(a5, b5, c5) = detail::misc::median15<V>(last   - 16, index);
    a = detail::misc::median5(a1, a2, a3, a4, a5);
    b = detail::misc::median5(b1, b2, b3, b4, b5);
    c = detail::misc::median5(c1, c2, c3, c4, c5);
  }
  return std::make_tuple(a, b, c);
}

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
    std::tie(a, b, c) = detail::inplace::pivot<V>(first, last, index);

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

} // sort

#endif  // SORT_DETAIL_INPLACE_H