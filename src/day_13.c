/*
 * AoC 2021 Day 13 solution
 */

#include "utils.h"

/*
 * grid_type
 *
 * Element: grid
 *     2D array representing the grid. If the bool is true, the position has
 *     been marked and if false it has not.
 * Element: length
 *     Number of elements in the first array.
 * Element: width
 *     Number of elements in the second array.
 */
typedef struct Grid {
    bool     **grid;
    uint16_t   length;
    uint16_t   width;
} grid_type;

/*
 * fold_type
 *
 * Element: axis
 *     The axis of the fold. Must be 'x' or 'y'.
 * Element: coordinate
 *     The x or y coordinate of the fold.
 */
typedef struct Fold {
    char     axis;
    uint16_t coordinate;
} fold_type;

/*
 * folds_type
 *
 * Element: folds
 *     Array of fold_type structs.
 * Element: num_folds
 *     Number of elements in the folds array.
 */
typedef struct Folds {
    fold_type *folds;
    uint16_t   num_folds;
} folds_type;

/*
 * allocate_empty_grid
 *
 * Allocates the memory for the 2D array of a grid_type struct. The grid bools
 * are set to false to represent an unmarked point.
 *
 * Argument: grid
 *     Grid to allocate the 2D array of. The grid length and width attributes
 *     must be set to determine the size of the array to allocate.
 *
 * Return: void
 */
static void
allocate_empty_grid(grid_type *grid)
{
    uint16_t i;

    grid->grid = calloc_b(grid->length, sizeof(bool *));
    for (i = 0; i < grid->length; i++) {
        grid->grid[i] = calloc_b(grid->width, sizeof(bool));
    }
}

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
 * parse_text_into_grid_and_folds
 *
 * Parse the day's input into a grid with the initial marked points and a set
 * of folds to perform.
 *
 * Argument: parsed_text
 *     IN: Parsed text struct of the day's input.
 * Argument: grid
 *     OUT: Grid parsed from the text. Memory must be freed by the caller.
 * Argument: grid
 *     OUT: Set of folds parsed from the text. Memory must be freed by the
 *     caller.
 *
 * Return: void
 */
static void
parse_text_into_grid_and_folds(parsed_text_type  parsed_text,
                               grid_type        *grid,
                               folds_type       *folds)
{
    uint16_t i, j;
    uint16_t x, y;
    char     axis;
    uint16_t coordinate;

    /* Find the length and width of the grid and folds first */
    grid->grid = NULL;
    grid->length = 0;
    grid->width = 0;
    folds->folds = NULL;
    folds->num_folds = 0;
    for (i = 0; i < parsed_text.num_lines; i++) {
        if (!IS_EMTPY_STR(parsed_text.lines[i].line)
            && !is_str_in_str(parsed_text.lines[i].line, "fold")) {
            /* Does not contain 'fold', hence is a grid position */
            sscanf(parsed_text.lines[i].line, "%hd,%hd", &x, &y);
            grid->width = (x + 1) > grid->width ? (x + 1) : grid->width;
            grid->length = (y + 1) > grid->length ? (y + 1) : grid->length;
        } else if (!IS_EMTPY_STR(parsed_text.lines[i].line)) {
            /* Contains 'fold' */
            folds->num_folds++;
        }
    }

    /* Allocate the grid. Use calloc to initialise to false */
    allocate_empty_grid(grid);
    folds->folds = malloc_b(folds->num_folds * sizeof(fold_type));

    /* Mark specified grid positions */
    for (i = 0, j = 0; i < parsed_text.num_lines; i++) {
        if (!IS_EMTPY_STR(parsed_text.lines[i].line)
            && !is_str_in_str(parsed_text.lines[i].line, "fold")) {
            /* Does not contain 'fold', hence is a grid position */
            sscanf(parsed_text.lines[i].line, "%hd,%hd", &x, &y);
            grid->grid[y][x] = true;
        } else if (!IS_EMTPY_STR(parsed_text.lines[i].line)) {
            /* Contains 'fold' */
            sscanf(parsed_text.lines[i].line,
                   "fold along %c=%hd", &axis, &coordinate);
            assert(axis == 'x' || axis == 'y');
            folds->folds[j].axis = axis;
            folds->folds[j++].coordinate = coordinate;
        }
    }
}

/*
 * fold_grid
 *
 * Performs a fold on the given grid. A new grid is returned, the memory of
 * which must be freed by the caller.
 *
 * Argument: grid
 *     Grid to fold.
 * Argument: fold
 *     Fold type to perform on the grid.
 *
 * Return: grid_type
 */
static grid_type
fold_grid(grid_type grid, fold_type fold)
{
    grid_type folded_grid;
    uint16_t  i, j;
    uint16_t  post_fold_coordinate;

    /* Find the size of the new grid after folding */
    switch (fold.axis) {
        case 'x':
            /* Folding along x, length remains the same */
            folded_grid.length = grid.length;
            /*
             * New width is the maximum number of columns on either side of
             * the fold.
             */
            folded_grid.width = MAX(fold.coordinate,
                                    grid.width - 1 - fold.coordinate);
            break;
        case 'y':
            /* Folding along y, width remains the same */
            folded_grid.width = grid.width;
            /*
             * New length is the maximum number of rows on either side of
             * the fold.
             */
            folded_grid.length = MAX(fold.coordinate,
                                     grid.length - 1 - fold.coordinate);
            break;
        default:
            assert(false);
    }

    allocate_empty_grid(&folded_grid);

    /*
     * Fill in the new grid after folding.
     */
    for (i = 0; i < grid.length; i++) {
        for (j = 0; j < grid.width; j++) {
            if (!grid.grid[i][j]) {
                /* Not marked, can skip */
                continue;
            }
            switch (fold.axis) {
                case 'x':
                    if (j == fold.coordinate) {
                        /* On the fold axis, check it is false and skip */
                        assert(!grid.grid[i][j]);
                        break;
                    }
                    /*
                     * Folding along x, y coordinate remains the same. Work out
                     * the new x coordinate and mark the point in the new grid.
                     */
                    post_fold_coordinate = folded_grid.width
                                           - (MAX(fold.coordinate, j)
                                              - MIN(fold.coordinate, j));
                    folded_grid.grid[i][post_fold_coordinate] = true;
                    break;
                case 'y':
                    if (i == fold.coordinate) {
                        /* On the fold axis, check it is false and skip */
                        assert(!grid.grid[i][j]);
                        break;
                    }
                    /*
                     * Folding along y, x coordinate remains the same. Work out
                     * the new y coordinate and mark the point in the new grid.
                     */
                    post_fold_coordinate = folded_grid.length
                                           - (MAX(fold.coordinate, i)
                                              - MIN(fold.coordinate, i));
                    folded_grid.grid[post_fold_coordinate][j] = true;
                    break;
                default:
                    assert(false);
            }
        }
    }

    return (folded_grid);
}

/*
 * find_num_marked
 *
 * Find the number of marked points in the grid.
 *
 * Argument: grid
 *     Grid to find number of marked points in.
 *
 * Return: uint16_t
 */
static uint16_t
find_num_marked(grid_type grid)
{
    uint16_t i, j;
    uint16_t num_marked = 0;

    for (i = 0; i < grid.length; i++) {
        for (j = 0; j < grid.width; j++) {
            if (grid.grid[i][j]) {
                num_marked++;
            }
        }
    }

    return (num_marked);
}

/*
 * print_grid
 *
 * Prints the grid to stdout.
 *
 * Argument: grid
 *     Grid to print.
 *
 * Return: void
 */
static void
print_grid(grid_type grid)
{
    uint16_t i, j;

    for (i = 0; i < grid.length; i++) {
        for (j = 0; j < grid.width; j++) {
            if (grid.grid[i][j]) {
                putchar('#');
            } else {
                putchar('.');
            }
        }
        putchar('\n');
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
    grid_type        folded_grid;
    grid_type        tmp;
    folds_type       folds;
    uint16_t         i;

    parsed_text = parse_file(file_name);

    parse_text_into_grid_and_folds(parsed_text, &grid, &folds);

    /* Do first fold and find number of points marked */
    folded_grid = fold_grid(grid, folds.folds[0]);
    if (print_output) {
        printf("Part 1: Number of points marked = %hd\n",
               find_num_marked(folded_grid));
    }

    /* Do rest of folds and print grid to get code */
    for (i = 1; i < folds.num_folds; i++) {
        tmp = fold_grid(folded_grid, folds.folds[i]);
        free_grid_type(folded_grid);
        folded_grid = tmp;
    }
    if (print_output) {
        printf("Part 2:\n");
        print_grid(folded_grid);
    }

    free_grid_type(folded_grid);
    free_grid_type(grid);
    free(folds.folds);
    folds.folds = NULL;

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
