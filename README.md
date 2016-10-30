# sort
three-pivot quicksort + novel linear time suffix array construction algorithm by depth awareness

# info
See comments in suffix.h for info on the new SACA

See inplace.h for the three-pivot quicksort

Daware is now able to use additional memory to speed up the sorting and is recursion free (this has a performance hit of around 5%).

# benchmark
####benchmark results for the modified libdivsufsort on the gauntlet corpus (times include ONLY the trsort/daware calls):

| File          |     Size     |    daware     |     trsort    |
|---------------|-------------:|--------------:|--------------:|
| abac          |       200000 |       0.00072 |   **0.00044** |
| abba          |     10500596 |   **0.21695** |       0.44065 |
| book1x20      |     15375420 |   **0.34858** |       0.42186 |
| fib_s14930352 |     14930352 |   **0.52311** |       1.27228 |
| fss10         |     12078908 |   **0.29139** |       0.96954 |
| fss9          |      2851443 |   **0.05968** |       0.12723 |
| houston       |      3839141 |       0.01902 |   **0.00460** |
| paper5x80     |       956322 |   **0.01323** |       0.01690 |
| test1         |      2097152 |   **0.04363** |       0.07027 |
| test2         |      2097152 |   **0.03824** |       0.04143 |
| test3         |      2097088 |   **0.02161** |       0.03987 |
| sum           |     67023574 |   **1.57616** |       3.40507 |

####benchmark results for enwik8 and enwik9 from the LTCB (times include ONLY the trsort/daware calls):

| File   |      Size     |   daware   | trsort |
|--------|--------------:|-----------:|-------:|
| enwik8 |   100.000.000 |  **1.959** |  2.226 |
| enwik9 | 1.000.000.000 | **26.128** | 39.441 |

####benchmark results for Manzini's Corpus (times include ONLY the trsort/daware calls):

| File            |     Size    |   daware   |   trsort   |
|-----------------|------------:|-----------:|-----------:|
| chr22.dna       | 34.553.758  |  **0.628** |      0.944 |
| etext99         | 105.277.340 |  **2.346** |      2.903 |
| gcc-3.0.tar     | 86.630.400  |  **1.372** |      1.455 |
| howto           | 39.422.105  |      0.667 |  **0.641** |
| jdk13c          | 69.728.899  |  **1.418** |      2.064 |
| linux-2.4.5.tar | 116.254.720 |  **1.843** |      2.025 |
| rctail96        | 114.711.151 |  **2.535** |      3.819 |
| rfc             | 116.421.901 |  **2.000** |      2.392 |
| sprot34.dat     | 109.617.186 |  **2.092** |      2.534 |
| w3c2            | 104.201.579 |  **1.961** |      2.788 |
| sum             | 896.819.039 | **16.862** |     21.565 |

All benchmarks were carried out on a i7 4770K with 16 GB RAM. Both were compiled with Visual Studio 2015 under Windows 10 in default Release mode.
