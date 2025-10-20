# Wec-Systems-Tasks-2025
WEC Systems task: Parallel computing with C (includes Rust + FFI as well)

## Task 1
The task 1 programs were compiled with the highest levels of
optimisations applied using `-O3` flag. It is compiled with `clang`.

sequential.c:

> Time taken to add all elements sequentially: 586632.251740 us (micro seconds)

parallel.c: (with 4 threads)

> Time taken to add all elements in parallel: 251345.157623 us (micro seconds)

These benchmarks were run on an **AMD A6-6310** system (4Gb RAM and 1.8GHz max freq.)
These executables are available in the _Task 1 Submission_ release as well.

## Tast 2
The task 2 programs were compiled with highest levels of
optimisations applied.

Hash 1 answer: `system`

Hash 2 answer: `kerNEl` (Note: Using the newly given hash value)

shracker_serial.rs: (Hash 1)

> The computation took 174 s

shracker.rs: (Hash 1) (with 8 threads)

> The computation took 73 s

Hash 2 takes about 45 minutes (with 8 threads)

These benchmarks were run on a **MediaTek Helio G85** device (4Gb RAM with 2GHz and 1.8GHz max freq cores.)
These executables for x64 are available in the _Task 2 Submission_ release.
