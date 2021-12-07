# C Advent of Code 2021

My attempt at [Advent of Code 2021](https://adventofcode.com/2021/) using C.

All solution code should be put under `src`. The filename for each day should
be `day_xx.c`.

All input should be put in `data`. The filename for the real input of each day
should be `day_xx_input.txt`, and test input `day_xx_test_input.txt`.

To run a day's code, do `./run <day>`.

To compile a day's code, do `./run <day> --compile`. O3 optimisation with clang is used.

To run a day's code with test input, do `./run <day> --test`.

To run a day's code under valgrind to check for memory leaks, use `--valgrind`.

To run a day's code under gdb to debug, use `--gdb`. This will recompile without optimisation before starting gdb, then recompile with optimisation again afterwards.

Use `all` instead of a day to run/compile all days at once.
