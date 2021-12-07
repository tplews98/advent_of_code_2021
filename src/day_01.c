/*
 * AoC 2021 Day 1 solution
 */

#include "utils.h"

/*
 * find_number_increasing
 *
 * Find the number of occurances where for an index i, the sum of the elements
 * i, i-1, ..., i-num_previous_to_add is smaller than the sum of the elements
 * i-1, i-2, ..., i-num_previous_to_add-1.
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: num_previous_to_add
 *     Number of values to consider in sum.
 *
 * Return: size_t
 *
 */
size_t
find_number_increasing(int    *numbers_array,
                       size_t  len,
                       size_t  num_previous_to_add)
{
    size_t num_increasing = 0;
    size_t i;
    int    previous_sum = 0;
    int    current_sum = 0;

    assert(num_previous_to_add > 0);
    assert(num_previous_to_add <= len);

    for (i = num_previous_to_add; i < len; i++) {
        previous_sum = 0;
        current_sum = 0;
        for (int j = 0; j < num_previous_to_add; j++) {
            previous_sum += numbers_array[i-j-1];
            current_sum += numbers_array[i-j];
        }
        if (current_sum > previous_sum) {
            num_increasing++;
        }
    }

    return (num_increasing);
}

/*
 * runner
 *
 * Function which performs the days task. Repeat multiple times to benchmark.
 *
 * Argument: file_name
 *     File to read input from.
 * Argument: print_output
 *     Whether to print output or not. This should only be done on the final
 *     run to not spam the console.
 *
 * Return: void
 */
static void
runner(char *file_name, bool print_output)
{
    parsed_text_type  parsed_text;
    int              *numbers_array = NULL;
    size_t            num_increasing;

    parsed_text = parse_file(file_name);

    numbers_array = parse_text_to_ints(parsed_text);

    num_increasing = find_number_increasing(numbers_array,
                                            parsed_text.num_lines, 1);
    if (print_output) {
        printf("Part 1: Number of increasing depths = %zu\n", num_increasing);
    }

    num_increasing = find_number_increasing(numbers_array,
                                            parsed_text.num_lines, 3);
    if (print_output) {
        printf("Part 2: Number of increasing last 3 depths = %zu\n",
               num_increasing);
    }

    free(numbers_array);
    free_parsed_text(parsed_text);
}

/*
 * Main function.
 */
int
main(int argc, char **argv)
{
    char *file_name = NULL;

    assert(argc == 2);
    file_name = argv[1];

    run_main_func_with_benchmark(runner, file_name);

    return (0);
}
