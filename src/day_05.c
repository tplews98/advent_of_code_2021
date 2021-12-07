/*
 * AoC 2021 Day 5 solution
 */

#include <stdint.h>

#include "utils.h"

/*
 * grid_type
 *
 * Element: columns
 *     Array of columns. Each line is an array of integers.
 * Element: max_x
 *     The maximum x coordinate of the grid. Corresponds to the number of
 *     elements in the columns array. max_x is the furthest-right element in
 *     the grid.
 * Element: max_y
 *     The maximum y coordinate of the grid. Corresponds to the number of
 *     elements in each line in the columns array. max_y is the furthest-down
 *     element in the grid.
 */
typedef struct Grid {
    uint16_t **columns;
    uint16_t   max_x;
    uint16_t   max_y;
} grid_type;

/*
 * line_type
 *
 * Element: x_start
 *     x coordinate of the start of the line.
 * Element: y_start
 *     y coordinate of the start of the line.
 * Element: x_end
 *     x coordinate of the end of the line.
 * Element: y_end
 *     y coordinate of the end of the line.
 */
typedef struct Line {
    uint16_t x_start;
    uint16_t y_start;
    uint16_t x_end;
    uint16_t y_end;
} line_type;

/*
 * parse_line_entries
 *
 * Parse text into a line type. The text should be in the form:
 *   "x_start,y_start -> x_end,y_end"
 * e.g. "1,2 -> 5,6".
 *
 * Argument: text
 *     String to parse.
 *
 * Return: line_type
 */
static line_type
parse_text_into_line_type(char *text)
{
    line_type parsed_line;

    sscanf(text,
           "%hu,%hu -> %hu,%hu",
           &parsed_line.x_start,
           &parsed_line.y_start,
           &parsed_line.x_end,
           &parsed_line.y_end);

    return (parsed_line);
}

/*
 * parse_line_entries
 *
 * Parse text into an array of line_types. Each line in the text is of the form
 *   "x_start,y_start -> x_end,y_end"
 * e.g. "1,2 -> 5,6".
 *
 * Argument: parsed_text
 *     Parsed text struct.
 * Argument: grid
 *     OUT: Struct of the grid. This is created once the maximum x and y
 *     coordinates from the parsed lines are known.
 * Argument: lines
 *     OUT: Array of line structs.
 *
 * Return: void
 */
static void
parse_lines_and_make_grid(parsed_text_type   parsed_text,
                          grid_type         *grid,
                          line_type        **lines)
{
    uint16_t i;

    /* Set initial sensible values */
    grid->columns = NULL;
    grid->max_x = 0;
    grid->max_y = 0;
    *lines = NULL;

    *lines = malloc_f(parsed_text.num_lines * sizeof(line_type));

    for (i = 0; i < parsed_text.num_lines; i++) {
        (*lines)[i] = parse_text_into_line_type(parsed_text.lines[i].line);
        if ((*lines)[i].x_start > grid->max_x) {
            grid->max_x = (*lines)[i].x_start;
        } else if ((*lines)[i].x_end > grid->max_x) {
            grid->max_x = (*lines)[i].x_end;
        }
        if ((*lines)[i].y_start > grid->max_y) {
            grid->max_y = (*lines)[i].y_start;
        } else if ((*lines)[i].y_end > grid->max_y) {
            grid->max_y = (*lines)[i].y_end;
        }
    }

    grid->columns = malloc_f((grid->max_x + 1) * sizeof(uint16_t *));

    for (i = 0; i <= grid->max_x; i++) {
        grid->columns[i] = calloc_f(grid->max_y + 1, sizeof(uint16_t));
    }
}

/*
 * fill_in_grid_with_non_diagonal_lines
 *
 * Add the non-diagonal lines only to the grid.
 *
 * Argument: grid
 *     Emtpy grid to fill.
 * Argument: lines
 *     Array of line structs to fill the grid with. Any diagonal lines are
 *     skipped.
 * Argument: num_lines
 *     Number of lines in the array.
 *
 * Return: void
 */
static void
fill_in_grid_with_non_diagonal_lines(grid_type  grid,
                                     line_type *lines,
                                     size_t     num_lines)
{
    size_t   i, j;
    uint16_t start, end;

    for (i = 0; i < num_lines; i++) {
        if (lines[i].x_start == lines[i].x_end) {
            // Vertical line.
            start = lines[i].y_start < lines[i].y_end ? lines[i].y_start : lines[i].y_end;
            end = lines[i].y_end > lines[i].y_start ? lines[i].y_end : lines[i].y_start;
            for (j = start; j <= end; j++) {
                grid.columns[lines[i].x_start][j]++;
            }
        } else if (lines[i].y_start == lines[i].y_end) {
            // Horizontal line.
            start = lines[i].x_start < lines[i].x_end ? lines[i].x_start : lines[i].x_end;
            end = lines[i].x_end > lines[i].x_start ? lines[i].x_end : lines[i].x_start;
            for (j = start; j <= end; j++) {
                grid.columns[j][lines[i].y_start]++;
            }
        } else {
            // A diagonal line, skip.
            continue;
        }
    }
}

/*
 * find_number_of_intersecting_lines
 *
 * Finds the number of grid positions where at least two lines overlap.
 *
 * Argument: grid
 *     Grid with lines added.
 *
 * Return: size_t
 */
static size_t
find_number_of_intersecting_lines(grid_type grid)
{
    size_t   num_intersecting = 0;
    uint16_t x, y;

    for (x = 0; x <= grid.max_x; x++) {
        for (y = 0; y <= grid.max_y; y++) {
            if (grid.columns[x][y] > 1) {
                num_intersecting++;
            }
        }
    }

    return (num_intersecting);
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
    grid_type         grid;
    line_type        *lines = NULL;
    size_t            num_intersecting;
    uint16_t          i;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    parse_lines_and_make_grid(parsed_text, &grid, &lines);

    fill_in_grid_with_non_diagonal_lines(grid, lines, parsed_text.num_lines);

    num_intersecting = find_number_of_intersecting_lines(grid);
    printf("Part 1: Number of intersecting lines = %zu\n", num_intersecting);

    if (grid.columns != NULL) {
        for (i = 0; i <= grid.max_x; i++) {
            free(grid.columns[i]);
            grid.columns[i] = NULL;
        }
        free(grid.columns);
        grid.columns = NULL;
    }
    free(lines);
    lines = NULL;

    free_parsed_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}