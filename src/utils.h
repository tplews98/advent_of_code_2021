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

/* Number of times to run main function to benchmark */
#define NUM_TIMES_TO_BENCHMARK 500

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
 * malloc_b
 *
 * Wrapper around malloc() which asserts the returned pointer is not NULL;
 */
void *malloc_b(size_t size);

/*
 * calloc_b
 *
 * Wrapper around calloc() which asserts the returned pointer is not NULL;
 */
void *calloc_b(size_t nmemb, size_t size);

/*
 * realloc_b
 *
 * Wrapper around realloc() which asserts the returned pointer is not NULL;
 */
void *realloc_b(void *ptr, size_t size);

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
 * free_parsed_text
 *
 * Free allocated memory from the parsed_text struct.
 *
 * Argument: parsed_text
 *     parsed_text_type struct to free.
 *
 * Return: void
 *
 */
void free_parsed_text(parsed_text_type parsed_text);

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
 * parse_binary_num_text_to_ints
 *
 * Parse text of binary numbers into array of integers.
 *
 * Argument: parsed_text
 *     Parsed text struct, where each line is an binary number.
 *
 * Return: int *
 *
 */
int *parse_binary_num_text_to_ints(parsed_text_type parsed_text);

/*
 * split_string_on_char
 *
 * Split given text on a character. If there are consecutive characters which
 * are all the split_on character, they are ignored (only split once and rest
 * do not caused another split but are skipped over).
 *
 * Argument: text
 *     Text to split on.
 * Argument: split_on
 *     Character to split on.
 *
 * Return: parsed_text_type
 *
 */
parsed_text_type split_string_on_char(char *text, char split_on);

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
