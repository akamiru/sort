# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting and is recursion free (this has a performance hit of around 5%).

# benchmark
####benchmark results for the modified libdivsufsort on the gauntlet corpus (times include ONLY the trsort/daware calls):
| File          |     Size     |    daware     |     trsort    |    speed up   |
|---------------|-------------:|--------------:|--------------:|--------------:|
| abac          |       200000 |       0.00062 |   **0.00044** |       0.70819 |
| abba          |     10500596 |   **0.20459** |       0.43929 |       2.14713 |
| book1x20      |     15375420 |   **0.34036** |       0.41807 |       1.22833 |
| fib_s14930352 |     14930352 |   **0.49284** |       1.29884 |       2.63541 |
| fss10         |     12078908 |   **0.27180** |       0.96237 |       3.54077 |
| fss9          |      2851443 |   **0.05758** |       0.12660 |       2.19888 |
| houston       |      3839141 |       0.01980 |   **0.00457** |       0.23091 |
| paper5x80     |       956322 |   **0.01265** |       0.01677 |       1.32561 |
| test1         |      2097152 |   **0.04073** |       0.06961 |       1.70914 |
| test2         |      2097152 |   **0.03523** |       0.03874 |       1.09972 |
| test3         |      2097088 |   **0.02155** |       0.04045 |       1.87681 |
| sum           |     67023574 |   **1.49774** |       3.41574 |       2.28059 |

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     |   daware   | trsort |
|--------|--------------:|-----------:|-------:|
| enwik8 |   100.000.000 |  **1.901** |  2.226 |
| enwik9 | 1.000.000.000 | **24.976** | 39.441 |

####benchmark results for Manzini's Corpus (times include ONLY the trsort/daware calls):

| File            |     Size    |   daware   |   trsort   |
|-----------------|------------:|-----------:|-----------:|
| chr22.dna       | 34.553.758  |  **0.608** |      0.944 |
| etext99         | 105.277.340 |  **2.262** |      2.903 |
| gcc-3.0.tar     | 86.630.400  |  **1.295** |      1.455 |
| howto           | 39.422.105  |  **0.633** |      0.641 |
| jdk13c          | 69.728.899  |  **1.284** |      2.064 |
| linux-2.4.5.tar | 116.254.720 |  **1.798** |      2.025 |
| rctail96        | 114.711.151 |  **2.408** |      3.819 |
| rfc             | 116.421.901 |  **1.930** |      2.392 |
| sprot34.dat     | 109.617.186 |  **2.053** |      2.534 |
| w3c2            | 104.201.579 |  **1.861** |      2.788 |
| sum             | 896.819.039 | **16.132** |     21.565 |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
