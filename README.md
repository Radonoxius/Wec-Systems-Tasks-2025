# Wec-Systems-Tasks-2025
WEC Systems task: Parallel computing with C (includes a touch of Rust + FFI as well)

## Task 1
The task 1 programs were compiled with the highest levels of
optimisations applied using `-O3` flag. It is compiled with `clang`.

sequential.c:

> Time taken to add all elements sequentially: 586632.251740 us (micro seconds)

parallel.c: (with 4 threads)

> Time taken to add all elements in parallel: 251345.157623 us (micro seconds)

These benchmarks were run on an **AMD A6-6310** system (4Gb RAM and 1.8GHz max freq.)
These executables are available in the _Task 1 Submission_ release as well.

## Task 2
The task 2 programs were compiled with highest levels of
optimisations applied.

shracker_serial: (Hash 1)

> The computation took: 82 s

shracker: (Hash 1) (with 8 threads)

> The computation took: 14 s

Hash 2 takes about 15 minutes (with 8 threads)

Note: These numbers show the performance using `atomics`!

These benchmarks were run on **Radxa Rock 5C** (2.4GHz and 1.8GHz max freq cores.)
These executables for x64 are available in the _Task 2 Submission_ release.

## Task 3
This program in **LINUX-ONLY** and doesnt work on other platforms!

You need to have a GPU/Device that supports `OpenCLv2` or higher and `SVM memory (coarse-grained)`.
Youll also need the official drivers. (Integrated GPU might be preferred)

This program was tested using **ARM Mali-G610** gpu on **Radxa Rock 5C**.
The executable for x64 is available in the _Task 3 Submission_ release.
