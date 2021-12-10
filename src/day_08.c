/*
 * AoC 2021 Day 8 solution
 */

#include "utils.h"

/* Number of digits in the input */
#define NUM_UNIQUE_SIGNALS 10

/* Number of digits in the output */
#define NUM_OUTPUTS 4

/* Max array len to hold a digit string (7 segments + null char) */
#define MAX_PATTERN_STR_LEN 8

/* Macros for easily identifying unqiue numbers for part 1 */
#define IS_1(str)        (strlen(str) == 2) // 2 segments
#define IS_7(str)        (strlen(str) == 3) // 3 segments
#define IS_4(str)        (strlen(str) == 4) // 4 segments
#define IS_2_3_OR_5(str) (strlen(str) == 5) // 5 segments
#define IS_0_6_OR_9(str) (strlen(str) == 6) // 6 segments
#define IS_8(str)        (strlen(str) == 7) // 7 segments

/*
 * note_type
 *
 * Element: unique_signals
 *     The NUM_UNIQUE_SIGNALS before the '|' delimeter in a note.
 * Element: output
 *     The NUM_OUTPUTS after the '|' delimeter in a note.
 */
typedef struct Note {
    char unique_signals[NUM_UNIQUE_SIGNALS][MAX_PATTERN_STR_LEN];
    int  unique_signals_nums[NUM_UNIQUE_SIGNALS];
    char output[NUM_OUTPUTS][MAX_PATTERN_STR_LEN];
    int  output_nums[NUM_OUTPUTS];
} note_type;

/*
 * parse_text_into_note_types
 *
 * Parse lines of notes into an array of note_type structs.
 *
 * Argument: parsed_text
 *     Parsed text struct from the day's input.
 *
 * Return: note_type
 */
static note_type *
parse_text_into_note_types(parsed_text_type parsed_text)
{
    size_t     i, j;
    char       format_str[] = "%s %s %s %s %s %s %s %s %s %s | %s %s %s %s";
    note_type *notes = NULL;

    /*
     * Check the format string len matches the expected number of unique
     * signals and outputs
     */
    assert(strlen(format_str) == ((NUM_UNIQUE_SIGNALS + NUM_OUTPUTS) * 3 + 1));

    notes = calloc_b(parsed_text.num_lines, sizeof(note_type));

    for (i = 0; i < parsed_text.num_lines; i++) {
        sscanf(parsed_text.lines[i].line,
               format_str,
               notes[i].unique_signals[0],
               notes[i].unique_signals[1],
               notes[i].unique_signals[2],
               notes[i].unique_signals[3],
               notes[i].unique_signals[4],
               notes[i].unique_signals[5],
               notes[i].unique_signals[6],
               notes[i].unique_signals[7],
               notes[i].unique_signals[8],
               notes[i].unique_signals[9],
               notes[i].output[0],
               notes[i].output[1],
               notes[i].output[2],
               notes[i].output[3]);

        /* Sort strings so they are easier to deal with */
        for (j = 0; j < NUM_UNIQUE_SIGNALS; j++) {
            sort_string(notes[i].unique_signals[j],
                        strlen(notes[i].unique_signals[j]));
        }
        for (j = 0; j < NUM_OUTPUTS; j++) {
            sort_string(notes[i].output[j],
                        strlen(notes[i].output[j]));
        }

        /*
         * Set the corresponding numbers to 100 (i.e. an impossible value on
         * the display) so we know they have not yet been identified.
         */
        for (j = 0; j < NUM_UNIQUE_SIGNALS; j++) {
            notes[i].unique_signals_nums[j] = 100;
        }
        for (j = 0; j < NUM_OUTPUTS; j++) {
            notes[i].output_nums[j] = 100;
        }
    }

    return (notes);
}

/*
 * find_num_1_4_7_8_in_output
 *
 * Find the number of easily identifiable 1s, 4s, 7s and 8s in the outputs
 * of the notes.
 *
 * Argument: notes
 *     Array of note structs. The outputs do not need to yet be identified.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: size_t
 */
static size_t
find_num_1_4_7_8_in_output(note_type *notes, size_t len)
{
    size_t i, j;
    size_t num_1_4_7_8s = 0;

    for (i = 0; i < len; i++) {
        for (j = 0; j < NUM_OUTPUTS; j++) {
            if (IS_1(notes[i].output[j])
                || IS_4(notes[i].output[j])
                || IS_7(notes[i].output[j])
                || IS_8(notes[i].output[j])) {
                num_1_4_7_8s++;
            }
        }
    }

    return (num_1_4_7_8s);
}

/*
 * identify_1_4_7_8s
 *
 * Identify the easily identifiable 1s, 4s, 7s and 8s in the unique signals
 * of the notes.
 *
 * Argument: notes
 *     Array of note structs. Nothing needs to have been identified yet.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: void
 */
static void
identify_1_4_7_8s(note_type *notes, size_t len)
{
    size_t i, j;

    for (i = 0; i < len; i++) {
        for (j = 0; j < NUM_UNIQUE_SIGNALS; j++) {
            if (IS_1(notes[i].unique_signals[j])) {
                notes[i].unique_signals_nums[j] = 1;
            } else if (IS_4(notes[i].unique_signals[j])) {
                notes[i].unique_signals_nums[j] = 4;
            } else if (IS_7(notes[i].unique_signals[j])) {
                notes[i].unique_signals_nums[j] = 7;
            } else if (IS_8(notes[i].unique_signals[j])) {
                notes[i].unique_signals_nums[j] = 8;
            }
        }
    }
}

/*
 * identify_0_2_3_5_6_9s
 *
 * Identify the more difficult 0s, 2s, 3s, 5s, 6s and 9s in the unique signals
 * of the notes.
 *
 * Argument: notes
 *     Array of note structs. The 1s, 4s, 7s and 8s in the unique signals must
 *     have already been identified.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: void
 */
static void
identify_0_2_3_5_6_9s(note_type *notes, size_t len)
{
    size_t i, j, k;
    size_t index;
    size_t matching_segments;
    char   s[2] = "";

    for (i = 0; i < len; i++) {
        for (j = 0; j < NUM_UNIQUE_SIGNALS; j++) {
            if (IS_0_6_OR_9(notes[i].unique_signals[j])) {
                /*
                 * 0 and 9 share the same segments with 1 and 6 does not. We
                 * can also easily identify 1. Hence if the string under test
                 * does not contain the same segments as the 1 it must be a 6,
                 * otherwise it is a 0 or 9.
                 * A 4 also contains the same segments as a 9 but not a 0, and
                 * a 4 is easily identifiable so we can identify the 0 and 9 as
                 * well.
                 *
                 * First find the 1 string then check if each segment is in the
                 * string under test.
                 */
                index = 0;
                while (!IS_1(notes[i].unique_signals[index])) {
                    index++;
                }
                /*
                 * Found the 1. Check if each char in it is also in the string
                 * under test.
                 */
                for (k = 0; k < strlen(notes[i].unique_signals[index]); k++) {
                    s[0] = notes[i].unique_signals[index][k];
                    if (!is_str_in_str(notes[i].unique_signals[j], s)) {
                        /* Char was not in string under test, must be a 6. */
                        notes[i].unique_signals_nums[j] = 6;
                        break;
                    }
                }
                if (notes[i].unique_signals_nums[j] == 6) {
                    /* Was a set to a 6, continue. */
                    continue;
                }
                /*
                 * Was not set to a 6, must be a 0 or 9. Find the 4 then
                 * check if each segment is in the string under test.
                 */
                index = 0;
                while (!IS_4(notes[i].unique_signals[index])) {
                    index++;
                }
                /* Found the 4 */
                for (k = 0; k < strlen(notes[i].unique_signals[index]); k++) {
                    s[0] = notes[i].unique_signals[index][k];
                    if (!is_str_in_str(notes[i].unique_signals[j], s)) {
                        /* Char was not in string under test, must be a 0. */
                        notes[i].unique_signals_nums[j] = 0;
                        break;
                    }
                }
                if (notes[i].unique_signals_nums[j] != 0) {
                    /* Was not set to a 0, must be a 9. */
                    notes[i].unique_signals_nums[j] = 9;
                }
            } else if (IS_2_3_OR_5(notes[i].unique_signals[j])) {
                /*
                 * 3 shares the same segments with 1, and 2 and 6 do not. We
                 * can also easily identify 3. Hence if the string under test
                 * contains the same segments as the 1 it must be a 6,
                 * otherwise it is a 2 or 6.
                 *
                 * A 5 shares 3 segements with a 4, but a 2 only shares 2
                 * segments with a 4. We can identify a 4 and check how many
                 * segments match to identify a 2 and a 5.
                 *
                 * First find the 1 string then check if each segment is in the
                 * string under test.
                 */
                index = 0;
                while (!IS_1(notes[i].unique_signals[index])) {
                    index++;
                }
                /*
                 * Found the 1. Check if each char in it is also in the string
                 * under test.
                 */
                for (k = 0; k < strlen(notes[i].unique_signals[index]); k++) {
                    s[0] = notes[i].unique_signals[index][k];
                    if (!is_str_in_str(notes[i].unique_signals[j], s)) {
                        // Char was not in string under test, must be a 2 or 5.
                        break;
                    }
                }
                if (k == strlen(notes[i].unique_signals[index])) {
                    /*
                     * Checked all characters successfully without breaking,
                     * must be a 3.
                     */
                    notes[i].unique_signals_nums[j] = 3;
                    continue;
                }
                /*
                 * Find the 4 and check how many matches there are with the
                 * string under test.
                 */
                index = 0;
                while (!IS_4(notes[i].unique_signals[index])) {
                    index++;
                }
                /* Found the 4 */
                matching_segments = 0;
                for (k = 0; k < strlen(notes[i].unique_signals[index]); k++) {
                    s[0] = notes[i].unique_signals[index][k];
                    if (is_str_in_str(notes[i].unique_signals[j], s)) {
                        matching_segments++;
                    }
                }
                if (matching_segments == 3) {
                    /* 3 matches, must be a 5 */
                    notes[i].unique_signals_nums[j] = 5;
                } else if (matching_segments == 2) {
                    /* 2 matches, must be a 2 */
                    notes[i].unique_signals_nums[j] = 2;
                } else {
                    /* Something has gone wrong */
                    assert(false);
                }
            }
        }
    }
}

/*
 * identify_outputs
 *
 * Identify the output digits.
 *
 * Argument: notes
 *     Array of note structs. All unique signals for each note must have
 *     already been identified.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: void
 */
static void
identify_outputs(note_type *notes, size_t len)
{
    size_t i, j, k;

    for (i = 0; i < len; i++) {
        for (j = 0; j < NUM_OUTPUTS; j++) {
            /*
             * Find the string which matches this output in the unique signals
             * then get it's value from the num array.
             */
            k = 0;
            while (strncmp(notes[i].output[j],
                           notes[i].unique_signals[k],
                           strlen(notes[i].output[j]) + 1) != 0) {
                k++;
            }
            /* Found the matching string, get it's value */
            notes[i].output_nums[j] = notes[i].unique_signals_nums[k];
        }
    }
}

/*
 * find_sum_of_outputs
 *
 * Find the sum of all the outputs of the notes.
 *
 * Argument: notes
 *     Array of note structs. All output digits must have already been
 *     identified.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: int
 */
static int
find_sum_of_outputs(note_type *notes, size_t len)
{
    size_t i, j;
    int    output;
    int    sum = 0;

    for (i = 0; i < len; i++) {
        output = 0;
        for (j = 0; j < NUM_OUTPUTS; j++) {
            output *= 10;
            output += notes[i].output_nums[j];
        }
        sum += output;
    }

    return (sum);
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
    size_t            num_1_4_7_8s;
    note_type        *notes = NULL;
    int               output_sum;

    parsed_text = parse_file(file_name);

    notes = parse_text_into_note_types(parsed_text);

    num_1_4_7_8s = find_num_1_4_7_8_in_output(notes, parsed_text.num_lines);
    if (print_output) {
        printf("Part 1: Number of 1,4,7,8s = %zu\n", num_1_4_7_8s);
    }

    identify_1_4_7_8s(notes, parsed_text.num_lines);
    identify_0_2_3_5_6_9s(notes, parsed_text.num_lines);
    identify_outputs(notes, parsed_text.num_lines);
    output_sum = find_sum_of_outputs(notes, parsed_text.num_lines);
    if (print_output) {
        printf("Part 2: Sum of outputs = %d\n", output_sum);
    }

    free(notes);
    notes = NULL;
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
