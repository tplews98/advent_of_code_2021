/*
 * AoC 2021 Day 10 solution
 */

#include "utils.h"

/* Macros for identifying open/close brackets */
#define IS_OPEN_BRACKET(char)   (char == '('                                  \
                                 || char == '['                               \
                                 || char == '{'                               \
                                 || char == '<')
#define IS_CLOSED_BRACKET(char) (char == ')'                                  \
                                 || char == ']'                               \
                                 || char == '}'                               \
                                 || char == '>')

/*
 * find_syntax_errors_part_1
 *
 * Split the parsed text into lines with and without syntax errors and
 * calculate and return the syntax error score.
 *
 * Argument: parsed_text
 *     Parsed text struct of lines to check the syntax of.
 * Argument: syntax_error_lines
 *     OUT: Array of lines with syntax errors. The memory of the array of
 *     pointers must be freed by the caller, but no the individual lines as
 *     they point to the same line in the parsed_text struct which should be
 *     freed separately.
 * Argument: num_syntax_error_lines
 *     OUT: Number of elements in the syntax_error_lines array.
 * Argument: non_syntax_error_lines
 *     OUT: Array of lines without syntax errors. The memory of the array of
 *     pointers must be freed by the caller, but no the individual lines as
 *     they point to the same line in the parsed_text struct which should be
 *     freed separately.
 * Argument: num_non_syntax_error_lines
 *    OUT:  Number of elements in the non_syntax_error_lines array.
 *
 * Return: size_t
 */
static size_t
find_syntax_errors_part_1(parsed_text_type    parsed_text,
                          char             ***syntax_error_lines,
                          size_t             *num_syntax_error_lines,
                          char             ***non_syntax_error_lines,
                          size_t             *num_non_syntax_error_lines)
{
    size_t  i, j;
    size_t  max_str_len;
    char    c;
    char   *open_bracket_buffer = NULL;
    size_t  current_buffer_pos;
    bool    is_syntax_error;
    size_t  syntax_error_score;

    /* Find max length of a line and allocate a buffer which can fit it */
    max_str_len = 0;
    for (i = 0; i < parsed_text.num_lines; i++) {
        if (parsed_text.lines[i].len > max_str_len) {
            max_str_len = parsed_text.lines[i].len;
        }
    }
    open_bracket_buffer = malloc_b(max_str_len + 1);

    /*
     * Over-allocate memory for the two syntax and non-syntax error lines as
     * we don't know how many there will be.These will be reallocated later
     * with the correct size after all lines have been added.
     */
    *syntax_error_lines = malloc_b(parsed_text.num_lines * sizeof(char *));
    *non_syntax_error_lines = malloc_b(parsed_text.num_lines * sizeof(char *));
    *num_syntax_error_lines = 0;
    *num_non_syntax_error_lines = 0;

    /*
     * For each line, build up a buffer of open bracket types. When we
     * encounter a close bracket, make sure it is the the same bracket type as
     * the last open bracket on the buffer. If it is, remove that bracket from
     * the buffer and continue, if it is not the same there is a syntax error.
     *
     * Keep track of the syntax error score if there is an error.
     */
    syntax_error_score = 0;
    for (i = 0; i < parsed_text.num_lines; i++) {
        /* Reset the buffer to null characters */
        memset(open_bracket_buffer, '\0', max_str_len + 1);
        current_buffer_pos = 0;
        is_syntax_error = false;

        for (j = 0; j < parsed_text.lines[i].len; j++) {
            c = parsed_text.lines[i].line[j];
            if (IS_OPEN_BRACKET(c)) {
                /* Is an open bracket, put it on the buffer */
                open_bracket_buffer[current_buffer_pos++] = c;
            } else if (IS_CLOSED_BRACKET(c)) {
                /*
                 * Is an closed bracket, check the last char on the buffer was
                 * the equivalent open bracket.
                 */
                if (current_buffer_pos == 0
                    || (c == ')' && open_bracket_buffer[current_buffer_pos-1] != '(')
                    || (c == ']' && open_bracket_buffer[current_buffer_pos-1] != '[')
                    || (c == '}' && open_bracket_buffer[current_buffer_pos-1] != '{')
                    || (c == '>' && open_bracket_buffer[current_buffer_pos-1] != '<')) {
                    /*
                     * Was not the matching open bracket, or there was no
                     * previous open bracket -> syntax error.
                     */
                    (*syntax_error_lines)[(*num_syntax_error_lines)++] =
                                                     parsed_text.lines[i].line;
                    is_syntax_error = true;
                    if (c == ')') {
                        syntax_error_score += 3;
                    } else if (c == ']') {
                        syntax_error_score += 57;
                    } else if (c == '}') {
                        syntax_error_score += 1197;
                    } else if (c == '>') {
                        syntax_error_score += 25137;
                    }
                    break;
                } else {
                    /* Was a matching bracket, pop it from the buffer */
                    open_bracket_buffer[--current_buffer_pos] = '\0';
                }
            } else {
                assert(false);
            }
        }
        if (!is_syntax_error) {
            /*
            * We made it to the end of the line without finding any synatx
            * errors.
            */
            (*non_syntax_error_lines)[(*num_non_syntax_error_lines)++] =
                                                    parsed_text.lines[i].line;
        }
    }

    /* Realloc the arrays to have the right size */
    *syntax_error_lines = realloc_b(*syntax_error_lines,
                                    *num_syntax_error_lines * sizeof(char *));
    *non_syntax_error_lines = realloc_b(
                                 *non_syntax_error_lines,
                                 *num_non_syntax_error_lines * sizeof(char *));

    free(open_bracket_buffer);
    open_bracket_buffer = NULL;

    return (syntax_error_score);
}

/*
 * autocomplete_lines_part_2
 *
 * Work out the autocomplete score for the given lines. The lines must not have
 * any syntax errors.
 *
 * Argument: non_syntax_error_lines
 *     Array of lines without syntax errors to complete.
 * Argument: num_lines
 *     Number of elements in the array.
 *
 * Return: size_t
 */
static size_t
autocomplete_lines_part_2(char   **non_syntax_error_lines,
                          size_t   num_lines)
{
    size_t  i, j;
    size_t  max_str_len;
    size_t  tmp;
    char    c;
    char   *open_bracket_buffer = NULL;
    size_t  current_buffer_pos;
    size_t *autocomplete_scores = NULL;
    size_t  autocomplete_score;

    /* Find max length of a line and allocate a buffer which can fit it */
    max_str_len = 0;
    for (i = 0; i < num_lines; i++) {
        tmp = strlen(non_syntax_error_lines[i]);
        if (tmp > max_str_len) {
            max_str_len = tmp;
        }
    }
    open_bracket_buffer = malloc_b(max_str_len + 1);

    autocomplete_scores = malloc_b(num_lines * sizeof(size_t));

    /*
     * For each line, build up a buffer of open bracket types. When we
     * encounter a close bracket, make sure it is the the same bracket type as
     * the last open bracket on the buffer. If it is, remove that bracket from
     * the buffer and continue. There should be no syntax errors in the given
     * lines.
     *
     * Once a line has been fully parsed, work out the autocomplete score of
     * brackets needed to complete the line and add it to the array of scores.
     */
    for (i = 0; i < num_lines; i++) {
        /* Reset the buffer to null characters */
        memset(open_bracket_buffer, '\0', max_str_len + 1);
        current_buffer_pos = 0;

        for (j = 0; j < strlen(non_syntax_error_lines[i]); j++) {
            c = non_syntax_error_lines[i][j];
            if (IS_OPEN_BRACKET(c)) {
                /* Is an open bracket, put it on the buffer */
                open_bracket_buffer[current_buffer_pos++] = c;
            } else if (IS_CLOSED_BRACKET(c)) {
                /*
                 * Is an closed bracket, check the last char on the buffer was
                 * the equivalent open bracket.
                 */
                if (current_buffer_pos == 0
                    || (c == ')' && open_bracket_buffer[current_buffer_pos-1] != '(')
                    || (c == ']' && open_bracket_buffer[current_buffer_pos-1] != '[')
                    || (c == '}' && open_bracket_buffer[current_buffer_pos-1] != '{')
                    || (c == '>' && open_bracket_buffer[current_buffer_pos-1] != '<')) {
                    /*
                     * Was not the matching open bracket, or there was no
                     * previous open bracket -> syntax error. Should not happen
                     * so assert.
                     */
                    assert(false);
                } else {
                    /* Was a matching bracket, pop it from the buffer */
                    open_bracket_buffer[--current_buffer_pos] = '\0';
                }
            } else {
                assert(false);
            }
        }
        /*
         * Have now got the buffer of remaining open brackets. The order of the
         * matching close brackets is simply the reverse of the buffer.
         */
        autocomplete_score = 0;
        for (; current_buffer_pos >= 1; current_buffer_pos--) {
            c = open_bracket_buffer[current_buffer_pos - 1];
            autocomplete_score *= 5;
            if (c == '(') {
                autocomplete_score += 1;
            } else if (c == '[') {
                autocomplete_score += 2;
            } else if (c == '{') {
                autocomplete_score += 3;
            } else if (c == '<') {
                autocomplete_score += 4;
            }
        }
        autocomplete_scores[i] = autocomplete_score;
    }

    /*
     * The final autocomplete score is the middle value when sorted.
     */
    sort_long_numbers(autocomplete_scores, num_lines);
    autocomplete_score = autocomplete_scores[num_lines / 2];

    free(autocomplete_scores);
    autocomplete_scores = NULL;

    free(open_bracket_buffer);
    open_bracket_buffer = NULL;

    return (autocomplete_score);
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
    parsed_text_type   parsed_text;
    char             **syntax_error_lines = NULL;
    size_t             num_syntax_error_lines;
    char             **non_syntax_error_lines = NULL;
    size_t             num_non_syntax_error_lines;
    size_t             syntax_error_score;
    size_t             autocomplete_score;

    parsed_text = parse_file(file_name);

    syntax_error_score = find_syntax_errors_part_1(
                                                  parsed_text,
                                                  &syntax_error_lines,
                                                  &num_syntax_error_lines,
                                                  &non_syntax_error_lines,
                                                  &num_non_syntax_error_lines);
    if (print_output) {
        printf("Part 1: Syntax error score = %zu\n", syntax_error_score);
    }

    autocomplete_score = autocomplete_lines_part_2(non_syntax_error_lines,
                                                   num_non_syntax_error_lines);
    if (print_output) {
        printf("Part 2: Autocomplete score = %zu\n", autocomplete_score);
    }

    free(syntax_error_lines);
    free(non_syntax_error_lines);
    syntax_error_lines = NULL;
    non_syntax_error_lines = NULL;

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
