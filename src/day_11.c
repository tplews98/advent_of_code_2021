/*
 * AoC 2021 Day 11 solution
 */

#include "utils.h"

/* Size of octopus grid */
#define OCTOPUS_GRID_LEN   10
#define OCTOPUS_GRID_WIDTH 10

/*
 * octopus_type
 *
 * Element: enery
 *     Current energy level of the ocotopus
 * Element: is_low_point
 *     Whether this octopus has flashed in the current step.
 */
typedef struct Octopus {
    uint16_t enery;
    bool     has_flashed;
} octopus_type;

/*
 * octopus_grid_type
 *
 * Element: grid
 *     2D array of octopus structs.
 */
typedef struct Octopus_Grid {
    octopus_type grid[OCTOPUS_GRID_LEN][OCTOPUS_GRID_WIDTH];
} octopus_grid_type;

/*
 * parse_text_into_octopus_grid
 *
 * Parse text input into an octopus grid struct.
 *
 * Argument: parsed_text
 *     Parsed text struct of input.
 *
 * Return: octopus_grid_type
 */
static octopus_grid_type
parse_text_into_octopus_grid(parsed_text_type parsed_text)
{
    size_t            i, j;
    octopus_grid_type octopus_grid;

    assert(parsed_text.num_lines == OCTOPUS_GRID_LEN);

    for (i = 0; i < OCTOPUS_GRID_LEN; i++) {
        assert(parsed_text.lines[i].len == OCTOPUS_GRID_WIDTH);
        for (j = 0; j < OCTOPUS_GRID_WIDTH; j++) {
            octopus_grid.grid[i][j].enery =
                               (uint16_t) (parsed_text.lines[i].line[j] - '0');
            octopus_grid.grid[i][j].has_flashed = false;
        }
    }

    return (octopus_grid);
}

/*
 * flash_octopus
 *
 * Flash an octopus and increase the energy of all neighbouring octopuses
 * (including diagonals). If any neighbours get an energy > 9, they also flash.
 * Returns the number of flashes caused.
 *
 * Argument: octopus_grid
 *     Octopus grid the octopus is part of.
 * Argument: i_pos
 *     Index of the octopus into the first grid array.
* Argument: j_pos
 *     Index of the octopus into the second grid array.
 *
 * Return: size_t
 */
static size_t
flash_octopus(octopus_grid_type *octopus_grid,
              size_t             i_pos,
              size_t             j_pos)
{
    octopus_type *neighbour_octopus = NULL;
    size_t        num_flashes = 0;

    if (!octopus_grid->grid[i_pos][j_pos].has_flashed
        && octopus_grid->grid[i_pos][j_pos].enery > 9) {
        num_flashes++;
        octopus_grid->grid[i_pos][j_pos].has_flashed = true;
        octopus_grid->grid[i_pos][j_pos].enery = 0;
        /*
         * Increase energy level of all neighbours if they have not already
         * flashed.
         */
        if (i_pos > 0) {
            /* Set above */
            neighbour_octopus = &(octopus_grid->grid[i_pos-1][j_pos]);
            if (!neighbour_octopus->has_flashed) {
                neighbour_octopus->enery++;
                if (neighbour_octopus->enery > 9) {
                    num_flashes += flash_octopus(octopus_grid, i_pos-1, j_pos);
                }
            }
            if (j_pos > 0) {
                /* Set above left diagonal */
                neighbour_octopus = &(octopus_grid->grid[i_pos-1][j_pos-1]);
                if (!neighbour_octopus->has_flashed) {
                    neighbour_octopus->enery++;
                    if (neighbour_octopus->enery > 9) {
                        num_flashes += flash_octopus(
                                               octopus_grid, i_pos-1, j_pos-1);
                    }
                }
            }
            if (j_pos < (OCTOPUS_GRID_WIDTH - 1)) {
                /* Set above right diagonal */
                neighbour_octopus = &(octopus_grid->grid[i_pos-1][j_pos+1]);
                if (!neighbour_octopus->has_flashed) {
                    neighbour_octopus->enery++;
                    if (neighbour_octopus->enery > 9) {
                        num_flashes += flash_octopus(
                                               octopus_grid, i_pos-1, j_pos+1);
                    }
                }
            }
        }
        if (i_pos < (OCTOPUS_GRID_LEN - 1)) {
            /* Set below */
            neighbour_octopus = &(octopus_grid->grid[i_pos+1][j_pos]);
            if (!neighbour_octopus->has_flashed) {
                neighbour_octopus->enery++;
                if (neighbour_octopus->enery > 9) {
                    num_flashes += flash_octopus(octopus_grid, i_pos+1, j_pos);
                }
            }
            if (j_pos > 0) {
                /* Set below left diagonal */
                neighbour_octopus = &(octopus_grid->grid[i_pos+1][j_pos-1]);
                if (!neighbour_octopus->has_flashed) {
                    neighbour_octopus->enery++;
                    if (neighbour_octopus->enery > 9) {
                        num_flashes += flash_octopus(
                                               octopus_grid, i_pos+1, j_pos-1);
                    }
                }
            }
            if (j_pos < (OCTOPUS_GRID_WIDTH - 1)) {
                /* Set below right diagonal */
                neighbour_octopus = &(octopus_grid->grid[i_pos+1][j_pos+1]);
                if (!neighbour_octopus->has_flashed) {
                    neighbour_octopus->enery++;
                    if (neighbour_octopus->enery > 9) {
                        num_flashes += flash_octopus(
                                               octopus_grid, i_pos+1, j_pos+1);
                    }
                }
            }
        }
        if (j_pos > 0) {
            /* Set left */
            neighbour_octopus = &(octopus_grid->grid[i_pos][j_pos-1]);
            if (!neighbour_octopus->has_flashed) {
                neighbour_octopus->enery++;
                if (neighbour_octopus->enery > 9) {
                    num_flashes += flash_octopus(octopus_grid, i_pos, j_pos-1);
                }
            }
        }
        if (j_pos < (OCTOPUS_GRID_WIDTH - 1)) {
            /* Set right */
            neighbour_octopus = &(octopus_grid->grid[i_pos][j_pos+1]);
            if (!neighbour_octopus->has_flashed) {
                neighbour_octopus->enery++;
                if (neighbour_octopus->enery > 9) {
                    num_flashes += flash_octopus(octopus_grid, i_pos, j_pos+1);
                }
            }
        }
    }

    return (num_flashes);
}

/*
 * iterate_step
 *
 * Calculate and set the octopus_grid status after the next step according to
 * the following rules:
 *   1. Increase the energy level of each octopus by 1.
 *   2. Any octopus with an energy level > 9 flash and increase the energy
 *      level of all neighbours by 1 (including diagonals). This can cause
 *      further flashes. Octopuses flash at most once during a step.
 *   3. Any octopuses that flash during this step have their energy reset to 0.
 *
 * Returns the total number of flashes in this step.
 *
 * Argument: octopus_grid
 *     Octopus grid to iterate.
 *
 * Return: size_t
 */
static size_t
iterate_step(octopus_grid_type *octopus_grid)
{
    size_t i, j;
    size_t num_flashes = 0;

    /*
     * Reset flashed state from previous step and increase the energy level of
     * all octopuses by 1
     */
    for (i = 0; i < OCTOPUS_GRID_LEN; i++) {
        for (j = 0; j < OCTOPUS_GRID_WIDTH; j++) {
            octopus_grid->grid[i][j].has_flashed = false;
            octopus_grid->grid[i][j].enery++;
        }
    }

    /* Flash octopuses */
    for (i = 0; i < OCTOPUS_GRID_LEN; i++) {
        for (j = 0; j < OCTOPUS_GRID_WIDTH; j++) {
            num_flashes += flash_octopus(octopus_grid, i, j);
        }
    }

    return (num_flashes);
}


/*
 * find_steps_needed_to_synchronise
 *
 * Find the number of steps before all octopuses in the grid flash
 * synchronously.
 *
 * Argument: octopus_grid
 *     Octopus grid to synchronise.
 *
 * Return: size_t
 */
static size_t
find_steps_needed_to_synchronise(octopus_grid_type *octopus_grid)
{
    size_t i, j;
    size_t current_step = 0;
    bool   all_synchronised;

    while (true) {
        all_synchronised = false;
        for (i = 0; i < OCTOPUS_GRID_LEN; i++) {
            for (j = 0; j < OCTOPUS_GRID_WIDTH; j++) {
                all_synchronised = (octopus_grid->grid[i][j].enery
                                    == octopus_grid->grid[0][0].enery);
                if (!all_synchronised) {
                    /* Not synchronised */
                    break;
                }
            }
        }
        if (all_synchronised) {
            /* All octopuses are synchronised */
            break;
        }
        current_step++;
        iterate_step(octopus_grid);
    }

    return (current_step);
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
    octopus_grid_type octopus_grid;
    size_t            i;
    size_t            total_flashes;
    size_t            num_steps;

    parsed_text = parse_file(file_name);

    octopus_grid = parse_text_into_octopus_grid(parsed_text);

    total_flashes = 0;
    num_steps = 100;
    for (i = 0; i < num_steps; i++) {
        total_flashes += iterate_step(&octopus_grid);
    }
    if (print_output) {
        printf("Part 1: Total number of flashes = %zu\n", total_flashes);
    }

    /* Find number of extra steps needed to synchronise */
    num_steps += find_steps_needed_to_synchronise(&octopus_grid);
    if (print_output) {
        printf("Part 2: Steps needed to synchronise = %zu\n", num_steps);
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

    run_main_func_with_benchmark(runner, file_name, NUM_TIMES_TO_BENCHMARK);

    return (0);
}
