/*
 * AoC 2021 Day 18 solution
 */

#include "utils.h"

/*
 * came_from_type
 *
 * Enum for representing which direction we reached the current num from when
 * traversing nested snailfish nums.
 */
typedef enum Came_From {
    LEFT,
    RIGHT,
    PARENT,
} came_from_type;

/*
 * snailfish_num_type
 *
 * Element: left_value
 *     Value of the left element of the snailfish num. Should only be accessed
 *     if left_snailfish_num is NULL and hence the left element is a literal
 *     value, not a sub-snailfish num.
 * Element: left_snailfish_num
 *     Pointer to the left element sub-snailfish num. If this is NULL the left
 *     element is a literal value instead.
 * Element: right_value
 *     Value of the right element of the snailfish num. Should only be accessed
 *     if right_snailfish_num is NULL and hence the right element is a literal
 *     value, not a sub-snailfish num.
 * Element: right_snailfish_num
 *     Pointer to the right element sub-snailfish num. If this is NULL the
 *     right element is a literal value instead.
 * Element: parent
 *     Pointer to the parent of this snailfish number. Will be NULL if this
 *     number has no parent.
 */
typedef struct Snailfish_Num {
    uint8_t               left_value;
    struct Snailfish_Num *left_snailfish_num;
    uint8_t               right_value;
    struct Snailfish_Num *right_snailfish_num;
    struct Snailfish_Num *parent;
} snailfish_num_type;

/*
 * initialise_snailfish_num
 *
 * Initialise a snailfish_num struct with initial values. The initial values
 * correspond to the number [0,0] with no parent.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to initialisef.
 *
 * Return: void
 */
static void
initialise_snailfish_num(snailfish_num_type *snailfish_num)
{
    /* Initialise to [0,0] without a parent */
    snailfish_num->left_value          = 0;
    snailfish_num->left_snailfish_num  = NULL;
    snailfish_num->right_value         = 0;
    snailfish_num->right_snailfish_num = 0;
    snailfish_num->parent              = NULL;
}

/*
 * copy_snailfish_num
 *
 * Make a copy of the given snailfish number. If the number has sub-snailfish
 * numbers as elements in its pair these will also be copied.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to make a copy of.
 *
 * Return: snailfish_num_type *
 */
static snailfish_num_type *
copy_snailfish_num(snailfish_num_type snailfish_num)
{
    snailfish_num_type *copy = NULL;

    copy = malloc_b(sizeof(snailfish_num_type));
    initialise_snailfish_num(copy);

    copy->left_value  = snailfish_num.left_value;
    copy->right_value = snailfish_num.right_value;
    copy->parent      = snailfish_num.parent;

    if (snailfish_num.left_snailfish_num != NULL) {
        copy->left_snailfish_num =
                       copy_snailfish_num(*(snailfish_num.left_snailfish_num));
        copy->left_snailfish_num->parent = copy;
    } else {
        /* No sub-snailfish nums as the left number, just a value */
        copy->left_snailfish_num = NULL;
    }

    if (snailfish_num.right_snailfish_num != NULL) {
        copy->right_snailfish_num =
                      copy_snailfish_num(*(snailfish_num.right_snailfish_num));
        copy->right_snailfish_num->parent = copy;
    } else {
        /* No sub-snailfish nums as the right number, just a value */
        copy->right_snailfish_num = NULL;
    }

    return (copy);
}

/*
 * free_snailfish_num
 *
 * Free the memory allocated by a snailfish_num_type struct.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to free the memory of.
 *
 * Return: void
 */
static void
free_snailfish_num(snailfish_num_type snailfish_num)
{
    if (snailfish_num.left_snailfish_num != NULL) {
        free_snailfish_num(*(snailfish_num.left_snailfish_num));
        free(snailfish_num.left_snailfish_num);
        snailfish_num.left_snailfish_num = NULL;
    }
    if (snailfish_num.right_snailfish_num != NULL) {
        free_snailfish_num(*(snailfish_num.right_snailfish_num));
        free(snailfish_num.right_snailfish_num);
        snailfish_num.right_snailfish_num = NULL;
    }
    snailfish_num.parent = NULL;
}

/*
 * print_snailfish_num
 *
 * Print a snailfish number to stdout.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to print.
 *
 * Return: void
 */
void
print_snailfish_num(snailfish_num_type num)
{
    printf("[");
    if (num.left_snailfish_num != NULL) {
        print_snailfish_num(*num.left_snailfish_num);
    } else {
        printf("%hd", num.left_value);
    }
    printf(",");
    if (num.right_snailfish_num != NULL) {
        print_snailfish_num(*num.right_snailfish_num);
    } else {
        printf("%hd", num.right_value);
    }
    printf("]");
    if (num.parent == NULL) {
        printf("\n");
    }
}

/*
 * parse_str_into_snailfish_num
 *
 * Parse a string into a snailfish_num struct. Line must be a valid snailfish
 * number. The number is not reduced. The memory of the returned snailfish
 * number must be freed by the caller.
 *
 * Argument: str
 *     String of the snailfish number to parse.
 * Argument: remaining_str
 *     Pointer to the next character in the str array after this snailfish
 *     number has been parsed. This is used in recursive calls internal to this
 *     function. Any external callers of this function should pass NULL.
 * Argument: remaining_str
 *     Pointer to the parent of the snailfish number being parsed. This is used
 *     in recursive calls internal to this function. Any external callers of
 *     this function should pass NULL.
 *
 * Return: snailfish_num_type
 */
static snailfish_num_type
parse_str_into_snailfish_num(char                *str,
                             char               **remaining_str,
                             snailfish_num_type  *parent)
{
    snailfish_num_type  snailfish_num;
    char               *current_str_ptr = NULL;
    int                 value;

    initialise_snailfish_num(&snailfish_num);
    snailfish_num.parent = parent;

    current_str_ptr = str;

    /* Make sure str is start of a snailfish num */
    assert(current_str_ptr[0] == '[');
    current_str_ptr++;

    /*
     * Parse the left element of the pair. If it starts with '[' it is a
     * sub-snailfish number, otherwise it will be a value.
     */
    if (current_str_ptr[0] != '[') {
        /* Not an opening bracket to another snailfish num, must be a digit */
        assert(sscanf(current_str_ptr, "%d,", &value) == 1);
        snailfish_num.left_value = (uint8_t) value;
    } else {
        /*
         * Is an opening bracket to another snailfish num. Parse the left
         * element.
         */
        snailfish_num.left_snailfish_num = malloc_b(
                                                   sizeof(snailfish_num_type));
        *(snailfish_num.left_snailfish_num) =
                                parse_str_into_snailfish_num(current_str_ptr,
                                                             &current_str_ptr,
                                                             &snailfish_num);

        /* Make sure this is the end of the first element */
        assert(current_str_ptr[0] == ',');
    }

    /* Skip the str until we reach the comma separating the pair */
    while (current_str_ptr[0] != ',') {
        current_str_ptr++;
    }
    current_str_ptr++;

    /* Right element may be another value or a sub-snailfish num */
    if (current_str_ptr[0] != '[') {
        /* Is another value */
        assert(sscanf(current_str_ptr, "%d]", &value) == 1);
        snailfish_num.right_value = (uint8_t) value;
    } else {
        /* Is a snailfish num */
        snailfish_num.right_snailfish_num = malloc_b(
                                                   sizeof(snailfish_num_type));
        *(snailfish_num.right_snailfish_num) =
                                parse_str_into_snailfish_num(current_str_ptr,
                                                             &current_str_ptr,
                                                             &snailfish_num);


        /* Make sure this is end of the pair */
        assert(current_str_ptr[0] == ']');
    }

    /* Skip the str until we reach the close bracket of this pair */
    while (current_str_ptr[0] != ']') {
        current_str_ptr++;
    }
    current_str_ptr++;

    if (remaining_str != NULL) {
        *remaining_str = current_str_ptr;
    }

    return (snailfish_num);
}

/*
 * parse_text_into_snailfish_nums
 *
 * Parse the day's input into a list of snailfish nums. The memory of the array
 * and elements within it must be freed by the caller.
 *
 * Argument: parsed_text
 *     Parsed text struct of the day's input.
 *
 * Return: snailfish_num_type *
 */
static snailfish_num_type *
parse_text_into_snailfish_nums(parsed_text_type parsed_text)
{
    size_t              i;
    snailfish_num_type *snailfish_nums = NULL;

    snailfish_nums = malloc_b(parsed_text.num_lines
                              * sizeof(snailfish_num_type));
    for (i = 0; i < parsed_text.num_lines; i++) {
        snailfish_nums[i] = parse_str_into_snailfish_num(
                                        parsed_text.lines[i].line, NULL, NULL);
    }

    return (snailfish_nums);
}

/*
 * explode_snailfish_num
 *
 * Explode a snailfish number. The snailfish number given must be a valid
 * candidate for explosion (have both pair elements be literal values and have
 * a parent). This function will set any linked snailfish numbers literal
 * values and parent pointers according to the explosion rules.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to explode.
 *
 * Return: void
 */
static void
explode_snailfish_num(snailfish_num_type *snailfish_num)
{
    snailfish_num_type *parent = NULL;
    came_from_type      came_from_original;
    came_from_type      came_from;

    /*
     * Snailfish nums can only be exploded if they have a parent and they must
     * not have any sub-snailfish nums as in the pair.
     */
    assert(snailfish_num->parent != NULL);
    assert(snailfish_num->left_snailfish_num == NULL
           && snailfish_num->right_snailfish_num == NULL);

    parent = snailfish_num->parent;
    if (parent->left_snailfish_num != NULL) {
        /* Left num is the one to be exploded */
        came_from_original = LEFT;
        parent->left_value = 0;
    } else {
        /* Right num is the one to be exploded */
        came_from_original = RIGHT;
        parent->right_value = 0;
    }

    /* Add the left value to the closest regular value on the right */
    came_from = came_from_original;
    while (parent != NULL) {
        if (came_from == LEFT) {
            /* Last came from a left node so must go up to parent */
            if (parent->parent != NULL) {
                /* Find out whether child is from left/right of new parent */
                if (parent == parent->parent->left_snailfish_num) {
                    came_from = LEFT;
                } else if (parent == parent->parent->right_snailfish_num) {
                    came_from = RIGHT;
                } else {
                    assert(false);
                }
            }
            parent = parent->parent;
        } else if (came_from == RIGHT) {
            /* Last came from a right node so can check left node */
            if (parent->left_snailfish_num == NULL) {
                /* Left node is a regular value, add the left value to it */
                parent->left_value += snailfish_num->left_value;
                break;
            } else {
                /* Left node is another snailfish num, must check that */
                parent = parent->left_snailfish_num;
                came_from = PARENT;
            }
        } else if (came_from == PARENT) {
            /* Last came from a parent node so check right node first */
            if (parent->right_snailfish_num == NULL) {
                /* Right node is a regular value, add the left value to it */
                parent->right_value += snailfish_num->left_value;
                break;
            } else {
                /* Right node is another snailfish num, must check that */
                parent = parent->right_snailfish_num;
                came_from = PARENT;
            }
        } else {
            assert(false);
        }
    }

    /* Add the right value to the closest regular value on the right */
    parent = snailfish_num->parent;
    came_from = came_from_original;
    while (parent != NULL) {
        if (came_from == LEFT) {
            /* Last came from a left node so can check right node */
            if (parent->right_snailfish_num == NULL) {
                /* Right node is a regular value, add the right value to it */
                parent->right_value += snailfish_num->right_value;
                break;
            } else {
                /* Right node is another snailfish num, must check that */
                parent = parent->right_snailfish_num;
                came_from = PARENT;
            }
        } else if (came_from == RIGHT) {
            /* Last came from a right node so must go up to parent */
            if (parent->parent != NULL) {
                /* Find out whether child is from left/right of new parent */
                if (parent == parent->parent->left_snailfish_num) {
                    came_from = LEFT;
                } else if (parent == parent->parent->right_snailfish_num) {
                    came_from = RIGHT;
                } else {
                    assert(false);
                }
            }
            parent = parent->parent;
        } else if (came_from == PARENT) {
            /* Last came from a parent node so check left node first */
            if (parent->left_snailfish_num == NULL) {
                /* Left node is a regular value, add the right value to it */
                parent->left_value += snailfish_num->right_value;
                break;
            } else {
                /* Left node is another snailfish num, must check that */
                parent = parent->left_snailfish_num;
                came_from = PARENT;
            }
        } else {
            assert(false);
        }
    }

    /* Free the snailfish num that was removed from the tree */
    parent = snailfish_num->parent;
    free_snailfish_num(*snailfish_num);
    if (came_from_original == LEFT) {
        /* Left num was the one exploded */
        free(parent->left_snailfish_num);
        parent->left_snailfish_num = NULL;
    } else {
        /* Right num was the one exploded */
        free(parent->right_snailfish_num);
        parent->right_snailfish_num = NULL;
    }
}

/*
 * split_snailfish_num
 *
 * Split a snailfish number. The snailfish number given must be a valid
 * candidate for splitting (have a pair element be literal value >= 10). This
 * function will set the parent pointers according to the splitting rules.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to split.
 *
 * Return: void
 */
static void
split_snailfish_num(snailfish_num_type *snailfish_num)
{
    /*
     * Snailfish nums can only be split if they have a parent and they must
     * have at least one value >= 10 to split.
     */
    assert(snailfish_num->parent != NULL);
    assert(snailfish_num->left_value >= 10
           || snailfish_num->right_value >= 10);

    if (snailfish_num->left_value >= 10) {
        /* Split the left element */
        snailfish_num->left_snailfish_num = malloc_b(
                                                   sizeof(snailfish_num_type));
        initialise_snailfish_num(snailfish_num->left_snailfish_num);
        snailfish_num->left_snailfish_num->left_value =
                                                 snailfish_num->left_value / 2;
        snailfish_num->left_snailfish_num->right_value =
                                           (snailfish_num->left_value + 1) / 2;
        snailfish_num->left_value = 0;
        snailfish_num->left_snailfish_num->parent = snailfish_num;
    } else {
        /* Split the right element */
        snailfish_num->right_snailfish_num = malloc_b(
                                                   sizeof(snailfish_num_type));
        initialise_snailfish_num(snailfish_num->right_snailfish_num);
        snailfish_num->right_snailfish_num->left_value =
                                                snailfish_num->right_value / 2;
        snailfish_num->right_snailfish_num->right_value =
                                          (snailfish_num->right_value + 1) / 2;
        snailfish_num->right_value = 0;
        snailfish_num->right_snailfish_num->parent = snailfish_num;
    }
}

/*
 * internal_reduce_snailfish_num_explode
 *
 * Search the snailfish number (and any children) for any pairs which can be
 * exploded and perform the explosion.
 * Return true if any explosion was performed, false if not.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to search for possible explosions.
 * Argument: depth
 *     The current depth of this snailfish num (the number of parents the given
 *     number has).  This is used in recursive calls internal to this function.
 *     Any external callers of this function should pass in 0 for the total
 *     snailfish num.
 *
 * Return: bool
 */
static bool
internal_reduce_snailfish_num_explode(snailfish_num_type *snailfish_num,
                                      size_t              depth)
{
    bool something_changed = false;

    /* Check the depth is 0 if the given snailfish number has no parent */
    assert(snailfish_num->parent != NULL || depth == 0);

    /* Check the left node */
    if (snailfish_num->left_snailfish_num != NULL) {
        something_changed = internal_reduce_snailfish_num_explode(
                                             snailfish_num->left_snailfish_num,
                                             depth + 1);
    } else if (depth >= 4 && snailfish_num->right_snailfish_num == NULL) {
        /* Both elements are numbers and at a depth>=4 so explode */
        explode_snailfish_num(snailfish_num);
        something_changed = true;
    }

    /* Check the right node */
    if (!something_changed && snailfish_num->right_snailfish_num != NULL) {
        /* Nothing was reduced in left node, check right */
        something_changed = internal_reduce_snailfish_num_explode(
                                            snailfish_num->right_snailfish_num,
                                            depth + 1);
    }

    return (something_changed);
}

/*
 * internal_reduce_snailfish_num_split
 *
 * Search the snailfish number (and any children) for any pairs which can be
 * split and perform the split.
 * Return true if any split was performed, false if not.
 *
 * Argument: snailfish_num
 *     Snailfish num struct to search for possible splits.
 *
 * Return: bool
 */
static bool
internal_reduce_snailfish_num_split(snailfish_num_type *snailfish_num)
{
    bool something_changed = false;

    /* Check the left node */
    if (snailfish_num->left_snailfish_num != NULL) {
        /* Check if left node can be reduced */
        something_changed = internal_reduce_snailfish_num_split(
                                            snailfish_num->left_snailfish_num);
    }

    /* See if either node can be split */
    if (!something_changed
        && (snailfish_num->left_value >= 10
            || snailfish_num->right_value >= 10)) {
        /* Left or right is a value that can be split */
        split_snailfish_num(snailfish_num);
        something_changed = true;
    }

    /* Check the right node */
    if (!something_changed && snailfish_num->right_snailfish_num != NULL) {
        something_changed = internal_reduce_snailfish_num_split(
                                           snailfish_num->right_snailfish_num);
    }

    return (something_changed);
}

/*
 * reduce_snailfish_num
 *
 * Reduce a snailfish number by performing explosions and splits until no
 * further actions can be performed (the number is fully reduced).
 *
 * Argument: snailfish_num
 *     Snailfish number to reduce.
 *
 * Return: void
 */
static void
reduce_snailfish_num(snailfish_num_type *snailfish_num)
{
    bool something_changed = true;

    while (something_changed) {
        something_changed = internal_reduce_snailfish_num_explode(
                                                             snailfish_num, 0);
        if (!something_changed) {
            something_changed = internal_reduce_snailfish_num_split(
                                                                snailfish_num);
        }
    }
}

/*
 * add_snailfish_nums
 *
 * Add two snailfish numbers and reduce the result. Note that snailfish number
 * addition is not commutative so the order the arguments are passed in as
 * matters. Returns the result of the addition. The memory of the returned
 * snailfish number must be freed by the caller.
 *
 * Argument: num_1
 *     Left snailfish number to add.
 * Argument: num_2
 *     Right snailfish number to add.
 *
 * Return: snailfish_num_type
 */
static snailfish_num_type
add_snailfish_nums(snailfish_num_type num_1, snailfish_num_type num_2)
{
    snailfish_num_type result;

    initialise_snailfish_num(&result);

    result.left_snailfish_num = copy_snailfish_num(num_1);
    result.left_snailfish_num->parent = &result;
    result.right_snailfish_num = copy_snailfish_num(num_2);
    result.right_snailfish_num->parent = &result;

    reduce_snailfish_num(&result);

    return (result);
}

/*
 * find_magnitude_of_snailfish_num
 *
 * Find the magnitude of the given snailfish number. The magnitude of a
 * snailfish number is 3 * the magnitude of the left element + 2 * the
 * magnitude of the right element, where the magnitude of a literal value is
 * the value itself.
 *
 * Argument: num
 *     Snailfish number to find the magnitude of.
 *
 * Return: size_t
 */
static size_t
find_magnitude_of_snailfish_num(snailfish_num_type num)
{
    size_t magnitude = 0;

    if (num.left_snailfish_num != NULL) {
        magnitude += 3 * find_magnitude_of_snailfish_num(
                                                      *num.left_snailfish_num);
    } else {
        magnitude += 3 * num.left_value;
    }

    if (num.right_snailfish_num != NULL) {
        magnitude += 2 * find_magnitude_of_snailfish_num(
                                                     *num.right_snailfish_num);
    } else {
        magnitude += 2 * num.right_value;
    }

    return (magnitude);
}

/*
 * find_maximum_magnitude_of_sums
 *
 * Find the maximum magnitude which can be obtained by adding any two snailfish
 * numbers together from the given array.
 *
 * Argument: snailfish_nums
 *     Array of snailfish numbers.
 * Argument: len
 *     Number of elements in the array.
 *
 * Return: size_t
 */
static size_t
find_maximum_magnitude_of_sums(snailfish_num_type *snailfish_nums, size_t len)
{
    size_t             i, j;
    size_t             max_magnitude;
    size_t             magnitude;
    snailfish_num_type sum_result;

    max_magnitude = 0;
    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) {
            if (i != j) {
                sum_result = add_snailfish_nums(snailfish_nums[i],
                                                snailfish_nums[j]);
                magnitude = find_magnitude_of_snailfish_num(sum_result);
                if (magnitude > max_magnitude) {
                    max_magnitude = magnitude;
                }
                free_snailfish_num(sum_result);
            }
        }
    }

    return (max_magnitude);
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
    parsed_text_type    parsed_text;
    snailfish_num_type *snailfish_nums = NULL;
    snailfish_num_type  final_num;
    snailfish_num_type *tmp = NULL;
    size_t              magnitude;
    size_t              i;

    parsed_text = parse_file(file_name);

    snailfish_nums = parse_text_into_snailfish_nums(parsed_text);

    tmp = copy_snailfish_num(snailfish_nums[0]);
    final_num = *tmp;
    for (i = 1; i < parsed_text.num_lines; i++) {
        final_num = add_snailfish_nums(*tmp, snailfish_nums[i]);
        free_snailfish_num(*tmp);
        *tmp = final_num;
    }
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }

    magnitude = find_magnitude_of_snailfish_num(final_num);
    if (print_output) {
        printf("Part 1: Magnitude of final number = %zu\n", magnitude);
    }

    magnitude = find_maximum_magnitude_of_sums(snailfish_nums,
                                               parsed_text.num_lines);
    if (print_output) {
        printf("Part 2: Maximum magnitude of two nums summed = %zu\n",
               magnitude);
    }

    free_snailfish_num(final_num);
    if (snailfish_nums != NULL) {
        for (i = 0; i < parsed_text.num_lines; i++) {
            free_snailfish_num(snailfish_nums[i]);
        }
        free(snailfish_nums);
        snailfish_nums = NULL;
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
