# Wec-Systems-Tasks-2025
WEC Systems task: Parallel computing with C (includes Rust + FFI as well)

## Task 1
The task 1 programs were compiled with the highest levels of
optimisations applied using `-O3` flag. It was compiled with `clang`.

sequential.c:

> Time taken to add all elements sequentially: 586632.251740 us (micro seconds)

parallel.c: (with 4 threads)

> Time taken to add all elements in parallel: 251345.157623 us (micro seconds)

These benchmarks were run on an **AMD A6-6310** system (4Gb RAM and 1.8GHz max freq.)
