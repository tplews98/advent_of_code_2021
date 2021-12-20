/*
 * AoC 2021 Day 9 solution
 */

#include "utils.h"

/*
 * height_type
 *
 * Element: height
 *     Height of the area.
 * Element: is_low_point
 *     Whether this area is a low point or not (has no adjacent areas which
 *     are higher).
 * Element: basin_low_point
 *     Pointer to the low point of the basin this area is in. Will be NULL for
 *     heights of 9 as they are not in any basins.
 * Element: basin_size
 *     Size of the basin. This is only set for low-points, all other points
 *     will be 0.
 */
typedef struct Height {
    uint8_t        height;
    bool           is_low_point;
    struct Height *basin_low_point;
    size_t         basin_size;
} height_type;

/*
 * height_map_type
 *
 * Element: height_map
 *     2D array of the entire area, where each element is a height_type.
 * Element: length
 *     How long the area is (number of elements in first array).
 * Element: width
 *     How wide the area is (number of elements in second array).
 */
typedef struct Height_Map {
    height_type **height_map;
    size_t        length;
    size_t        width;
} height_map_type;

/*
 * free_height_map
 *
 * Free allocated memory from the height_map struct.
 *
 * Argument: height_map
 *     height_map struct to free.
 *
 * Return: void
 *
 */
static void
free_height_map(height_map_type height_map)
{
    size_t i;

    if (height_map.height_map != NULL) {
        for (i = 0; i < height_map.length; i++) {
            free(height_map.height_map[i]);
            height_map.height_map[i] = NULL;
        }
        free(height_map.height_map);
        height_map.height_map = NULL;
        height_map.length = 0;
        height_map.width = 0;
    }
}

/*
 * parse_text_into_height_map
 *
 * Parse lines of notes into an array of note_type structs.
 *
 * Argument: parsed_text
 *     Parsed text struct from the day's input.
 *
 * Return: height_map_type
 */
static height_map_type
parse_text_into_height_map(parsed_text_type parsed_text)
{
    size_t          i, j;
    height_map_type height_map;

    height_map.width = strlen(parsed_text.lines[0].line);
    height_map.length = parsed_text.num_lines;
    height_map.height_map = malloc_b(height_map.length
                                     * sizeof(height_type *));

    for (i = 0; i < parsed_text.num_lines; i++) {
        height_map.height_map[i] = calloc_b(height_map.width,
                                            sizeof(height_type));
        for (j = 0; j < height_map.width; j++) {
            height_map.height_map[i][j].height =
                                (uint8_t) (parsed_text.lines[i].line[j] - '0');
        }
    }

    return (height_map);
}


/*
 * identify_low_points
 *
 * Identify low points in the height map.
 *
 * Argument: height_map
 *     Height map to identify low points of.
 *
 * Return: void
 */
static void
identify_low_points(height_map_type height_map)
{
    size_t i, j;
    bool   is_low_point;

    for (i = 0; i < height_map.length; i++) {
        for (j = 0; j < height_map.width; j++) {
            /*
             * Check each adjacent square in turn to see if it is lower than
             * the current area. If any are lower then this is not a low point.
             */
            is_low_point = true;
            if (i != 0) {
                /* Check above */
                is_low_point = (height_map.height_map[i][j].height
                                < height_map.height_map[i-1][j].height);
            }
            if (is_low_point && i != (height_map.length - 1)) {
                /* Check below */
                is_low_point = (height_map.height_map[i][j].height
                                < height_map.height_map[i+1][j].height);
            }
            if (is_low_point && j != 0) {
                /* Check left */
                is_low_point = (height_map.height_map[i][j].height
                                < height_map.height_map[i][j-1].height);
            }
            if (is_low_point && j != (height_map.width - 1)) {
                /* Check right */
                is_low_point = (height_map.height_map[i][j].height
                                < height_map.height_map[i][j+1].height);
            }
            height_map.height_map[i][j].is_low_point = is_low_point;
        }
    }
}

/*
 * find_sum_of_risk_levels_of_low_points
 *
 * Find the sum of the risk levels of all the low points in the height map. The
 * risk level of a point is the height+1.
 *
 * Argument: height_map
 *     Height map. Low points already be identified.
 *
 * Return: void
 */
static size_t
find_sum_of_risk_levels_of_low_points(height_map_type height_map)
{
    size_t i, j;
    size_t total_risk_level = 0;

    for (i = 0; i < height_map.length; i++) {
        for (j = 0; j < height_map.width; j++) {
            if (height_map.height_map[i][j].is_low_point) {
                total_risk_level += height_map.height_map[i][j].height + 1;
            }
        }
    }

    return (total_risk_level);
}

/*
 * find_basin_low_point_area_is_in
 *
 * For a given area (at height_map.height_map[y][x]), find the low point of the
 * basin the area is in.
 *
 * Argument: height_map
 *     Height map the area is in.
 * Argument: area_x_pos
 *     x position of the area.
 * Argument: area_y_pos
 *     y position of the area.
 *
 * Return: height_type *
 */
static height_type *
find_basin_low_point_area_is_in(height_map_type height_map,
                                size_t          area_x_pos,
                                size_t          area_y_pos)
{
    height_type  area;
    uint8_t      lowest_adjacent_height;
    size_t       i, j;
    size_t       i_lowest, j_lowest;

    area = height_map.height_map[area_y_pos][area_x_pos];
    if (area.height == 9) {
        /* Areas with height 9 are not in basins */
        return (NULL);
    }

    /*
     * Start at the given area and check adjacent areas for the lowest. Then
     * start from this new lowest and continue until we find a low point.
     */
    i = area_y_pos;
    j = area_x_pos;
    while (!height_map.height_map[i][j].is_low_point) {
        /*
         * Find the lowest adjacent area.
         */
        lowest_adjacent_height = 9;
        /* Check above */
        if (i != 0
            && height_map.height_map[i-1][j].height < lowest_adjacent_height) {
            /* Above is lower than current lowest */
            i_lowest = i - 1;
            j_lowest = j;
            lowest_adjacent_height =
                              height_map.height_map[i_lowest][j_lowest].height;
        }
        /* Check below */
        if (i != (height_map.length - 1)
            && height_map.height_map[i+1][j].height < lowest_adjacent_height) {
            /* Below is lower than current lowest */
            i_lowest = i + 1;
            j_lowest = j;
            lowest_adjacent_height =
                              height_map.height_map[i_lowest][j_lowest].height;
        }
        /* Check left */
        if (j != 0
            && height_map.height_map[i][j-1].height < lowest_adjacent_height) {
            /* Left is lower than current lowest */
            i_lowest = i;
            j_lowest = j - 1;
            lowest_adjacent_height =
                              height_map.height_map[i_lowest][j_lowest].height;
        }
        /* Check right */
        if (j != (height_map.width - 1)
            && height_map.height_map[i][j+1].height < lowest_adjacent_height) {
            /* Right is lower than current lowest */
            i_lowest = i;
            j_lowest = j + 1;
            lowest_adjacent_height =
                              height_map.height_map[i_lowest][j_lowest].height;
        }
        /* Set the next area to try */
        i = i_lowest;
        j = j_lowest;
    }

    return (&(height_map.height_map[i][j]));
}

/*
 * identify_basins
 *
 * Identify the basin each area is in and set the size of each basin for each
 * low-point.
 *
 * Argument: height_map
 *     Height map to identify basins of.
 *
 * Return: void
 */
static void
identify_basins(height_map_type height_map)
{
    size_t       i, j;

    for (i = 0; i < height_map.length; i++) {
        for (j = 0; j < height_map.width; j++) {
            height_map.height_map[i][j].basin_low_point =
                             find_basin_low_point_area_is_in(height_map, j, i);
            if (height_map.height_map[i][j].basin_low_point != NULL) {
                height_map.height_map[i][j].basin_low_point->basin_size++;
            }
        }
    }
}

/*
 * find_3_largest_basin_sizes_multiplied
 *
 * Multiply the 3 largest basin sizes together for part 2.
 *
 * Argument: height_map
 *     Height map. The sizes of each basin must already be identified.
 *
 * Return: size_t
 */
static size_t
find_3_largest_basin_sizes_multiplied(height_map_type height_map)
{
    size_t  i, j, k;
    size_t  num_basins;
    int    *basin_sizes = NULL;
    size_t  largest_basins_multipled;

    /*
     * First find the number of basins, then add the basin sizes to an array
     * of integers, sort it and return the 3 largest sizes multiplied together.
     */

    num_basins = 0;
    for (i = 0; i < height_map.length; i++) {
        for (j = 0; j < height_map.width; j++) {
            if (height_map.height_map[i][j].is_low_point) {
                num_basins++;
            }
        }
    }

    basin_sizes = malloc_b(num_basins * sizeof(int));
    k = 0;
    for (i = 0; i < height_map.length; i++) {
        for (j = 0; j < height_map.width; j++) {
            if (height_map.height_map[i][j].is_low_point) {
                basin_sizes[k++] = height_map.height_map[i][j].basin_size;
            }
        }
    }

    sort_numbers(basin_sizes, num_basins);

    largest_basins_multipled = (basin_sizes[num_basins - 1]
                                * basin_sizes[num_basins - 2]
                                * basin_sizes[num_basins - 3]);

    free(basin_sizes);

    return (largest_basins_multipled);
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
    height_map_type  height_map;
    size_t           total_risk_level;
    size_t           largest_basins_multipled;


    parsed_text = parse_file(file_name);

    height_map = parse_text_into_height_map(parsed_text);

    identify_low_points(height_map);

    total_risk_level = find_sum_of_risk_levels_of_low_points(height_map);
    if (print_output) {
        printf("Part 1: Total risk level of low points = %zu\n",
               total_risk_level);
    }

    identify_basins(height_map);
    largest_basins_multipled = find_3_largest_basin_sizes_multiplied(
                                                                   height_map);
    if (print_output) {
        printf("Part 2: 3 largest basin sizes multipled = %zu\n",
               largest_basins_multipled);
    }

    free_height_map(height_map);
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
