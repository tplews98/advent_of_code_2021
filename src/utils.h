/*
 * Common helper utils
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifndef __UTILS_H__
#define __UTILS_H__

/*
 * parsed_line
 *
 * Element: line
 *     Line of text.
 * Element: num_lines
 *     Number of characters in line.
 */
typedef struct Parsed_Line {
    char   *line;
    size_t  len;
} parsed_line_type;

/*
 * parsed_text
 *
 * Element: lines
 *     Array of parsed lines.
 * Element: num_lines
 *     Number of elements in parsed_line arrray.
 */
typedef struct Parsed_Text {
    parsed_line_type *lines;
    size_t            num_lines;
} parsed_text_type;

/*
 * parse_file
 *
 * Parse text from file into parsed_text_type struct.
 *
 * Argument: file_name
 *     Name of the file to parse.
 *
 * Return: parsed_text_type
 *
 */
parsed_text_type parse_file(char *file_name);

/*
 * free_parse_text
 *
 * Free allocated memory from the parsed_text struct.
 *
 * Argument: parsed_text
 *     parsed_text_type struct to free.
 *
 * Return: void
 *
 */
void free_parse_text(parsed_text_type parsed_text);

/*
 * parse_text_to_ints
 *
 * Parse text into array of integers.
 *
 * Argument: parsed_text
 *     Parsed text struct, where each line is an integer.
 *
 * Return: int *
 *
 */
int *parse_text_to_ints(parsed_text_type parsed_text);

/*
 * print_elapsed_time
 *
 * Print the elapsed time with an appropriate unit.
 *
 * Argument: elapsed_time_ns
 *     Time in nanoseconds.
 * Argument: description
 *     What the time is that was measured, printed before the time.
 *
 * Return: void
 *
 */
void print_elapsed_time(double elapsed_time_ns, char *description);

#endif /* __UTILS_H__ */
