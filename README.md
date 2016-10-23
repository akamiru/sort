# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting

# benchmark
####benchmark results for the modified libdivsufsort on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File          |     Size     |      trsort | daware    | OMP trsort | OMP daware |
|---------------|-------------:|------------:|----------:|-----------:|-----------:|
| abac          |      200.000 |   **0.001** |     0.001 |      0.001 |      0.001 |
| *abac2*       | *44.190.433* | ***0.095*** |   *0.095* |    *0.095* |    *0.097* |
| abba          |   10.500.596 |       0.437 | **0.247** |      0.462 |      0.280 |
| book1x20      |   15.375.420 |       0.359 | **0.278** |      0.370 |      0.304 |
| fib_s14930352 |   14.930.352 |       1.321 |     0.621 |      1.256 |  **0.617** |
| fss10         |   12.078.908 |       0.955 | **0.379** |      0.952 |      0.386 |
| fss9          |    2.851.443 |       0.126 | **0.067** |      0.142 |      0.078 |
| houston       |    3.839.141 |   **0.004** |     0.007 |      0.006 |      0.012 |
| paper5x80     |      956.322 |       0.015 | **0.010** |      0.019 |      0.011 |
| test1         |    2.097.152 |       0.063 | **0.040** |      0.072 |      0.058 |
| test2         |    2.097.152 |   **0.032** |     0.046 |      0.047 |      0.060 |
| test3         |    2.097.088 |       0.040 | **0.017** |      0.078 |      0.023 |
| sum           |   67.023.574 |       3.448 | **1.808** |      3.500 |      1.927 |

The OMP Version is libdivsufsort compiled with OpenMP. While both trsort and
daware are single threaded the times are quite influenced for small files
and therefore included in the benchmark.

abac2 is a longer version of abac and NOT an official gauntlet corpus file. Therefore
it is NOT included in the sum.

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     | trsort |   daware   | OMP trsort | OMP daware |
|--------|--------------:|-------:|-----------:|-----------:|-----------:|
| enwik8 |   100.000.000 |  2.226 |  **2.056** |      2.222 |      2.064 |
| enwik9 | 1.000.000.000 | 39.441 | **27.587** |     39.493 |     27.594 |

####benchmark results for Manzini's Corpus (times include ONLY the trsort/daware calls):

| File            | Size        | trsort | daware    | OMP trsort | OMP daware |
|-----------------|-------------|--------|-----------|------------|------------|
| chr22.dna       | 34.553.758  | 0.944  | **0.687** | 0.945      | 0.687      |
| etext99         | 105.277.340 | 2.903  | **2.437** | 2.907      | 2.442      |
| gcc-3.0.tar     | 86.630.400  | 1.455  | 1.402     | 1.452      | **1.389**  |
| howto           | 39.422.105  | 0.641  | 0.665     | **0.639**  | 0.668      |
| jdk13c          | 69.728.899  | 2.064  | 1.466     | 2.063      | **1.456**  |
| linux-2.4.5.tar | 116.254.720 | 2.025  | 1.914     | 1.979      | **1.902**  |
| rctail96        | 114.711.151 | 3.819  | 2.737     | 3.809      | **2.731**  |
| rfc             | 116.421.901 | 2.392  | 2.100     | 2.394      | **2.098**  |
| sprot34.dat     | 109.617.186 | 2.534  | 2.213     | 2.532      | **2.202**  |
| w3c2            | 104.201.579 | 2.788  | **2.047** | 2.786      | 2.051      |
| sum             | 896.819.039 | 21.565 | 17.668    | 21.506     | **17.626** |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
