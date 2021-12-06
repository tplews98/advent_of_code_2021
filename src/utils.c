/*
 * Common helper utils
 */

#include "utils.h"

/*
 * LINE_BUF_SIZE:
 *     Maximum length of a line read from the file.
 */
#define LINE_BUF_SIZE 300

/*
 * initialise_parsed_text_type
 *
 * Sets the input parsed_text struct elements to their initial values.
 *
 * Element: parsed_text
 *     parsed_text_type struct to initialise.
 *
 * Returns: void
 *
 */
static void
initialise_parsed_text_type(parsed_text_type *parsed_text)
{
    parsed_text->lines = NULL;
    parsed_text->num_lines = 0;
}

/*
 * count_lines_in_file
 *
 * Counts the number of lines in the file.
 *
 * Argument: file_name
 *     Name of the file to get the number of lines of.
 *
 * Return: size_t
 *
 */
static size_t
count_lines_in_file(FILE *open_file)
{
    size_t num_lines;
    char   line_buf[LINE_BUF_SIZE];

    num_lines = 0;
    while (fgets(line_buf, LINE_BUF_SIZE, open_file) != NULL) {
        num_lines++;
    }
    fseek(open_file, 0, SEEK_SET);
    return (num_lines);
}

/*
 * Doc in utils.h
 */
parsed_text_type
parse_file(char *file_name)
{
    int               rc = 0;
    FILE             *fp = NULL;
    char              line_buf[LINE_BUF_SIZE];
    size_t            i;
    parsed_text_type  parsed_text;

    initialise_parsed_text_type(&parsed_text);

    /*
     * Open the file.
     */
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file %s\n", file_name);
        assert(false);
    }

    if (rc == 0) {
        parsed_text.num_lines = count_lines_in_file(fp);
        /*
         * Alloc the required space for the number of lines in the file.
         */
        parsed_text.lines = malloc(
                             parsed_text.num_lines * sizeof(parsed_line_type));
        assert(parsed_text.lines != NULL);

        /*
         * Parse each row into the instructions array.
         */
        i = 0;
        while (fgets(line_buf, LINE_BUF_SIZE, fp) != NULL) {
            /*
             * Remove newline from line_buf.
             */
            if (line_buf[strlen(line_buf) - 1] == '\n') {
                line_buf[strlen(line_buf) - 1] = '\0';
            }
            parsed_text.lines[i].line = strndup(line_buf, strlen(line_buf) + 1);
            parsed_text.lines[i].len = strlen(parsed_text.lines[i].line);
            i++;
        }
    }

    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    return (parsed_text);
}

/*
 * Doc in utils.h
 */
void
free_parsed_text(parsed_text_type parsed_text)
{
    size_t i;

    if (parsed_text.lines != NULL) {
        for (i = 0; i < parsed_text.num_lines; i++) {
            free(parsed_text.lines[i].line);
            parsed_text.lines[i].line = NULL;
        }
        free(parsed_text.lines);
        parsed_text.lines = NULL;
    }
}

/*
 * Doc in utils.h
 */
int *
parse_text_to_ints(parsed_text_type parsed_text)
{
    int    *numbers_array = NULL;
    size_t  i;

    numbers_array = malloc(parsed_text.num_lines * sizeof(int));
    assert(numbers_array != NULL);

    for (i = 0; i < parsed_text.num_lines; i++) {
        numbers_array[i] = atoi(parsed_text.lines[i].line);
    }

    return (numbers_array);
}

/*
 * Doc in utils.h
 */
int *
parse_binary_num_text_to_ints(parsed_text_type parsed_text)
{
    int    *numbers_array = NULL;
    size_t  i;

    numbers_array = malloc(parsed_text.num_lines * sizeof(int));
    assert(numbers_array != NULL);

    for (i = 0; i < parsed_text.num_lines; i++) {
        numbers_array[i] = strtol(parsed_text.lines[i].line, NULL, 2);
    }

    return (numbers_array);
}

/*
 * Doc in utils.h
 */
parsed_text_type
split_string_on_char(char *text, char split_on)
{
    size_t            i;
    char              split_on_str[2] = "";
    char             *text_copy;
    char             *token;
    parsed_text_type  parsed_text;

    split_on_str[0] = split_on;
    initialise_parsed_text_type(&parsed_text);

    /*
     * Figure out how many splits we need to do to later allocate the required
     * memory.
     */
    text_copy = strndup(text, strlen(text));
    token = strtok(text_copy, split_on_str);
    while (token != NULL) {
        parsed_text.num_lines++;
        token = strtok(NULL, split_on_str);
    }
    free(text_copy);

    parsed_text.lines = malloc(
                             parsed_text.num_lines * sizeof(parsed_line_type));
    assert(parsed_text.lines != NULL);

    /*
     * Redo the splitting, this time filling the allocated array.
     */
    text_copy = strndup(text, strlen(text));
    token = strtok(text_copy, split_on_str);
    i = 0;
    while (token != NULL) {
        parsed_text.lines[i].line = strndup(token, strlen(token) + 1);
        parsed_text.lines[i].len = strlen(parsed_text.lines[i].line);
        i++;

        token = strtok(NULL, split_on_str);
    }
    free(text_copy);

    return (parsed_text);
}

/*
 * Doc in utils.h
 */
void
print_elapsed_time(double runtime_ns, char *description)
{
    if (runtime_ns > 1000000000) {
        printf("%s: %.3fs\n", description, runtime_ns / 1000000000);
    } else if (runtime_ns > 1000000) {
        printf("%s: %.3fms\n", description, runtime_ns / 1000000);
    } else if (runtime_ns > 1000) {
        printf("%s: %.3fus\n", description, runtime_ns / 1000);
    } else {
        printf("%s: %.0fns\n", description, runtime_ns);
    }
}
