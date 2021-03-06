/*
 * AoC 2021 Day 2 solution
 */

#include "utils.h"

/* Maximum length of a direction string (strlen("forward")+1)*/
#define DIRECTION_STR_MAX_SIZE 8

/* Macros for easily identifying direction strings */
#define IS_FORWARD(str) (STRS_EQUAL(str, "forward"))
#define IS_DOWN(str)    (STRS_EQUAL(str, "down"))
#define IS_UP(str)      (STRS_EQUAL(str, "up"))

/*
 * position_type
 *
 * Element: horizontal
 *     Horizontal position of ship.
 * Element: depth
 *     Depth of ship.
 * Element: aim
 *     Current aim of the ship (unused in part 1).
 */
typedef struct Position {
    int horizontal;
    int depth;
    int aim;
} position_type;

/*
 * direction_enum_type
 *
 * Enum of possible directions the ship can go.
 */
typedef enum Direction {
    FORWARD,
    DOWN,
    UP,
} direction_enum_type;

/*
 * instruction_type
 *
 * Element: direction
 *     Enum value of direction.
 * Element: distance
 *     Distance to travel in direction.
 */
typedef struct Instruction {
    direction_enum_type direction;
    int                 distance;
} instruction_type;

/*
 * initialise_position_type
 *
 * Sets the input position_type struct elements to their initial values.
 *
 * Element: parsed_text
 *     position_type struct to initialise.
 *
 * Returns: void
 *
 */
static void
initialise_position_type(position_type *position)
{
    position->horizontal = 0;
    position->depth = 0;
    position->aim = 0;
}

/*
 * parse_line_bag_colours
 *
 * Parses the colour of the bag in the input line.
 *
 * Argument: direction
 *     Bag struct to parse into.
 * Argument: distance
 *     Line to parse.
 *
 * Return: void
 *
 */
static instruction_type
parse_line_from_instruction(char *line)
{
    instruction_type instruction;
    char             direction[DIRECTION_STR_MAX_SIZE] = "";
    int              distance;

    sscanf(line, "%s %d", direction, &distance);

    if (IS_FORWARD(direction)) {
        instruction.direction = FORWARD;
    } else if (IS_DOWN(direction)) {
        instruction.direction = DOWN;
    } else if (IS_UP(direction)) {
        instruction.direction = UP;
    } else {
        assert(false);
    }

    instruction.distance = distance;

    return (instruction);
}

/*
 * calculate_final_position_part_1
 *
 * Parse instructions from lines and calculate the final position.
 *
 * Argument: parsed_text
 *     Parsed text from input file.
 *
 * Return: position_type
 *
 */
static position_type
calculate_final_position_part_1(parsed_text_type parsed_text)
{
    position_type    position;
    instruction_type instruction;
    int              i;

    initialise_position_type(&position);

    for (i = 0; i < parsed_text.num_lines; i++) {
        instruction = parse_line_from_instruction(parsed_text.lines[i].line);
        if (instruction.direction == FORWARD) {
            position.horizontal += instruction.distance;
        } else if (instruction.direction == DOWN) {
            position.depth += instruction.distance;
        } else if (instruction.direction == UP) {
            position.depth -= instruction.distance;
        } else {
            assert(false);
        }
    }

    return (position);
}

/*
 * calculate_final_position_part_2
 *
 * Parse instructions from lines and calculate the final position.
 *
 * Argument: parsed_text
 *     Parsed text from input file.
 *
 * Return: position_type
 *
 */
static position_type
calculate_final_position_part_2(parsed_text_type parsed_text)
{
    position_type    position;
    instruction_type instruction;
    int              i;

    initialise_position_type(&position);

    for (i = 0; i < parsed_text.num_lines; i++) {
        instruction = parse_line_from_instruction(parsed_text.lines[i].line);
        if (instruction.direction == FORWARD) {
            position.horizontal += instruction.distance;
            position.depth += instruction.distance * position.aim;
        } else if (instruction.direction == DOWN) {
            position.aim += instruction.distance;
        } else if (instruction.direction == UP) {
            position.aim -= instruction.distance;
        } else {
            assert(false);
        }
    }

    return (position);
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
    position_type     final_position;

    parsed_text = parse_file(file_name);

    final_position = calculate_final_position_part_1(parsed_text);
    if (print_output) {
        printf("Part 1: Horizontal = %d, Depth = %d, H*D = %d\n",
               final_position.horizontal,
               final_position.depth,
               final_position.horizontal * final_position.depth);
    }

    final_position = calculate_final_position_part_2(parsed_text);
    if (print_output) {
        printf("Part 2: Horizontal = %d, Depth = %d, H*D = %d\n",
               final_position.horizontal,
               final_position.depth,
               final_position.horizontal * final_position.depth);
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
