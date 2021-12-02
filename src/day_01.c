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
 * Return: void
 *
 */
int
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
 * Main function.
 */
int
main(int argc, char **argv)
{
    struct timespec   start_time, end_time;
    int               rc = 0;
    char             *file_name = NULL;
    parsed_text_type  parsed_text;
    int              *numbers_array = NULL;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    numbers_array = parse_text_to_ints(parsed_text);

    printf("Part 1: Number of increasing depths = %d\n",
           find_number_increasing(numbers_array, parsed_text.num_lines, 1));
    printf("Part 2: Number of increasing last 3 depths = %d\n",
           find_number_increasing(numbers_array, parsed_text.num_lines, 3));

    free(numbers_array);
    free_parse_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}
