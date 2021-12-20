/*
 * AoC 2021 Day 12 solution
 */

#include "utils.h"

/* Max cave name length (strlen("start") + 1) */
#define MAX_CAVE_LEN 6

/* Macros for easily identifying caves from names */
#define IS_START(cave)      (STRS_EQUAL_WITH_SIZE(                            \
                                             cave.name, "start", MAX_CAVE_LEN))
#define IS_END(cave)        (STRS_EQUAL_WITH_SIZE(                            \
                                             cave.name, "end", MAX_CAVE_LEN))
#define IS_SMALL_CAVE(cave) (cave.name[0] >= 'a'                              \
                             && cave.name[0] <= 'z'                           \
                             && !IS_START(cave)                               \
                             && !IS_END(cave))
#define IS_BIG_CAVE(cave)   (cave.name[0] >= 'A' && cave.name[0] <= 'Z')

/*
 * cave_type
 *
 * Element: name
 *     Name of the cave.
 * Element: connected_caves
 *     Array of pointers to other cave structs which this cave is connected to.
 * Element: num_connected_caves
 *     Length of the connected_caves array.
 */
typedef struct Cave {
    char          name[MAX_CAVE_LEN];
    struct Cave **connected_caves;
    size_t        num_connected_caves;
} cave_type;

/*
 * find_cave_in_array
 *
 * Find the cave with the given name in the caves array. If a matching cave is
 * not found NULL is returned.
 *
 * Element: caves
 *     Array of caves to search.
 * Element: num_caves
 *     Number of caves in array.
 * Element: cave_name
 *     Name of cave to find.
 *
 * Returns: cave_type *
 */
static cave_type *
find_cave_in_array(cave_type *caves, size_t num_caves, char *cave_name)
{
    cave_type *cave = NULL;
    size_t     i;

    for (i = 0; i < num_caves; i++) {
        if (STRS_EQUAL_WITH_SIZE(cave_name, caves[i].name, MAX_CAVE_LEN)) {
            /* Found the cave with the same name */
            cave = &(caves[i]);
            break;
        }
    }

    return (cave);
}

/*
 * parse_text_into_caves
 *
 * Parse text of day's input into an array of caves.
 *
 * Element: parsed_text
 *     Parsed text struct of day's input.
 * Element: caves
 *     OUT: Array of caves parsed from text.
 * Element: num_caves
 *     OUT: Number of caves in array.
 *
 * Returns: void
 */
static void
parse_text_into_caves(parsed_text_type   parsed_text,
                      cave_type        **caves,
                      size_t            *num_caves)
{
    size_t            i;
    cave_type        *cave_1 = NULL;
    cave_type        *cave_2 = NULL;
    parsed_text_type  split_text;

    /*
     * Allocate an array which could take the maximum possible number of caves
     */
    *caves = malloc_b(parsed_text.num_lines * sizeof(cave_type));

    /* Add the start cave as the first element */
    *num_caves = 0;
    strncpy((*caves)[*num_caves].name, "start", strlen("start") + 1);
    (*caves)[*num_caves].connected_caves = NULL;
    (*caves)[*num_caves].num_connected_caves = 0;
    (*num_caves)++;

    /*
     * Parse a line. If a cave in the line has not already been created then
     * add it to the array. Add links between the two caves.
     */
    for (i = 0; i < parsed_text.num_lines; i++) {
        split_text = split_string_on_char(parsed_text.lines[i].line, '-');
        cave_1 = find_cave_in_array(*caves,
                                    *num_caves,
                                    split_text.lines[0].line);
        if (cave_1 == NULL) {
            /* Cave has not yet been added to array */
            cave_1 = &((*caves)[(*num_caves)++]);
            strncpy(cave_1->name,
                    split_text.lines[0].line,
                    split_text.lines[0].len + 1);
            cave_1->connected_caves = NULL;
            cave_1->num_connected_caves = 0;
        }
        cave_2 = find_cave_in_array(*caves,
                                    *num_caves,
                                    split_text.lines[1].line);
        if (cave_2 == NULL) {
            /* Cave has not yet been added to array */
            cave_2 = &((*caves)[(*num_caves)++]);
            strncpy(cave_2->name,
                    split_text.lines[1].line,
                    split_text.lines[1].len + 1);
            cave_2->connected_caves = NULL;
            cave_2->num_connected_caves = 0;
        }
        /* Link caves */
        cave_1->num_connected_caves++;
        cave_1->connected_caves = realloc_b(
                            cave_1->connected_caves,
                            cave_1->num_connected_caves * sizeof(cave_type *));
        cave_1->connected_caves[cave_1->num_connected_caves - 1] = cave_2;
        cave_2->num_connected_caves++;
        cave_2->connected_caves = realloc_b(
                            cave_2->connected_caves,
                            cave_2->num_connected_caves * sizeof(cave_type *));
        cave_2->connected_caves[cave_2->num_connected_caves - 1] = cave_1;

        free_parsed_text(split_text);
    }
}

/*
 * find_num_visits
 *
 * Find the number of visits to the given cave in the current path.
 *
 * Element: cave
 *     Cave to check the number of visits to.
 * Element: current_path
 *     Array of the current path to check in.
 * Element: path_len
 *     Length of the current_path array.
 *
 * Returns: size_t
 */
static size_t
find_num_visits(cave_type  cave,
                cave_type *current_path,
                size_t     path_len)
{
    size_t i;
    size_t num_visits = 0;

    for (i = 0; i < path_len; i++) {
        if (STRS_EQUAL_WITH_SIZE(cave.name,
                                 current_path[i].name,
                                 MAX_CAVE_LEN)) {
            num_visits++;
        }
    }

    return (num_visits);
}

/*
 * find_number_of_routes
 *
 * Find the number of unique paths through the cave system from the given start
 * cave to the end cave. To find the total number of routes from the start cave
 * to the end cave, call this function with the start cave and NULL current
 * path.
 *
 * Element: start_cave
 *     Cave to start from (will be the next move in the current path).
 * Element: current_path
 *     Array of the current path through the cave system.
 * Element: path_len
 *     Length of the current_path array.
 * Element: allowed_small_visits
 *     Number of times we are allowed to visit each small cave. For part 2 this
 *     should be 2 initially, then 1 after a single small cave is visited
 *     twice.
 *
 * Returns: size_t
 */
static size_t
find_number_of_routes(cave_type  start_cave,
                      cave_type *current_path,
                      size_t     path_len,
                      size_t     allowed_small_visits)
{
    size_t     i;
    size_t     num_paths = 0;
    cave_type *current_path_copy = NULL;
    size_t     num_visits;

    if (current_path == NULL) {
        path_len = 0;
        current_path_copy = malloc_b(2 * sizeof(cave_type));
        current_path_copy[path_len++] = start_cave;
    } else {
        current_path_copy = malloc_b((path_len + 1) * sizeof(cave_type));
        memcpy(current_path_copy, current_path, path_len * sizeof(cave_type));
    }

    for (i = 0; i < start_cave.num_connected_caves; i++) {
        num_visits = find_num_visits(*(start_cave.connected_caves[i]),
                                     current_path_copy,
                                     path_len);
        if (IS_END((*(start_cave.connected_caves[i])))) {
            num_paths += 1;
        } else if ((IS_SMALL_CAVE((*(start_cave.connected_caves[i])))
                    && num_visits < allowed_small_visits)
                    || IS_BIG_CAVE((*(start_cave.connected_caves[i])))) {
            current_path_copy[path_len] = *(start_cave.connected_caves[i]);
            num_visits++;
            if (num_visits > 1
                && IS_SMALL_CAVE(current_path_copy[path_len])) {
                /*
                 * Have visited a small cave more than once, no others can be
                 * visited more than once now.
                 */
                num_paths += find_number_of_routes(
                    current_path_copy[path_len],
                    current_path_copy,
                    path_len + 1,
                    1);
            } else {
                num_paths += find_number_of_routes(
                    current_path_copy[path_len],
                    current_path_copy,
                    path_len + 1,
                    allowed_small_visits);
            }
        }
    }

    free(current_path_copy);
    current_path_copy = NULL;

    return (num_paths);
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
    cave_type        *caves = NULL;
    size_t            num_caves;
    size_t            num_routes;
    size_t            i;

    parsed_text = parse_file(file_name);

    parse_text_into_caves(parsed_text, &caves, &num_caves);

    num_routes = find_number_of_routes(caves[0], NULL, 0, 1);
    if (print_output) {
        printf("Part 1: Number of routes = %zu\n", num_routes);
    }

    num_routes = find_number_of_routes(caves[0], NULL, 0, 2);
    if (print_output) {
        printf("Part 2: Number of routes = %zu\n", num_routes);
    }

    if (caves != NULL) {
        for (i = 0; i < num_caves; i++) {
            free(caves[i].connected_caves);
            caves[i].connected_caves = NULL;
        }
        free(caves);
        caves = NULL;
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

    run_main_func_with_benchmark(runner, file_name,
                                 NUM_TIMES_TO_BENCHMARK_SMALL);

    return (0);
}
