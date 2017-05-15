# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA and [General Description of DAware](https://github.com/akamiru/sort/wiki/General-Description-of-DAware) in the wiki for a high level description.

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting and is recursion free (this has a performance hit of around 5%).

# benchmark
benchmark results for the modified libdivsufsort
###benchmark results on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File            |    Size    |   daware    |    trsort   | speed up |
|-----------------|-----------:|------------:|------------:|---------:|
| abac            |     200000 |     0.00052 | **0.00037** |  0.71137 |
| abba            |   10500596 | **0.20222** |     0.44477 |  2.19939 |
| book1x20        |   15375420 | **0.27960** |     0.36002 |  1.28761 |
| fib_s14930352   |   14930352 | **0.50890** |     1.37436 |  2.70065 |
| fss10           |   12078908 | **0.26025** |     0.95504 |  3.66972 |
| fss9            |    2851443 | **0.05514** |     0.12689 |  2.30138 |
| houston         |    3839141 |     0.01873 | **0.00388** |  0.20743 |
| paper5x80       |     956322 | **0.00998** |     0.01425 |  1.42752 |
| test1           |    2097152 | **0.03112** |     0.06099 |  1.95953 |
| test2           |    2097152 | **0.02496** |     0.03215 |  1.28798 |
| test3           |    2097088 | **0.01996** |     0.03738 |  1.87229 |
| sum             |   67023574 | **1.41139** |     3.41009 |  2.41612 |

###benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File            |    Size    |   daware    |    trsort   | speed up |
|-----------------|-----------:|------------:|------------:|---------:|
| enwik8          |  100000000 | **1.95499** |     2.23273 |  1.14207 |
| enwik9          | 1000000000 | **25.1776** |     39.3035 |  1.56105 |

###benchmark results for manzini's corpus (times include ONLY the trsort/daware calls):

| File            |    Size    |   daware    |    trsort   | speed up |
|-----------------|-----------:|------------:|------------:|---------:|
| chr22.dna       |   34553758 | **0.61256** |     0.94316 |  1.53970 |
| etext99         |  105277340 | **2.32381** |     2.90260 |  1.24907 |
| gcc-3.0.tar     |   86630400 | **1.33852** |     1.45377 |  1.08610 |
| howto           |   39422105 |     0.65759 | **0.63901** |  0.97173 |
| jdk13c          |   69728899 | **1.29496** |     2.05884 |  1.58989 |
| linux-2.4.5.tar |  116254720 | **1.86110** |     1.97610 |  1.06179 |
| rctail96        |  114711151 | **2.42790** |     3.79492 |  1.56305 |
| rfc             |  116421901 | **1.98627** |     2.39494 |  1.20575 |
| sprot34.dat     |  109617186 | **2.05498** |     2.53030 |  1.23130 |
| w3c2            |  104201579 | **1.89904** |     2.78458 |  1.46631 |
| sum             |  896819039 | **16.4567** |     21.4782 |  1.30513 |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
