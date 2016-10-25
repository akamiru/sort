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
//     currently found around line 205.
//   I'd really like to thank all authors who make their papers available online
//     many related papers are cited in the description most of which can be
//     found on the internet.

#ifndef SORT_SUFFIX_H
#define SORT_SUFFIX_H

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

#include "inplace.h"
#include "copy.h"

// define if additional space may be used
#define USE_COPY

namespace sort {

namespace detail {

namespace misc {
// I refactored this out to always give them the same name
// This may result in better code sharing
template <class U, class D>
inline auto index(U ISA, D depth) {
  // We don't have to check if a + depth > n because
  // all groups leading up to n are always already unique
  return [ISAd = ISA + depth](auto a) { return ISAd[a]; };
}

template <class A>
auto castTo() {
  return [](auto val) { return static_cast<std::remove_reference_t<A>>(val); };
}

} // misc

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

  // Use partition if it is less work
  auto castToIndex = detail::misc::castTo<decltype(*ISA)>();

  while (b != e && (e - b) * 2 < (b - a) + (f - e)) {
    auto cgroup = castToIndex(b - SA);
    std::for_each(b, e, [ISA, cgroup](auto a) { ISA[a] = cgroup; });
    std::tie(c, d) = detail::misc::partition(b, e, index, cgroup);
    group = cgroup;
    std::for_each(d, e, [ISA, cgroup = castToIndex(d - SA)](auto a) { ISA[a] = cgroup; });

    // Flag end of lower part if there is one
    if (a != b) b[-1] = ~b[-1];

    // Induce the center
    a = b; b = c; f = e; e = d;
  }

  // Induce upper part
  while (e != f) {
    for (auto it = f; it != e; --it) {
      //auto v = (it[-1] >> (sizeof(decltype(it[-1])) * CHAR_BIT - 1)) ^ it[-1];
      auto v = it[-1] < 0 ? ~it[-1] : it[-1];
      // If the prev element is in the group
      if (depth <= v && ISA[v = (v - depth)] == group)
        *--d = v; // put it into the bucket
    }
    // name (needs a second pass to know the beginning)
    std::for_each(d, e, [ISA, cgroup = castToIndex(d - SA)](auto a) { ISA[a] = cgroup; });
    f = e; e = d;
  }

  // Induce lower part
  while (b != d) {
    auto cgroup = castToIndex(b - SA);
    for (auto it = a; it != b; ++it) {
      auto v = *it;
      // If the prev element is in the group
      if (depth <= v && ISA[v = (v - depth)] == group)
        ISA[*c++ = v] = cgroup;
    }
    // Flag end of lower part
    b[-1] = ~b[-1];

    // Induce the center
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
  // Only sort items leading to groups bigger than (or equal = tandem repeats)
  // to the current
  T a, b;
  static_assert(!std::is_reference<T>::value, "T is a reference");
  std::tie(a, b) = detail::misc::partition(first, last, index, first - SA);

  // name the type S group
  auto castToIndex = detail::misc::castTo<decltype(*ISA)>();
  std::for_each(b, last, [ISA, cgroup = castToIndex(b - SA)](auto c) { ISA[c] = cgroup; });

  // Induce sort the tandem repeats part
  return detail::suffix::induce(SA, ISA, first, a, b, last, depth, first - SA);
}

template <class T, class U, class D>
inline auto name(T SA, U ISA, D depth) {
  return [SA, ISA, depth = depth + 1](auto a, auto b) {
    auto castToIndex = detail::misc::castTo<decltype(*ISA)>();
    if (std::distance(a, b) < 2) {
      ISA[*a] = castToIndex(a - SA);
      return (void) (*a = ~*a); // Group is unique - just flag it
    }

    // Next sorting depth
    // If it's negative it was already sorted and contains the depth
    auto n = ISA[*a + depth]; // Get the element following the current
    auto ndepth = depth + ((n >> (sizeof(decltype(n)) * CHAR_BIT - 1)) & ~n);
    // auto ndepth = depth + (ISA[*a + depth] < 0 ? -ISA[*a + depth] - 1 : 0);

    // Naming Stage: rename and update depth
    std::for_each(a, b, [ISA, cgroup = castToIndex(a - SA), ndepth](auto c) {
      // Storing the depth in the next item is possible due to
      // the fact that we will never sort this pair again.
      // That this works strongly indicates O(n) time for radix sort
      // implementation
      // because it shows that every pair is sorted at max once and there are
      // only O(n) pairs.
      // The only problem arises with tandem repeats which need to be sorted
      // using induction
      ISA[c + 0] = +cgroup;
      ISA[c + 1] = -ndepth;
    });

    // Partition and return
    return (void) sort::detail::suffix::partition(SA, ISA, a, b, ndepth);
  };
}

} // suffix

} // detail

namespace suffix {

// Sort the suffix array depth aware in (theoretical) linear time
// expects the SA and ISA to be grouped by a single char
// [Sf Sl) is additional space available
#ifdef USE_COPY
template <class T, class U, class V> void daware(T SAf, T SAl, U ISAf, V Sf, V Sl) {
#else
template <class T, class U, class V> void daware(T SAf, T SAl, U ISAf) {
#endif
  // This is a "pulling" or "lazy" rather than a "pushing" version of GSACA
  //  while GSACA sorts previous elements using info of the current group
  //  this implementation sorts groups using info of subsequent elements
  //  in a way related to prefix doubling. For GSACA see
  // "Linear-time Suffix Sorting - A New Approach for Suffix Array Construction" - Baier

  // More interesting papers on the topic include
  //  - "Faster suffix sorting" - Larsson, Sadakane (Prefix Doubling)
  //  - "A Taxonomy of Suffix Array Construction Algorithms" - PUGLISI, SMYTH, TURPIN (General Overview)
  //  - probably also see "An Efficient Algorithm for Suffix Sorting" - Peng, Wang, Xue, Wei
  //    because it incorporates depth info into prefix doubling but sadly I
  //    don't have access to it

  // A "pulling" version allows us to drop all but the additional data
  // for ISA and sorting depth (correspond to Previous Pointers in GSACA).
  // see the comment at the naming stage in sort::detail::misc::name() (line 185).
  // This is more cache friendly and results in less book keeping work.
  // The depth array can be further emplaced into the ISA to achieve 8 * n
  // memory usage

  // This is enough to get a O(n) time, O(1) working space SACA using the
  // Improved Two Stage (ITS) approach
  // see "Short description of improved two-stage suffix sorting algorithm" - Mori
  // http://homepage3.nifty.com/wpage/software/itssort.txt
  // basically it's a single stage of SA-IS
  // see "Linear Suffix Array Construction by Almost Pure Induced-Sorting" - Nong, Zhang, Chan
  // [currently the actual worst case working space is O(log(n)) due to
  // introsort]

  // The current implementation has a worst and average case running time of
  // O(n * log(n)) due to the fact that it uses multi pivot introsort rather
  // than a linear time sort (which probably won't change)

  // Additional ideas:

  // "BlockQuicksort: How Branch Mispredictions don't affect Quicksort" - Edelkamp, Weiss
  // for multi pivot quicksort. But without it the reduced branch mispredictions
  // probably won't make up for the additional cache misses when not using three
  // pivot quicksort.
  // We could check by replacing insertion sort with "Pattern-defeating quicksort" - Peters
  // and setting the insertion sort limit to INT_MAX
  // https://github.com/orlp/pdqsort
  // Tests show a slow down of ~2 due to the amount of cache misses

  // "The Spreadsort High-performance General-case Sorting Algorithm" - Ross
  // Spreadsort might improve performance over pure quicksort
  // at least runtime garantee improves somewhat

  // "Improving multikey Quicksort for sorting strings with many equal elements" - Kim, Park
  // Split-end partitioning for the three way quicksort might not be optimal
  // because it is only used when we have a big amount of elements equal to
  // the pivot which results in a lot of copying. "collect-center" might be
  // faster. But sadly again I have no access to this paper.

  // We might be able to further reduce the cache misses if we could
  // classify the ISA values to S/L type while building the SA in the
  // previous stage and only sort the L type in the first stage.

  // Using sorting networks instead of insertion sort for small lists
  // might increase performance by reducing the number of comparisons.
  // Maybe compile time construction of Batcher's Odd-Even Mergesort
  // to keep the code clean.

  // Prefetching ISA when sorting
  // This should be tested on real data with valgrind - first tests are pretty
  // mixed

  // If we have additional memory available (with ITS we have an average of n left)
  // we could first copy SA[i], ISA[SA[i]] together, then sort this list and
  // then copy back the results. That should reduce the number of cache misses
  // to near linear (quicksort itself has O(n * log(n)) cache misses with a very
  // small const factor)

  // Actual implementation:
  // Start by iterating over all groups right to left
  // sorting all suffixes until their next smaller suffix
  // Basically this equals sorting the lyndon words starting at each SA[i]
  // So this has something to do with Lyndon Trees
  // which themselves have to do with maximal reptitions
  // maybe this leads to an even faster approach in those areas
  // "A new characterization of maximal repetitions by Lyndon trees" - Bannai, I, Inenaga
  for (auto gl = SAl; gl > SAf + 1;) {
    // Name of the group equals the start of the group
    // We don't change it to the end this round because this
    // would result in some values suddenly getting a lower name
    // in the recursion which would lead to false sorting.
    auto gf = SAf + ISAf[gl[-1]];
    if (1 < std::distance(gf, gl)) {
      // everything left is type F everything right is type S
      T gc = detail::suffix::partition(SAf, ISAf, gf, gl, 1);

      T sgf = gc, sgl = gl;
      // handle all type S subgroups
      while (gc < sgl) {
        if (sgl[-1] < 0) { // is type F ?
          // skip over
          sgl = SAf + ISAf[~sgl[-1]];
        } else {  // is type S
          // get the start of the group
          sgf = SAf + ISAf[+sgl[-1]];

          if (std::distance(sgf, sgl) < 2) {
            *--sgl = ~*sgf; // we can directly skip over it
            continue;
          }

          // calculate the sorting depth
          auto n = ISAf[sgl[-1] + 1]; // Get the element following the current
          auto depth = 1 + ((n >> (sizeof(decltype(n)) * CHAR_BIT - 1)) & ~n);

          // get the index function
          auto index = detail::misc::index(ISAf, depth);

          // sort all type S
          constexpr auto RL = detail::misc::RL;
#ifdef USE_COPY
          sort::copy::quick<RL>(sgf, sgl, Sf, Sl, index, detail::suffix::name(SAf, ISAf, depth));
#else
          sort::inplace::quick<RL>(sgf, sgl, index, detail::suffix::name(SAf, ISAf, depth));
#endif
        }
      }
    } else
      *gf = ~*gf; // Group is unique - just flag it
    gl = gf;
  }

  // Induce the order of all suffixes from left to right
  for (auto gf = SAf + 1; gf < SAl;) {
    auto gl = gf;
    while (0 <= *gl++);  // End of the group is flagged
    gl[-1] = ~gl[-1];    // Flip the flag

    auto depth = ISAf[*gf + 1] < 0 ? -ISAf[*gf + 1] : 1;
    auto index = detail::misc::index(ISAf, depth);

    // All elements of the left are already unique so we simply need to sort
    constexpr auto NOCB = detail::misc::NOCB;
#ifdef USE_COPY
    sort::copy::quick<NOCB>(gf, gl, Sf, Sl, index);
#else
    sort::inplace::quick<NOCB>(gf, gl, index);
#endif
    // And give each of them a unique name
    auto castToIndex = detail::misc::castTo<decltype(*ISAf)>();
    std::for_each(gf, gl, [ISAf, SAf, castToIndex](auto &a) { ISAf[a] = castToIndex(&a - &*SAf); });

    gf = gl;
    // Scan over all unique groups
    for (; gf < SAl && *gf < 0; ++gf) {
      *gf = ~*gf;
      // Maybe we could somehow overwrite the embedded depth info in the
      // sorting stage to make this unnecessary
      ISAf[*gf] = castToIndex(gf - SAf);
    }
  }

  // Now the SA is completly sorted and ISA is completly reconstructed
}

} // suffix

} // sort

#endif  // SORT_SUFFIX_H
