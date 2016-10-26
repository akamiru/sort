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
//   I would be happy if you file a pull request at github when you change
//   something
//     so we can improve the software for everyone. Of course it's your right
//     not to do so.
//     If you have questions on this feel free to report an issue.
//     http://github.com/akamiru
//   Infos on the algorithm are mainly found in sort::suffix::daware()
//     currently found around line 230.
//   I'd really like to thank all authors who make their papers available online
//     many related papers are cited in the description most of which can be
//     found on the internet.

#ifndef SORT_COPY_H
#define SORT_COPY_H

#ifdef __INTEL_COMPILER
#pragma warning disable 3373
#endif

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <type_traits>

#include "detail/misc.h"
#include "inplace.h"

namespace sort {

namespace copy {

// Oportunistic version of the quicksort
// Uses free space given to it to copy together key and value
// then sorting it
template <int LR = detail::misc::LR, class T, class U, class I, class C>
inline void quick(T first, T last, U Sf, U Sl, I index, C cb) {
  using typeA = std::remove_reference_t<decltype(*Sf)>;
  using typeB = std::remove_reference_t<decltype(*first)>;
  using typeC = std::remove_reference_t<decltype(index(*first))>;
  static_assert(std::is_same<typeA, std::pair<typeB, typeC>>::value, "Type mismatch");

  if (detail::misc::COPY_MIN <= std::distance(first, last)
      && std::distance(first, last) < std::distance(Sf, Sl)) {
    Sl = Sf + std::distance(first, last);

    // get a pivot
	  typeC pivot; int equals;
    std::tie(pivot, equals) = detail::misc::median7_copy<typeC>(first, index);
	  if (equals > 4) return sort::inplace::quick<LR>(first, last, index, cb);

    // copy together
    auto a = Sf, b = Sl;
    for (auto it = first; it != last; ++it) {
      auto v = std::make_pair(*it, index(*it));
      *(v.second < pivot ? a++: --b) = v;
    }

    auto idx = [](auto a) { return a.second; };
    auto icb = [first, Sf, cb](auto a, auto b) {
      // copy back
      for (auto it = a; it != b; ++it)
        first[it - Sf] = it->first;

      // call the cb
      cb(first + (a - Sf), first + (b - Sf));
    };

    if (LR) {
      sort::inplace::quick<LR>(Sf, a, idx, icb);
      sort::inplace::quick<LR>(a, Sl, idx, icb);
    } else {
      sort::inplace::quick<LR>(a, Sl, idx, icb);
      sort::inplace::quick<LR>(Sf, a, idx, icb);
    }
  } else  // not enough space
    sort::inplace::quick<LR>(first, last, index, cb);
}

template <int LR = detail::misc::LR, class T, class U, class I>
inline void quick(T first, T last, U Sf, U Sl, I index) {
  using typeA = std::remove_reference_t<decltype(*Sf)>;
  using typeB = std::remove_reference_t<decltype(*first)>;
  using typeC = std::remove_reference_t<decltype(index(*first))>;
  static_assert(std::is_same<typeA, std::pair<typeB, typeC>>::value, "Type mismatch");

  if (detail::misc::COPY_MIN / sizeof(std::pair<typeB, typeC>) <= std::distance(first, last)
   && std::distance(first, last) < std::distance(Sf, Sl)) {
    Sl = Sf + std::distance(first, last);

    // get a pivot
    typeC pivot;
    std::tie(std::ignore, pivot, std::ignore) = detail::misc::median7<typeC>(first, index);

    // copy together
    auto a = Sf, b = Sl;
    for (auto it = first; it != last; ++it) {
      auto v = std::make_pair(*it, index(*it));
      *(v.second < pivot ? a++ : --b) = v;
    }

    auto idx = [](auto a) { return a.second; };

    if (LR) {
      sort::inplace::quick<LR>(Sf, a, idx);
      sort::inplace::quick<LR>(a, Sl, idx);
    } else {
      sort::inplace::quick<LR>(a, Sl, idx);
      sort::inplace::quick<LR>(Sf, a, idx);
    }

    for (auto it = Sf; it != Sl; ++it)
      first[std::distance(Sf, it)] = it->first;
  } else  // not enough space
    sort::inplace::quick<LR>(first, last, index);
}

} // copy

} // sort

#endif  // SORT_COPY_H
