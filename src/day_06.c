/*
 * AoC 2021 Day 6 solution
 */

#include "utils.h"

/*
 * Number of unique fish states, starting from 0 going up to NUM_FISH_STATES-1
 */
#define NUM_FISH_STATES 9

/*
 * fish_status_type
 *
 * Element: fish_states
 *     Array of fish statuses. There are NUM_FISH_STATES elements where each
 *     element is the number of fish currently in that state.
 */
typedef struct Fish_Status {
    size_t fish_states[NUM_FISH_STATES];
} fish_status_type;

/*
 * initialise_fish_status_type
 *
 * Set the values of a fish_status struct to their initial values (0).
 *
 * Argument: fish_status
 *     Fish status struct to initiliase.
 *
 * Return: void
 */
static void
initialise_fish_status_type(fish_status_type *fish_status)
{
    size_t i;

    for (i = 0; i < NUM_FISH_STATES; i++) {
        fish_status->fish_states[i] = 0;
    }
}

/*
 * parse_line_into_fish_status
 *
 * Parse a line of comma-separated fish values into a fish_status struct.
 *
 * Argument: line
 *     Comma separated initial values of fish.
 *
 * Return: fish_status_type
 */
static fish_status_type
parse_line_into_fish_status(char *line)
{
    fish_status_type  fish_status;
    parsed_text_type  split_text;
    size_t            i;

    split_text = split_string_on_char(line, ',');

    initialise_fish_status_type(&fish_status);
    for (i = 0; i < split_text.num_lines; i++) {
        fish_status.fish_states[atol(split_text.lines[i].line)]++;
    }

    free_parsed_text(split_text);

    return (fish_status);
}

/*
 * iterate_day
 *
 * Calculate and set the fish status after the next day according to the
 * following rules:
 *   1. If a fish is currently on 0, reset to 6 and spawn a new fish. Newly
 *      spawned fish have a value of 8.
 *   2. If a fish is not on 0, decrease it by 1.
 *
 * Argument: fish_status
 *     Fish status struct to iterate.
 *
 * Return: void
 */
static void
iterate_day(fish_status_type *fish_status)
{
    size_t           i;
    fish_status_type new_fish_status;

    initialise_fish_status_type(&new_fish_status);
    for (i = 0; i < NUM_FISH_STATES; i++) {
        if (i == 0) {
            new_fish_status.fish_states[6] += fish_status->fish_states[0];
            new_fish_status.fish_states[8] += fish_status->fish_states[0];
        } else {
            new_fish_status.fish_states[i-1] += fish_status->fish_states[i];
        }
    }

    *fish_status = new_fish_status;
}

/*
 * calculate_number_of_fish
 *
 * Calculate the number of current fish.
 *
 * Argument: fish_status
 *     Fish status struct.
 *
 * Return: size_t
 */
static size_t
calculate_number_of_fish(fish_status_type fish_status)
{
    size_t num_fish = 0;
    size_t i;

    for (i = 0; i < NUM_FISH_STATES; i++) {
        num_fish += fish_status.fish_states[i];
    }

    return (num_fish);
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
    fish_status_type  fish_status;
    size_t            day;

    parsed_text = parse_file(file_name);

    fish_status = parse_line_into_fish_status(parsed_text.lines[0].line);

    for (day = 0; day < 80; day++) {
        iterate_day(&fish_status);
    }
    if (print_output) {
        printf("Part 1: Number of fish after %zu days = %zu\n",
            day, calculate_number_of_fish(fish_status));
    }

    for (; day < 256; day++) {
        iterate_day(&fish_status);
    }
    if (print_output) {
        printf("Part 1: Number of fish after %zu days = %zu\n",
           day, calculate_number_of_fish(fish_status));
    }

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
