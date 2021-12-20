/*
 * AoC 2021 Day 7 solution
 */

#include "utils.h"

/*
 * find_fuel_needed_part_1
 *
 * Find the fuel cost of all crabs moving to the middle position for part 1,
 * where the fuel cost is the difference from the middle position.
 *
 * Argument: numbers_array
 *     Array of initial crab positions.
 * Argument: len
 *     Number of elements in the array.
 * Argument: meeting_position
 *     The position the crabs will go to which should cost the least amount of
 *     fuel (the median of the array).
 *
 * Return: int
 */
static int
find_fuel_needed_part_1(int *numbers_array, size_t len, int meeting_position)
{
    int    fuel = 0;
    size_t i;

    for (i = 0; i < len; i++) {
        fuel += abs(numbers_array[i] - meeting_position);
    }

    return (fuel);
}

/*
 * find_fuel_needed_part_2
 *
 * Find the fuel cost of all crabs moving to the middle position for part 2,
 * where the fuel cost is the sum of all integers up to and including the
 * difference from the middle position.
 *
 * Argument: numbers_array
 *     Array of initial crab positions.
 * Argument: len
 *     Number of elements in the array.
 * Argument: meeting_position
 *     The position the crabs will go to which should cost the least amount of
 *     fuel (the mean of the array). Crabs can only go to an integer, so this
 *     is rounded to the floor or ceil, depending on which gives the lower
 *     total fuel.
 *
 * Return: int
 */
static int
find_fuel_needed_part_2(int *numbers_array,
                        size_t len,
                        double meeting_position)
{
    int    fuel_floor = 0;
    int    fuel_ceil = 0;
    int    diff;
    int    meeting_position_int;
    size_t i;

    /*
     * Check both the floor and ceil of the mean and choose the lowest total.
     */

    meeting_position_int = floor(meeting_position);
    for (i = 0; i < len; i++) {
        diff = abs(numbers_array[i] - meeting_position_int);
        fuel_floor += (diff * (diff + 1)) / 2;
    }

    meeting_position_int = ceil(meeting_position);
    for (i = 0; i < len; i++) {
        diff = abs(numbers_array[i] - meeting_position_int);
        fuel_ceil += (diff * (diff + 1)) / 2;
    }

    return (fuel_floor < fuel_ceil ? fuel_floor : fuel_ceil);
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
    parsed_text_type  split_text;
    int              *crab_positions = NULL;
    int               median;
    double            mean;
    int               fuel_needed;

    parsed_text = parse_file(file_name);
    split_text = split_string_on_char(parsed_text.lines[0].line, ',');
    crab_positions = parse_text_to_ints(split_text);

    sort_numbers(crab_positions, split_text.num_lines);
    median = find_median_of_sorted_array(crab_positions, split_text.num_lines);
    fuel_needed = find_fuel_needed_part_1(crab_positions,
                                          split_text.num_lines,
                                          median);
    if (print_output) {
        printf("Part 1: Median value = %d, fuel needed = %d\n",
               median, fuel_needed);
    }

    mean = find_mean_of_array(crab_positions, split_text.num_lines);
    fuel_needed = find_fuel_needed_part_2(crab_positions,
                                          split_text.num_lines,
                                          mean);
    if (print_output) {
        printf("Part 2: Mean value = %.2f, fuel needed = %d\n",
               mean, fuel_needed);
    }

    free(crab_positions);
    crab_positions = NULL;

    free_parsed_text(split_text);
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

    run_main_func_with_benchmark(runner, file_name, NUM_TIMES_TO_BENCHMARK);

    return (0);
}
