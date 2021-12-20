/*
 * AoC 2021 Day 15 solution
 */

#include "utils.h"

/*
 * grid_position_type
 *
 * Element: value
 *     The risk value of this grid position.
 * Element: lowest_risk_to_position
 *     The lowest total risk level of a path from the start to this position.
 */
typedef struct Grid_Position {
    uint8_t value;
    size_t  lowest_risk_to_position;
} grid_position_type;

/*
 * grid_type
 *
 * Element: grid
 *     2D array representing the grid, where each element is a grid position
 *     struct.
 * Element: length
 *     Number of elements in the first array.
 * Element: width
 *     Number of elements in the second array.
 */
typedef struct Grid {
    grid_position_type **grid;
    uint16_t             length;
    uint16_t             width;
} grid_type;

/*
 * free_grid_type
 *
 * Free the memory allocated by a grid_type struct.
 *
 * Argument: grid
 *     Grid to free the memory of.
 *
 * Return: void
 */
static void
free_grid_type(grid_type grid)
{
    uint16_t i;

    if (grid.grid != NULL) {
        for (i = 0; i < grid.length; i++) {
            free(grid.grid[i]);
            grid.grid[i] = NULL;
        }
        free(grid.grid);
        grid.grid = NULL;
    }
}

/*
 * parse_text_into_grid
 *
 * Parse the day's input into a grid of risk values.
 *
 * Argument: parsed_text
 *     IN: Parsed text struct of the day's input.
 *
 * Return: grid_type
 */
static grid_type
parse_text_into_grid(parsed_text_type parsed_text)
{
    uint16_t  i, j;
    grid_type grid;

    grid.length = parsed_text.num_lines;
    grid.width = parsed_text.lines[0].len;
    grid.grid = malloc_b(grid.length * sizeof(grid_position_type *));
    for (i = 0; i < grid.length; i++) {
        grid.grid[i] = malloc_b(grid.width * sizeof(grid_position_type));
        for (j = 0; j < grid.width; j++) {
            grid.grid[i][j].value = parsed_text.lines[i].line[j] - '0';
            grid.grid[i][j].lowest_risk_to_position = 0;
        }
    }

    return (grid);
}

/*
 * make_full_grid
 *
 * Duplicate the input grid into a bigger tiled 5x5 grid. The input grid is
 * duplicated 4 times in each direction, where each duplication has an
 * increased risk level by 1 for each position. Risk levels above 9 wrap around
 * to 1.
 *
 * Argument: grid
 *     Grid to make a full version of.
 *
 * Return: grid_type
 */
static grid_type
make_full_grid(grid_type grid)
{
    uint16_t  i, j;
    grid_type full_grid;

    full_grid.length = grid.length * 5;
    full_grid.width = grid.width * 5;
    full_grid.grid = malloc_b(full_grid.length * sizeof(grid_position_type *));

    for (i = 0; i < full_grid.length; i++) {
        full_grid.grid[i] = malloc_b(
                                 full_grid.width * sizeof(grid_position_type));
        for (j = 0; j < full_grid.width; j++) {
            full_grid.grid[i][j].value =
                               grid.grid[i % grid.length][j % grid.width].value
                               + i / grid.length
                               + j / grid.width;
            if (full_grid.grid[i][j].value > 9) {
                /* Risk has gone above 9, wrap around */
                full_grid.grid[i][j].value -= 9;
            }
            full_grid.grid[i][j].lowest_risk_to_position = 0;
        }
    }

    return (full_grid);
}

/*
 * find_lowest_risks_for_each_position
 *
 * Find and set the lowest risk path to get to each element in the grid.
 *
 * Argument: grid
 *     Grid to find and set the lowest risk paths of.
 *
 * Return: void
 */
static void
find_lowest_risks_for_each_position(grid_type grid)
{
    uint16_t i, j;
    size_t   lowest_risk;
    bool     something_changed;

    /*
     * For each position, find the lowest total risk to that position. Moves
     * can be made upwards, downwards, left or right in the grid. If lowest
     * risks have been changed in this pass, record that and call this function
     * again, as a better path might be available coming from below or from the
     * right which is not available on the first pass.
     */
    something_changed = false;
    for (i = 0; i < grid.length; i++) {
        for (j = 0; j < grid.width; j++) {
            if (i == 0 && j == 0) {
                /* Starting position, risk is 0 */
                grid.grid[i][j].lowest_risk_to_position = 0;
                continue;
            }

            if (grid.grid[i][j].lowest_risk_to_position == 0) {
                /* Not attempt yet, default to coming from left or above */
                if (j != 0) {
                    lowest_risk = grid.grid[i][j-1].lowest_risk_to_position;
                } else {
                    lowest_risk = grid.grid[i-1][j].lowest_risk_to_position;
                }
                grid.grid[i][j].lowest_risk_to_position =
                                           lowest_risk + grid.grid[i][j].value;
                something_changed = true;
            } else {
                /* Already attempted, use current calculated risk */
                lowest_risk = grid.grid[i][j].lowest_risk_to_position
                                                    - grid.grid[i][j].value;
            }

            /* Check above */
            if (i > 0
                && lowest_risk > grid.grid[i-1][j].lowest_risk_to_position) {
                lowest_risk = grid.grid[i-1][j].lowest_risk_to_position;
            }
            /* Check below */
            if (i < grid.length - 1
                && grid.grid[i+1][j].lowest_risk_to_position != 0
                && lowest_risk > grid.grid[i+1][j].lowest_risk_to_position) {
                lowest_risk = grid.grid[i+1][j].lowest_risk_to_position;
            }
            /* Check left */
            if (j > 0
                && lowest_risk > grid.grid[i][j-1].lowest_risk_to_position) {
                lowest_risk = grid.grid[i][j-1].lowest_risk_to_position;
            }
            /* Check right */
            if (j < grid.width - 1
                && grid.grid[i][j+1].lowest_risk_to_position != 0
                && lowest_risk > grid.grid[i][j+1].lowest_risk_to_position) {
                lowest_risk = grid.grid[i][j+1].lowest_risk_to_position;
            }

            /* If there was a lower risk than the current, set it */
            if (lowest_risk + grid.grid[i][j].value
                < grid.grid[i][j].lowest_risk_to_position) {
                grid.grid[i][j].lowest_risk_to_position =
                                           lowest_risk + grid.grid[i][j].value;
                something_changed = true;
            }
        }
    }

    if (something_changed) {
        find_lowest_risks_for_each_position(grid);
    }
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
    parsed_text_type parsed_text;
    grid_type        grid;
    grid_type        full_grid;

    parsed_text = parse_file(file_name);

    grid = parse_text_into_grid(parsed_text);

    find_lowest_risks_for_each_position(grid);
    if (print_output) {
        printf("Part 1: Lowest path total risk = %zu\n",
               grid.grid[grid.length-1][grid.width-1].lowest_risk_to_position);
    }

    full_grid = make_full_grid(grid);
    find_lowest_risks_for_each_position(full_grid);
    if (print_output) {
        printf("Part 2: Lowest path total risk = %zu\n",
               full_grid.grid[full_grid.length-1][full_grid.width-1]
               .lowest_risk_to_position);
    }

    free_grid_type(full_grid);
    free_grid_type(grid);

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

    run_main_func_with_benchmark(runner, file_name,
                                 NUM_TIMES_TO_BENCHMARK_SMALL);

    return (0);
}
