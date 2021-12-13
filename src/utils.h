/*
 * Common helper utils
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#ifndef __UTILS_H__
#define __UTILS_H__

/* Number of times to run main function to benchmark */
#define NUM_TIMES_TO_BENCHMARK 100

/* Macros for easy string comparisons */
#define IS_EMTPY_STR(str)      (strncmp(str, "", 2) == 0)
#define STRS_EQUAL(str1, str2) (strncmp(str1, str2, strlen(str1) + 1) == 0)
#define STRS_EQUAL_WITH_SIZE(str1, str2, size) (strncmp(str1, str2, size) == 0)

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
 * find_sum_of_array
 *
 * Find the sum of all numbers in an array.
 *
 * Argument: numbers_array
 *     Array of integers to sum.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: int
 */
int find_sum_of_array(int *numbers_array, size_t len);

/*
 * sort_numbers
 *
 * Sort an array of integers into ascending order.
 *
 * Argument: numbers_array
 *     Array of integers to sort. This is sorted inplace.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: void
 */
void sort_numbers(int *numbers_array, size_t len);

/*
 * sort_long_numbers
 *
 * Sort an array of unsigned long integers into ascending order.
 *
 * Argument: numbers_array
 *     Array of integers to sort. This is sorted inplace.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: void
 */
void sort_long_numbers(size_t *numbers_array, size_t len);

/*
 * find_median_of_sorted_array
 *
 * Find the median value of an array of ascending integers.
 *
 * Argument: numbers_array
 *     Array of sorted integers to find the median of.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: int
 */
int find_median_of_sorted_array(int *numbers_array, size_t len);

/*
 * find_mean_of_array
 *
 * Find the mean of all numbers in an array.
 *
 * Argument: numbers_array
 *     Array of integers to find the mean of.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: double
 */
double find_mean_of_array(int *numbers_array, size_t len);

/*
 * sort_string
 *
 * Sort an string into ascending order (based on ascii value).
 *
 * Argument: string
 *     String to sort. This is sorted inplace.
 * Argument: len
 *     Lenght of string.
 *
 * Return: void
 */
void sort_string(char *string, size_t len);

/*
 * is_str_in_str
 *
 * Check if string_1 contains string_2.
 *
 * Argument: string_1
 *     String to check if it contains the other.
 * Argument: string_2
 *     String to check if it is in the other.
 *
 * Return: bool
 */
bool is_str_in_str(char *string_1, char *string_2);

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

/*
 * run_main_func_with_benchmark
 *
 * Run a day's runner function NUM_TIMES_TO_BENCHMARK times and print the
 * average time for each run.
 *
 * Argument: func
 *     A day's runner function. Must take two parameters: a char* file_name
 *     and a bool which determines if output should be printed. Output should
 *     only be printed on the final run.
 * Argument: file_name
 *     File to read input from.
 *
 * Return: void
 *
 */
void run_main_func_with_benchmark(void (*func)(char *, bool), char *file_name);

#endif /* __UTILS_H__ */
