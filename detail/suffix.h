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

// Linear Time Suffix Sorting By Depth Awareness

// Notes from the author:
//   This is a reference implementation currently NOT running in O(n)
//     It uses multi pivot introsort rather than a linear time sorting stage
//   I would be happy if you file a pull request on github when you change
//   something so we can improve the software for everyone. Of course it's 
//   your right not to do so.
//     If you have questions on this feel free to report an issue.
//     http://github.com/akamiru
//   Infos on the algorithm are mainly found in sort::suffix::daware()
//     currently found around line 215.
//   I'd really like to thank all authors who make their papers available online
//     many related papers are cited in the description most of which can be
//     found on the internet.

#ifndef SORT_DETAIL_SUFFIX_H
#define SORT_DETAIL_SUFFIX_H

#ifdef __INTEL_COMPILER
#pragma warning disable 3373
#endif

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

#include "misc.h"
#include "../inplace.h"
#include "../copy.h"

// define if additional space may be used
#define USE_COPY

namespace sort {

namespace detail {

namespace suffix {

template <class T, class U, class D, class G>
inline T induce(T SA, U ISA, T a, T b, T e, T f, D depth, G group) {
  if (b == e) {
    if (a != b) b[-1] = ~b[-1];
    return b;
  }

  auto index = detail::misc::index(ISA, depth);
  // [a, b) = lower range          = group
  // [b, e) = range to be induced  = group
  // [e, f) = high range           > group
  T c = b, d = e;

  auto castToIndex = detail::misc::castTo<decltype(*ISA)>();

  // Use partition if it is less work
  constexpr const int s = sizeof(decltype(*a));  // this saves 3 shifts
  while (b != e && (e - b) * 4 * s < ((b - a) + (f - e)) * s) {
    auto cgroup = castToIndex(b - SA);
    std::for_each(b, e, [ISA, cgroup](auto a) { ISA[a] = cgroup; });
    std::tie(c, d) = detail::inplace::partition(b, e, index, cgroup);
    group = cgroup;
    std::for_each(d, e, [ISA, cgroup = castToIndex(d - SA)](auto a) { ISA[a] = cgroup; });

    // Flag end of lower part if there is one
    if (a != b) b[-1] = ~b[-1];
    // Iterate into the center
    a = b; b = c; f = e; e = d;
  }

  // Induce upper part
  while (e != f) {
    for (auto it = f; it != e; --it) {
      auto v = it[-1] < 0 ? ~it[-1] : it[-1];
      // If the prev element is in the group
      if (depth <= v && ISA[v = (v - depth)] == group)
        *--d = v; // put it into the bucket
    }
    // name (needs a second pass to know the beginning)
    std::for_each(d, e, [ISA, cgroup = castToIndex(d - SA)](auto a) { ISA[a] = cgroup; });
    // Iterate into the left part
    f = e; e = d;
  }

  // Induce lower part
  while (b != d) {
    auto cgroup = castToIndex(b - SA);
    for (auto it = a; it != b; ++it) {
      auto v = *it;
      // If the prev element is in the group
      if (depth <= v && ISA[v = (v - depth)] == group)
        ISA[*c++ = v] = cgroup;  // While naming can be skipped we do not benefit
    }
    // Flag end of lower part to mark it type F
    b[-1] = ~b[-1];
    // Interate into the right part
    a = b; b = c;
  }
  // Flag the center
  if (a != b) b[-1] = ~b[-1];

  return b;
}

// Partition into type L, T and S
template <class T, class U, class D>
static T partition(T SA, U ISA, T first, T last, D depth) {
  // Index function: return the next element according to the current depth
  auto index = detail::misc::index(ISA, depth);

  // Name of the group equals the index of the first element
  // Only sort items leading to groups bigger than the current
  T a, b;
  static_assert(!std::is_reference<T>::value, "T is a reference");
  std::tie(a, b) = detail::inplace::partition(first, last, index, first - SA);

  // name the type S group
  auto castToIndex = detail::misc::castTo<decltype(*ISA)>();
  std::for_each(b, last, [ISA, cgroup = castToIndex(b - SA)](auto c) { ISA[c] = cgroup; });

  // Induce sort the tandem repeats part (= equal partition) into type L and S
  return detail::suffix::induce(SA, ISA, first, a, b, last, depth, first - SA);
}

template <class T, class U, class D>
inline auto name(T SA, U ISA, D depth) {
  return [SA, ISA, depth = depth + 1](auto a, auto b) {
    auto castToIndex = detail::misc::castTo<decltype(*ISA)>();
    if (std::distance(a, b) < 2) {
      ISA[*a] = castToIndex(a - SA);
      return (void) (*a = ~*a); // Group is unique - flag it as type F
    }

    // Get the element following the current
    auto n = ISA[*a + depth];
    // If it's negative it was already sorted and contains the current sorting depth
    auto ndepth = depth + ((n >> (sizeof(decltype(n)) * CHAR_BIT - 1)) & ~n);
    // auto ndepth = depth + (ISA[*a + depth] < 0 ? -ISA[*a + depth] - 1 : 0);

    // Naming Stage: rename and update depth
    std::for_each(a, b, [ISA, cgroup = castToIndex(a - SA), ndepth](auto c) {
      // Storing the depth in the next item is possible due to
      // the fact that we will never sort this pair again.
      // That this works strongly indicates O(n) time for radix sort implementation
      // because it shows that every pair is sorted at max once and there are
      // only O(n) pairs.The only problem arises with tandem repeats which need to 
      // be sorted using induction
      ISA[c + 0] = +cgroup;
      ISA[c + 1] = -ndepth;
    });

    // Partition and return
    return (void) sort::detail::suffix::partition(SA, ISA, a, b, ndepth);
  };
}

} // suffix

} // detail

} // sort

#endif  // SORT_DETAIL_SUFFIX_H
