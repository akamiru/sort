# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting and is recursion free (this has a performance hit of around 5%).

# benchmark
####benchmark results for the modified libdivsufsort on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File          |     Size     |   trsort    |   daware  | OMP trsort | OMP daware |
|---------------|-------------:|------------:|----------:|-----------:|-----------:|
| abac          |      200.000 |   **0.001** |     0.001 |      0.001 |      0.001 |
| *abac2*       | *44.190.433* | ***0.095*** |   *0.123* |    *0.095* |    *0.123* |
| abba          |   10.500.596 |       0.437 |     0.275 |      0.462 |  **0.273** |
| book1x20      |   15.375.420 |       0.359 | **0.283** |      0.370 |      0.291 |
| fib_s14930352 |   14.930.352 |       1.321 |     0.607 |      1.256 |  **0.603** |
| fss10         |   12.078.908 |       0.955 | **0.397** |      0.952 |      0.412 |
| fss9          |    2.851.443 |       0.126 | **0.069** |      0.142 |      0.084 |
| houston       |    3.839.141 |   **0.004** |     0.017 |      0.006 |      0.024 |
| paper5x80     |      956.322 |       0.015 | **0.011** |      0.019 |      0.013 |
| test1         |    2.097.152 |       0.063 | **0.043** |      0.072 |      0.053 |
| test2         |    2.097.152 |   **0.032** |     0.049 |      0.047 |      0.066 |
| test3         |    2.097.088 |       0.040 | **0.017** |      0.078 |      0.023 |
| sum           |   67.023.574 |       3.448 | **1.892** |      3.500 |      1.966 |

The OMP Version is libdivsufsort compiled with OpenMP. While both trsort and
daware are single threaded the times are quite influenced for small files
and therefore included in the benchmark.

abac2 is a longer version of abac and NOT an official gauntlet corpus file. Therefore
it is NOT included in the sum.

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     | trsort |   daware   | OMP trsort | OMP daware |
|--------|--------------:|-------:|-----------:|-----------:|-----------:|
| enwik8 |   100.000.000 |  2.226 |      2.152 |      2.222 |  **2.151** |
| enwik9 | 1.000.000.000 | 39.441 |     29.607 |     39.493 | **29.537** |

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
