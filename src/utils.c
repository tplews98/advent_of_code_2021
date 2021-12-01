/*
 * Common helper utils
 */

#include "utils.h"

/*
 * LINE_BUF_SIZE:
 *     Maximum length of a line read from the file.
 */
#define LINE_BUF_SIZE 120

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
free_parse_text(parsed_text_type parsed_text)
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
