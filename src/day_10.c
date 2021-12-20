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
 * find_syntax_error_and_autocomplete_scores
 *
 * Find the syntax error score for the lines in the parsed text with syntax
 * errors and the autocomplete scores for the lines without syntax errors.
 *
 * Argument: parsed_text
 *     Parsed text struct of lines to check the syntax of.
 * Argument: syntax_error_score
 *     OUT: Syntax error score calculated from the lines with syntax errors.
 * Argument: autocomplete_score
 *     OUT: Autocomplete score calculated from the lines without syntax errors.
 *
 * Return: void
 */
static void
find_syntax_error_and_autocomplete_scores(parsed_text_type  parsed_text,
                                          size_t           *syntax_error_score,
                                          size_t           *autocomplete_score)
{
    size_t  i, j;
    size_t  max_str_len;
    char    c;
    char   *open_bracket_buffer = NULL;
    size_t  current_buffer_pos;
    bool    is_syntax_error;
    size_t  num_non_syntax_error_lines;
    size_t *autocomplete_scores = NULL;

    /* Find max length of a line and allocate a buffer which can fit it */
    max_str_len = 0;
    for (i = 0; i < parsed_text.num_lines; i++) {
        if (parsed_text.lines[i].len > max_str_len) {
            max_str_len = parsed_text.lines[i].len;
        }
    }
    open_bracket_buffer = malloc_b(max_str_len + 1);

    /*
     * Over-allocate memory for the autocomplete scores array as we don't know
     * how many there will be.
     */
    autocomplete_scores = malloc_b(parsed_text.num_lines * sizeof(size_t));

    /*
     * For each line, build up a buffer of open bracket types. When we
     * encounter a close bracket, make sure it is the the same bracket type as
     * the last open bracket on the buffer. If it is, remove that bracket from
     * the buffer and continue, if it is not the same there is a syntax error.
     *
     * Keep track of the syntax error score if there is an error and the
     * autocomplete scores if there is not.
     */
    *syntax_error_score = 0;
    num_non_syntax_error_lines = 0;
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
                 * Is a closed bracket, check the last char on the buffer was
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
                    is_syntax_error = true;
                    switch (c) {
                        case ')':
                            *syntax_error_score += 3;
                            break;
                        case ']':
                            *syntax_error_score += 57;
                            break;
                        case '}':
                            *syntax_error_score += 1197;
                            break;
                        case '>':
                            *syntax_error_score += 25137;
                            break;
                        default:
                            assert(false);
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
             * We made it to the end of the line without finding any syntax
             * errors. Have now got the buffer of remaining open brackets. The
             * order of the matching close brackets is simply the reverse of
             * the buffer.
             */
            *autocomplete_score = 0;
            for (; current_buffer_pos >= 1; current_buffer_pos--) {
                c = open_bracket_buffer[current_buffer_pos - 1];
                *autocomplete_score *= 5;
                switch (c) {
                    case '(':
                        *autocomplete_score += 1;
                        break;
                    case '[':
                        *autocomplete_score += 2;
                        break;
                    case '{':
                        *autocomplete_score += 3;
                        break;
                    case '<':
                        *autocomplete_score += 4;
                        break;
                    default:
                        assert(false);
                }
            }
            autocomplete_scores[num_non_syntax_error_lines++] =
                                                           *autocomplete_score;
        }
    }

    /* Realloc the autocomplete_scores array to have the right size */
    autocomplete_scores = realloc_b(
                                  autocomplete_scores,
                                  num_non_syntax_error_lines * sizeof(size_t));

    /*
     * The final autocomplete score is the middle value when sorted.
     */
    sort_long_numbers(autocomplete_scores, num_non_syntax_error_lines);
    *autocomplete_score = autocomplete_scores[num_non_syntax_error_lines / 2];

    free(autocomplete_scores);
    autocomplete_scores = NULL;

    free(open_bracket_buffer);
    open_bracket_buffer = NULL;
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
    size_t             syntax_error_score;
    size_t             autocomplete_score;

    parsed_text = parse_file(file_name);

    find_syntax_error_and_autocomplete_scores(parsed_text,
                                              &syntax_error_score,
                                              &autocomplete_score);
    if (print_output) {
        printf("Part 1: Syntax error score = %zu\n", syntax_error_score);
        printf("Part 2: Autocomplete score = %zu\n", autocomplete_score);
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
