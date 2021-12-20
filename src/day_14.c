/*
 * AoC 2021 Day 14 solution
 */

#include "utils.h"

/*
 * polymer_pair_type
 *
 * Element: pair
 *     The pair of letters in the polymer.
 * Element: num
 *     Number of occurances of this pair in the polymer.
 */
typedef struct Polymer_Pair {
    char   pair[3];
    size_t num;
} polymer_pair_type;

/*
 * polymer_type
 *
 * Element: pairs
 *     Array of pairs in the polymer.
 * Element: num_pairs
 *     Number of elements in the pairs array.
 */
typedef struct Polymer {
    polymer_pair_type *pairs;
    size_t             num_pairs;
} polymer_type;

/*
 * pair_rule_type
 *
 * Element: pair
 *     The pair of letters which makes the result.
 * Element: result
 *     Result of the pair (what should be inserted between them).
 */
typedef struct Pair_Rule {
    char pair[3];
    char result;
} pair_rule_type;

/*
 * pair_rules_type
 *
 * Element: rules
 *     Array of pair rules.
 * Element: num_rules
 *     Number of elements in the rules array.
 */
typedef struct Pair_Rules {
    pair_rule_type *rules;
    uint8_t         num_rules;
} pair_rules_type;

/*
 * find_polymer_pair
 *
 * Find the matching polymer pair in the polymer pairs array and return a
 * pointer to it.
 *
 * Argument: pair
 *     Pair to find in polymer.
 * Argument: polymer
 *     Polymer type struct to find pair in.
 *
 * Return: void
 */
static polymer_pair_type *
find_polymer_pair(char *pair, polymer_type polymer)
{
    uint8_t i;

    for (i = 0; i < polymer.num_pairs; i++) {
        if (STRS_EQUAL_WITH_SIZE(pair, polymer.pairs[i].pair, 3)) {
            return (&(polymer.pairs[i]));
        }
    }
    /* Did not find a mathching pair */
    assert(false);

}

/*
 * parse_text_into_polymer_and_rules
 *
 * Parse the day's input into an initial polymer and the set of insertion
 * rules.
 *
 * Argument: parsed_text
 *     IN: Parsed text struct of the day's input.
 * Argument: polymer
 *     OUT: Initial polymer. Memory must be freed by the caller.
 * Argument: pair_rules
 *     OUT: Set of insertion rules for the polymers. Memory must be freed by
 *     the caller.
 *
 * Return: void
 */
static void
parse_text_into_polymer_and_rules(parsed_text_type  parsed_text,
                                  polymer_type     *polymer,
                                  pair_rules_type  *pair_rules)
{
    uint8_t i;
    char    pair[3] = "";

    /*
     * The first line of the text is the initial polymer, every other line
     * (apart from the empty second line) is a pair rule.
     *
     * Allocate the memory to store the pair rules and polymer pairs.
     */
    polymer->num_pairs = parsed_text.num_lines - 2;
    polymer->pairs = calloc_b(polymer->num_pairs, sizeof(polymer_pair_type));
    pair_rules->num_rules = parsed_text.num_lines - 2;
    pair_rules->rules = malloc_b(
                               pair_rules->num_rules * sizeof(pair_rule_type));

    /* Setup the polymer with the possible pairs and setup the pair rules */
    for (i = 2; i < parsed_text.num_lines; i++) {
        sscanf(parsed_text.lines[i].line,
               "%s -> %c",
               pair,
               &pair_rules->rules[i-2].result);
        strncpy(pair_rules->rules[i-2].pair, pair, 3);
        strncpy(polymer->pairs[i-2].pair, pair, 3);
    }

    /* Setup the initial pair counts from the initial polymer */
    for (i = 0; i < parsed_text.lines[0].len - 1; i++) {
        pair[0] = parsed_text.lines[0].line[i];
        pair[1] = parsed_text.lines[0].line[i+1];
        find_polymer_pair(pair, *polymer)->num++;
    }
}

/*
 * find_result_of_pair
 *
 * For the given pair, find the resulting element to be inserted between the
 * pair elements according to the given pair rules.
 *
 * Argument: pair
 *     Pair to find result of.
 * Argument: pair_rules
 *     Set of insertion rules for the polymers.
 *
 * Return: char
 */
static char
find_result_of_pair(char *pair, pair_rules_type pair_rules)
{
    uint8_t i;

    for (i = 0; i < pair_rules.num_rules; i++) {
        if (STRS_EQUAL_WITH_SIZE(pair, pair_rules.rules[i].pair, 3)) {
            return (pair_rules.rules[i].result);
        }
    }
    /* Did not find a mathching pair */
    assert(false);
}

/*
 * perform_polymer_step
 *
 * Perform a step on the polymer according to the pair rules.
 *
 * Argument: polymer
 *     IN & OUT: Polymer to perform step on. Modified polymer is returned as
 *     this argument.
 * Argument: pair_rules
 *     Set of insertion rules for the polymers.
 *
 * Return: void
 */
static void
perform_polymer_step(polymer_type *polymer, pair_rules_type pair_rules)
{
    size_t       i;
    char         pair[3] = "";
    char         result;
    polymer_type new_polymer;

    /*
     * Make a copy of the polymer so it is not modified when we are calculating
     * the number of new pairs.
     */
    new_polymer.num_pairs = polymer->num_pairs;
    new_polymer.pairs = calloc_b(new_polymer.num_pairs,
                                 sizeof(polymer_pair_type));
    for (i = 0; i < polymer->num_pairs; i++) {
        strncpy(new_polymer.pairs[i].pair, polymer->pairs[i].pair, 3);
    }

    for (i = 0; i < polymer->num_pairs; i++) {
        result = find_result_of_pair(polymer->pairs[i].pair, pair_rules);
        /*
         * For both chars in the current pair, increase the count of the newly
         * formed pairs by the number of the current pairs.
         */
        pair[0] = polymer->pairs[i].pair[0];
        pair[1] = result;
        find_polymer_pair(pair, new_polymer)->num += polymer->pairs[i].num;
        pair[0] = result;
        pair[1] = polymer->pairs[i].pair[1];
        find_polymer_pair(pair, new_polymer)->num += polymer->pairs[i].num;
    }

    /* Free current polymer and set it to the new polymer */
    free(polymer->pairs);
    polymer->pairs = new_polymer.pairs;
}

/*
 * find_num_occurances_of_most_common_char_in_polymer
 *
 * Find the number of occurances of the most common element in the polymer.
 *
 * Argument: polymer
 *     Polymer to find the most common element of.
 * Argument: initial_polymer
 *     String of the initial state of the polymer.
 *
 * Return: size_t
 */
static size_t
find_num_occurances_of_most_common_char_in_polymer(
                                                 polymer_type  polymer,
                                                 char         *initial_polymer)
{
    size_t        count[UCHAR_MAX + 1] = {0};
    size_t        i;
    size_t        max = 0;

    /*
     * Find the counts for each element from the count of the pairs. This will
     * double count elements as each element (apart from the first and last)
     * are in two pairs.
     */
    for (i = 0; i < polymer.num_pairs; i++) {
        count[(unsigned char) polymer.pairs[i].pair[0]] +=
                                                        polymer.pairs[i].num;
        count[(unsigned char) polymer.pairs[i].pair[1]] +=
                                                        polymer.pairs[i].num;
    }

    /*
     * Everything has been double counted except the first and last elements
     * in the initial polymer as they only appear in one pair. Increase their
     * counts by one to account for this.
     */
    count[(unsigned char) initial_polymer[0]]++;
    count[(unsigned char) initial_polymer[strlen(initial_polymer) - 1]]++;

    max = 0;
    for (i = 0; i < (UCHAR_MAX + 1); i++) {
        if (count[i] > 0 && max < count[i]) {
            max = count[i];
        }
    }

    return (max / 2);
}

/*
 * find_num_occurances_of_least_common_char_in_polymer
 *
 * Find the number of occurances of the most least element in the polymer.
 *
 * Argument: polymer
 *     Polymer to find the least common element of.
 * Argument: initial_polymer
 *     String of the initial state of the polymer.
 *
 * Return: size_t
 */
static size_t
find_num_occurances_of_least_common_char_in_polymer(
                                                 polymer_type  polymer,
                                                 char         *initial_polymer)
{
    size_t count[UCHAR_MAX + 1] = {0};
    size_t i;
    size_t min;

    /*
     * Find the counts for each element from the count of the pairs. This will
     * double count elements as each element (apart from the first and last)
     * are in two pairs.
     */
    min = 0;
    for (i = 0; i < polymer.num_pairs; i++) {
        min += polymer.pairs[i].num * 2;
        count[(unsigned char) polymer.pairs[i].pair[0]] +=
                                                        polymer.pairs[i].num;
        count[(unsigned char) polymer.pairs[i].pair[1]] +=
                                                        polymer.pairs[i].num;
    }

    /*
     * Everything has been double counted except the first and last elements
     * in the initial polymer as they only appear in one pair. Increase their
     * counts by one to account for this.
     */
    count[(unsigned char) initial_polymer[0]]++;
    count[(unsigned char) initial_polymer[strlen(initial_polymer) - 1]]++;

    for (i = 0; i < (UCHAR_MAX + 1); i++) {
        if (count[i] > 0 && min > count[i]) {
            min = count[i];
        }
    }

    return (min / 2);
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
    parsed_text_type parsed_text;
    polymer_type     polymer;
    pair_rules_type  pair_rules;
    size_t           i;

    parsed_text = parse_file(file_name);

    parse_text_into_polymer_and_rules(parsed_text, &polymer, &pair_rules);

    for (i = 0; i < 10; i++) {
        perform_polymer_step(&polymer, pair_rules);
    }
    if (print_output) {
        printf("Part 1: Quantity most common - least common = %zu\n",
               find_num_occurances_of_most_common_char_in_polymer(
                                          polymer, parsed_text.lines[0].line)
               - find_num_occurances_of_least_common_char_in_polymer(
                                          polymer, parsed_text.lines[0].line));
    }

    for (; i < 40; i++) {
        perform_polymer_step(&polymer, pair_rules);
    }
    if (print_output) {
        printf("Part 2: Quantity most common - least common = %zu\n",
               find_num_occurances_of_most_common_char_in_polymer(
                                          polymer, parsed_text.lines[0].line)
               - find_num_occurances_of_least_common_char_in_polymer(
                                          polymer, parsed_text.lines[0].line));
    }

    free(pair_rules.rules);
    pair_rules.rules = NULL;
    free(polymer.pairs);
    polymer.pairs = NULL;

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
