/*
 * AoC 2021 Day 2 solution
 */

#include "utils.h"

/* Maximum length of a direction string (strlen("forward")+1)*/
#define DIRECTION_STR_MAX_SIZE 8

/* Macros for easily identifying direction strings */
#define IS_FORWARD(str) (strncmp(str, "forward", strlen("forward") + 1) == 0)
#define IS_DOWN(str)    (strncmp(str, "down", strlen("down") + 1) == 0)
#define IS_UP(str)      (strncmp(str, "up", strlen("up") + 1) == 0)

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
 * Main function.
 */
int
main(int argc, char **argv)
{
    struct timespec   start_time, end_time;
    int               rc = 0;
    char             *file_name = NULL;
    parsed_text_type  parsed_text;
    position_type     final_position;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    final_position = calculate_final_position_part_1(parsed_text);
    printf("Part 1: Horizontal = %d, Depth = %d, H*D = %d\n",
           final_position.horizontal,
           final_position.depth,
           final_position.horizontal * final_position.depth);

    final_position = calculate_final_position_part_2(parsed_text);
    printf("Part 2: Horizontal = %d, Depth = %d, H*D = %d\n",
           final_position.horizontal,
           final_position.depth,
           final_position.horizontal * final_position.depth);

    free_parse_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}
