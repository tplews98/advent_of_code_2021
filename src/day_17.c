/*
 * AoC 2021 Day 17 solution
 */

#include "utils.h"

/*
 * target_area_type
 *
 * Element: x_start
 *     x coordinate of the start of the target area.
 * Element: type_id
 *     x coordinate of the end of the target area.
 * Element: length_type_id
 *     y coordinate of the start of the target area.
 * Element: length_value
 *     y coordinate of the end of the target area.
 */
typedef struct Target_Area {
    int x_start;
    int x_end;
    int y_start;
    int y_end;
} target_area_type;

/*
 * parse_line_into_target_area
 *
 * Parse a day's input line into a target area struct.
 *
 * Argument: line
 *     Day's input line containing target area info.
 *
 * Return: target_area_type
 */
static target_area_type
parse_line_into_target_area(char *line)
{
    target_area_type target_area;

    sscanf(line,
           "target area: x=%d..%d, y=%d..%d",
           &target_area.x_start,
           &target_area.x_end,
           &target_area.y_start,
           &target_area.y_end);

    return (target_area);
}

/*
 * sum_of_ints_up_to_n
 *
 * Find the sum of all integers up to and including n.
 *
 * Argument: n
 *     Integer to sum up to.
 *
 * Return: int
 */
static int
sum_of_ints_up_to_n(int n)
{
    return ((n * (n + 1)) / 2);
}

/*
 * does_hit_in_target_area
 *
 * Check whether the probe will hit the target area if launched with the given
 * initial velocities.
 *
 * Argument: target_area
 *     Target area struct defining the boundaries of the target area to hit.
 * Argument: initial_x_vel
 *     Initial x velocity the probe is launched with.
 * Argument: initial_y_vel
 *     Initial y velocity the probe is launched with.
 *
 * Return: bool
 */
static bool
does_hit_in_target_area(target_area_type target_area,
                        int              initial_x_vel,
                        int              initial_y_vel)
{
    int  x_pos, y_pos;
    int  x_vel, y_vel;
    bool hits_area;

    if ((initial_x_vel < 0 && target_area.x_start > 0)
        || (initial_x_vel > 0 && target_area.x_end < 0)
        || (initial_y_vel < 0 && target_area.y_start > 0)) {
        /*
         * Going in wrong direction initially, will never hit target area.
         */
        return (false);
    }

    /*
     * Iterate steps until the probe is in the target area or goes beyond it
     * such that it will never enter.
     */
    x_pos = 0;
    y_pos = 0;
    x_vel = initial_x_vel;
    y_vel = initial_y_vel;
    hits_area = false;
    while (abs(x_pos) <= MAX(abs(target_area.x_start), abs(target_area.x_end))
           && (y_vel >= 0 || y_pos >= target_area.y_start)) {
        if (x_pos >= target_area.x_start && x_pos <= target_area.x_end
            && y_pos >= target_area.y_start && y_pos <= target_area.y_end) {
            /* Currently in the target area */
            hits_area = true;
            break;
        }
        /* Perform a step */
        x_pos += x_vel;
        y_pos += y_vel;
        x_vel = x_vel > 0 ? (x_vel - 1) : 0;
        y_vel--;
    }

    return (hits_area);
}

/*
 * find_greatest_height_of_valid_velocities
 *
 * Find the greatest height the probe could be launched to while still hitting
 * the target area. Also set the total number of valid velocities.
 *
 * Argument: target_area
 *     Target area struct defining the boundaries of the target area to hit.
 * Argument: num_valid_velocities
 *     OUT: The total number of valid velocity pairs which will hit the target
 *     area.
 *
 * Return: int
 */
static int
find_greatest_height_of_valid_velocities(
                                        target_area_type target_area,
                                        int              *num_valid_velocities)
{
    int min_initial_x, max_initial_x;
    int min_initial_y, max_initial_y;
    int highest_y_value, y_max_height;
    int x_vel, y_vel;

    /*
     * Find the minimum initial x velocity.
     * If the start of the target area is < 0, the minimum velocity is negative
     * and such that for any more negative initial velocity the probe will
     * overshoot after one step.
     * If the start of the target area is > 0, the minimum velocity is positive
     * and such that the probe can reach the start of the area before its
     * velocity drops to 0.
     */
    if (target_area.x_start < 0) {
        min_initial_x = target_area.x_start;
    } else {
        min_initial_x = 0;
        while (target_area.x_start > sum_of_ints_up_to_n(min_initial_x)) {
            min_initial_x++;
        }
    }

    /*
     * Find the maximum initial x velocity.
     * If the end of the target area is < 0, the maximum velocity is negative
     * and such that the probe can reach the start of the area before its
     * velocity drops to 0.
     * If the end of the target area is > 0, the maximum velocity is positive
     * and such that for any greater initial velocity the probe will overshoot
     * after one step.
     */
    if (target_area.x_end < 0) {
        max_initial_x = 0;
        while (target_area.x_start > sum_of_ints_up_to_n(max_initial_x)) {
            max_initial_x++;
        }
    } else {
        max_initial_x = target_area.x_end;
    }

    /*
     * Find the minimum initial y velocity.
     * If the start of the target area is < 0, the minimum velocity is negative
     * and such that for any more negative initial velocity the probe will
     * overshoot after one step.
     * If the start of the target area is > 0, the minimum velocity is positive
     * and such that the probe can reach the start of the area before its
     * velocity drops to 0.
     */
    if (target_area.y_start < 0) {
        min_initial_y = target_area.y_start;
    } else {
        min_initial_y = 0;
        while (target_area.y_start > sum_of_ints_up_to_n(min_initial_y)) {
            min_initial_y++;
        }
    }

    /*
     * Find the minimum initial y velocity.
     * If the end of the target area is < 0, the maximum velocity is distance
     * from the start of the area - 1 from y=0, as this is the distance
     * travelled in one step after y=0 when the probe is travelling back down.
     * If the start of the target area is > 0, the maximum velocity is distance
     * from the end of the area from y=0, as this is the distance travelled
     * in one step before y=0 when the probe is travelling back down.
     */
    if (target_area.y_start <= 0 && target_area.y_end >= 0) {
        /* Any velocity is possible. This is probably an error so assert */
        assert(false);
    } else if(target_area.y_end < 0) {
        max_initial_y = abs(target_area.y_start + 1);
    } else {
        max_initial_y = target_area.y_end;
    }

    /*
     * Loop over all possible velocity combinations between the maximums and
     * minimums and record the maximum height achieved and number of valid
     * velocities.
     */
    highest_y_value = target_area.y_start;
    *num_valid_velocities = 0;
    for (x_vel = min_initial_x; x_vel <= max_initial_x; x_vel++) {
        for (y_vel = min_initial_y; y_vel <= max_initial_y; y_vel++) {
            if (does_hit_in_target_area(target_area, x_vel, y_vel)) {
                /* These velocities do hit the target area */
                (*num_valid_velocities)++;
                y_max_height = y_vel > 0 ? sum_of_ints_up_to_n(y_vel) : 0;
                if (y_max_height > highest_y_value) {
                    highest_y_value = y_max_height;
                }
            }
        }
    }

    return (highest_y_value);
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
    target_area_type target_area;
    int              highest_y_value;
    int              num_valid_velocities;

    parsed_text = parse_file(file_name);

    target_area = parse_line_into_target_area(parsed_text.lines[0].line);

    highest_y_value = find_greatest_height_of_valid_velocities(
                                           target_area, &num_valid_velocities);

    if (print_output) {
        printf("Part 1: Highest y value for valid velocities = %d\n",
               highest_y_value);
        printf("Part 2: Number of valid velocities = %d\n",
               num_valid_velocities);
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
