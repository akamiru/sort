# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting and is recursion free (this has a performance hit of around 5%).

# benchmark
####benchmark results for the modified libdivsufsort on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File          |     Size     |  daware   |  trsort   |
|---------------|-------------:|----------:|----------:|
| abac          |       200000 |   0.00075 |   0.00047 |
| abba          |     10500596 |   0.24368 |   0.44355 |
| book1x20      |     15375420 |   0.35135 |   0.41513 |
| fib_s14930352 |     14930352 |   0.56601 |   1.27311 |
| fss10         |     12078908 |   0.33959 |   0.97109 |
| fss9          |      2851443 |   0.05794 |   0.12842 |
| houston       |      3839141 |   0.01967 |   0.00467 |
| paper5x80     |       956322 |   0.01312 |   0.01694 |
| test1         |      2097152 |   0.04167 |   0.06982 |
| test2         |      2097152 |   0.03724 |   0.04146 |
| test3         |      2097088 |   0.02155 |   0.03798 |
| sum           |     67023574 |   1.69258 |   3.40263 |

abac2 is a longer version of abac and NOT an official gauntlet corpus file. Therefore
it is NOT included in the sum.

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     | trsort |   daware   | OMP trsort | OMP daware |
|--------|--------------:|-------:|-----------:|-----------:|-----------:|
| enwik8 |   100.000.000 |  2.226 |      2.152 |      2.222 |  **2.151** |
| enwik9 | 1.000.000.000 | 39.441 |     29.607 |     39.493 | **29.537** |

The OMP Version is libdivsufsort compiled with OpenMP. While both trsort and
daware are single threaded the times are quite influenced for small files
and therefore included in the benchmark.

####benchmark results for Manzini's Corpus (times include ONLY the trsort/daware calls):

| File            |     Size    | trsort |   daware   | OMP trsort | OMP daware |
|-----------------|------------:|-------:|-----------:|-----------:|-----------:|
| chr22.dna       | 34.553.758  |  0.944 |  **0.742** |      0.945 |      0.747 |
| etext99         | 105.277.340 |  2.903 |      2.589 |      2.907 |  **2.583** |
| gcc-3.0.tar     | 86.630.400  |  1.455 |  **1.428** |      1.452 |      1.430 |
| howto           | 39.422.105  |  0.641 |      0.682 |  **0.639** |      0.682 |
| jdk13c          | 69.728.899  |  2.064 |  **1.541** |      2.063 |      1.551 |
| linux-2.4.5.tar | 116.254.720 |  2.025 |      1.967 |      1.979 |  **1.961** |
| rctail96        | 114.711.151 |  3.819 |      2.916 |      3.809 |  **2.908** |
| rfc             | 116.421.901 |  2.392 |  **2.205** |      2.394 |      2.208 |
| sprot34.dat     | 109.617.186 |  2.534 |  **2.274** |      2.532 |      2.279 |
| w3c2            | 104.201.579 |  2.788 |      2.132 |      2.786 |  **2.131** |
| sum             | 896.819.039 | 21.565 | **18.476** |     21.506 |     18.480 |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
