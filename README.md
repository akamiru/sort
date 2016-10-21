# sort
three-pivot quicksort + novel linear time suffix array construction algorithm

# info
See comments in daware.h for info on the new SACA

See quick.h for the three pivot quicksort

# benchmark
####benchmark results for the modified libdivsuft on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File          |     Size     |      trsort | daware    | OMP trsort | OMP daware |
|---------------|-------------:|------------:|----------:|-----------:|-----------:|
| abac          |      200.000 |   **0.000** |     0.001 |      0.000 |      0.001 |
| *abac2*       | *44.190.433* | ***0.094*** |   *0.095* |    *0.095* |    *0.095* |
| abba          |   10.500.596 |       0.434 | **0.302** |      0.470 |      0.305 |
| book1x20      |   15.375.420 |       0.358 | **0.290** |      0.364 |      0.300 |
| fib_s14930352 |   14.930.352 |       1.309 | **0.767** |      1.273 |      0.782 |
| fss10         |   12.078.908 |       0.957 | **0.532** |      0.954 |      0.536 |
| fss9          |    2.851.443 |       0.125 | **0.078** |      0.143 |      0.090 |
| houston       |    3.839.141 |   **0.003** |     0.008 |      0.005 |      0.011 |
| paper5x80     |      956.322 |       0.015 | **0.010** |      0.020 |      0.013 |
| test1         |    2.097.152 |       0.062 | **0.044** |      0.084 |      0.060 |
| test2         |    2.097.152 |   **0.032** |     0.044 |      0.040 |      0.061 |
| test3         |    2.097.088 |       0.040 | **0.017** |      0.076 |      0.026 |
| sum           |   67.023.574 |       3.335 | **2.093** |      3.429 |      2.185 |

The OMP Version is libdivsufsort compiled with OpenMP. While both trsort and
daware are both single threaded the times are quiet influenced for small files
and therefore included in the benchmark.

abac2 is a longer version of abac and NOT an official gauntlet corpus file. Therefore
it is NOT included in the sum.

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     | trsort | daware | OMP trsort | OMP daware |
|--------|--------------:|-------:|-------:|-----------:|-----------:|
| enwik8 |   100.000.000 |  2.226 |  2.204 |      2.222 |  **2.194** |
| enwik9 | 1.000.000.000 | 39.488 | 33.132 |     39.455 | **33.120** |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
