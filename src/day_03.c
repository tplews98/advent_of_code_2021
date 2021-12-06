/*
 * AoC 2021 Day 3 solution
 */

#include "utils.h"

/*
 * find_most_significant_bit
 *
 * Find the most significant set bit in the number, where bit 0 is the 0/1
 * binary value.
 *
 * Argument: num
 *     Number to find highest set bit of.
 *
 * Return: size_t
 */
static size_t
find_most_significant_bit(int num)
{
    size_t most_sig_bit = 0;

    while (num > 1) {
        num = num >> 1;
        most_sig_bit++;
    }

    return (most_sig_bit);
}

/*
 * find_most_significant_bit_from_array
 *
 * Find the most significant set bit for any number in the given array, where
 * bit 0 is the 0/1 binary value.
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 *
 * Return: size_t
 */
static size_t
find_most_significant_bit_from_array(int *numbers_array, size_t len)
{
    size_t most_sig_bit;
    size_t i;
    int    tmp;

    tmp = 0;
    for (i = 0; i < len; i++) {
        tmp |= numbers_array[i];
    }
    most_sig_bit = find_most_significant_bit(tmp);

    return (most_sig_bit);
}

/*
 * invert_bits_in_number_up_to_max_bit
 *
 * For the given number, invert the bits of it up to the most significant
 * max_bit in the number, where bit 0 is the 0/1 binary value.
 *
 * Argument: num
 *     Number to invert.
 * Argument: max_bit
 *     Highest most significant bit to invert. All higher bits will be 0.
 *
 * Return: size_t
 */
static size_t
invert_bits_in_number_up_to_max_bit(int num, size_t max_bit)
{
    int mask;

    mask = (1 << (max_bit + 1)) - 1;

    return ~num & mask;
}

/*
 * find_most_common_least_significant_bit
 *
 * For an array of integers, find the most common least significant `n` bit.
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: n
 *     The least significant bit to check for the most common value, where bit
 *     0 is the 0/1 binary value.
 * Argument: choose_if_equal
 *     Value to choose if there is an equal number of 1s and 0s. Must be 0 or
 *     1.
 *
 * Return: int
 */
static int
find_most_common_least_significant_bit(int    *numbers_array,
                                       size_t  len,
                                       size_t  n,
                                       int     choose_if_equal)
{
    int    most_common;
    int    num_ones;
    size_t i;

    assert(choose_if_equal == 0 || choose_if_equal == 1);

    /*
     * Bit shift the number to get the nth least significant bit as the first
     * least significant bit, then AND with 1. This returns 1 if the bit was a
     * one and 0 if a 0. We find the total number of 1s here.
     */
    num_ones = 0;
    for (i = 0; i < len; i++) {
        num_ones += (numbers_array[i] >> n ) & 1;
    }

    /*
     * If the total number of 1s is > half the total number of numbers in the
     * array, then 1 is the most common bit, else 0.
     */
    if (num_ones * 2 > len) {
        most_common = 1;
    } else if (num_ones * 2 < len) {
        most_common = 0;
    } else {
        // Equal number of 1s and 0s.
        most_common = choose_if_equal;
    }

    return (most_common);
}

/*
 * filter_numbers_array_on_matching_bit
 *
 * Filter the numbers array
 *
 * Argument: numbers_array
 *     Array of integers. This will be modified by this function, the out
 *     pointer will point to the filtered array.
 * Argument: len
 *     Length of numbers_array. This will be modified by this function, the out
 *     pointer will point to the filtered array length.
 * Argument: bit_position
 *     The bit position to consider.
 * Argument: value_to_filter
 *     0 or 1 to filter on.
 *
 * Return: void
 */
static void
filter_numbers_array_on_matching_bit(int    **numbers_array,
                                     size_t  *len,
                                     size_t   bit_position,
                                     size_t   value_to_filter)
{
    size_t  i, j;
    int     mask;
    int    *filtered_array = NULL;
    size_t  filtered_array_len;

    assert(value_to_filter == 0 || value_to_filter == 1);

    mask = 1 << bit_position;

    /*
     * Find the number of matching numbers in the array and allocate the
     * required space for them.
     */
    filtered_array_len = 0;
    for (i = 0; i < *len; i++) {
        if (value_to_filter == 0 && ~(*numbers_array)[i] & mask) {
            filtered_array_len++;
        } else if (value_to_filter == 1 && (*numbers_array)[i] & mask) {
            filtered_array_len++;
        }
    }

    filtered_array = malloc(sizeof(filtered_array) * filtered_array_len);
    assert(filtered_array != NULL);

    /*
     * Add the matching numbers to the new array.
     */
    for (i = 0, j = 0; i < *len; i++) {
        if (value_to_filter == 0 && ~(*numbers_array)[i] & mask) {
            filtered_array[j++] = (*numbers_array)[i];
        } else if (value_to_filter == 1 && (*numbers_array)[i] & mask) {
            filtered_array[j++] = (*numbers_array)[i];
        }
    }

    /*
     * Free the old array and set the out pointers.
     */
    free(*numbers_array);
    *numbers_array = filtered_array;
    *len = filtered_array_len;
}

/*
 * find_gamma_rate
 *
 * Find the gamma rate from the numbers array. To calculate this, for each bit
 * in each number in the array, find the most common bit. This most common bit
 * forms the same-significance bit in the gamma rate number.
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: most_sig_bit
 *     The maximum bit to consider, all higher bits will be 0.
 *
 * Return: int
 */
static int
find_gamma_rate(int *numbers_array, size_t len, size_t most_sig_bit)
{
    size_t most_common_sig_bit;
    int    current_bit;
    int    final_num;

    /*
     * Find the most common bit for each bit, starting from the most
     * significant.
     */
    final_num = 0;
    for (current_bit = most_sig_bit; current_bit >= 0; current_bit--) {
        most_common_sig_bit = find_most_common_least_significant_bit(
                                                                numbers_array,
                                                                len,
                                                                current_bit,
                                                                0);
        final_num = final_num << 1;
        final_num += most_common_sig_bit;
    }

    return (final_num);
}

/*
 * find_epsilon_rate
 *
 * Find the epsilon rate rating from the numbers array. This can be calculated
 * from the gamma rate by flipping all the bits in the number up to a maximum
 * bit position.
 *
 * Argument: gamma_rate
 *     Gamma rate calculated from an array of numbers.
 * Argument: most_sig_bit
 *     The maximum bit to consider which was set in a number frpm the array of
 *     numbers that was used to calculate the gamma rate..
 *
 * Return: int
 */
static int
find_epsilon_rate(int gamma_rate, size_t most_sig_bit)
{
    int    epsilon_rate;

    epsilon_rate = invert_bits_in_number_up_to_max_bit(gamma_rate,
                                                       most_sig_bit);

    return (epsilon_rate);
}

/*
 * find_oxygen_rating
 *
 * Find the oxygen rating from the numbers array. To calculate this:
 *   - Determine the most common value in the most_sig_bit bit position
 *   - Filter the numbers_array to only those with this most common value
 *   - Repeat the first step with the next most significant bit position and
 *     so on, until only one number remains, which is the oxygen rating
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: most_sig_bit
 *     The maximum bit to consider, all higher bits are considered to be 0.
 *
 * Return: int
 */
static int
find_oxygen_rating(int *numbers_array, size_t len, size_t most_sig_bit)
{
    int    *filtered_array = NULL;
    size_t  current_bit;
    int     oxygen_rating;

    filtered_array = malloc(sizeof(int) * len);
    assert(filtered_array != NULL);
    memcpy(filtered_array, numbers_array, sizeof(int) * len);

    current_bit = most_sig_bit;
    while (len > 1) {
        filter_numbers_array_on_matching_bit(
                                        &filtered_array,
                                        &len,
                                        current_bit,
                                        find_most_common_least_significant_bit(
                                                                filtered_array,
                                                                len,
                                                                current_bit,
                                                                1));
        if (current_bit == 0) {
            // We have used all bits, can only be one entry remaining.
            assert(len == 1);
        } else {
            current_bit--;
        }
    }
    oxygen_rating = filtered_array[0];

    free(filtered_array);

    return oxygen_rating;
}

/*
 * find_C02_rating
 *
 * Find the C02 rating from the numbers array. To calculate this:
 *   - Determine the least common value in the most_sig_bit bit position
 *   - Filter the numbers_array to only those with this least common value
 *   - Repeat the first step with the next most significant bit position and
 *     so on, until only one number remains, which is the C02 rating
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: most_sig_bit
 *     The maximum bit to consider, all higher bits are considered to be 0.
 *
 * Return: int
 */
static int
find_C02_rating(int *numbers_array, size_t len, size_t most_sig_bit)
{
    int    *filtered_array = NULL;
    int     most_common_val;
    size_t  current_bit;
    int     c02_rating;

    filtered_array = malloc(sizeof(int) * len);
    assert(filtered_array != NULL);
    memcpy(filtered_array, numbers_array, sizeof(int) * len);

    current_bit = most_sig_bit;
    while (len > 1) {
        most_common_val = find_most_common_least_significant_bit(
                                                                filtered_array,
                                                                len,
                                                                current_bit,
                                                                1);
        filter_numbers_array_on_matching_bit(
                      &filtered_array,
                      &len,
                      current_bit,
                      invert_bits_in_number_up_to_max_bit(most_common_val, 0));
        if (current_bit == 0) {
            // We have used all bits, can only be one entry remaining.
            assert(len == 1);
        } else {
            current_bit--;
        }
    }
    c02_rating = filtered_array[0];

    free(filtered_array);

    return c02_rating;
}


/*
 * Main function.
 */
int
main(int argc, char **argv)
{
    struct timespec   start_time, end_time;
    int               rc = 0;
    char             *file_name = NULL;
    parsed_text_type  parsed_text;
    int              *numbers_array = NULL;
    size_t            most_sig_bit;
    int               gamma_rate;
    int               epsilon_rate;
    int               oxygen_rating;
    int               c02_rating;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    numbers_array = parse_binary_num_text_to_ints(parsed_text);
    most_sig_bit = find_most_significant_bit_from_array(numbers_array,
                                                        parsed_text.num_lines);

    gamma_rate = find_gamma_rate(numbers_array,
                                 parsed_text.num_lines,
                                 most_sig_bit);
    epsilon_rate = find_epsilon_rate(gamma_rate, most_sig_bit);
    printf("Part 1: Gamma = %d, Epsilon = %d, G*E = %d\n",
           gamma_rate,
           epsilon_rate,
           gamma_rate * epsilon_rate);

    oxygen_rating = find_oxygen_rating(numbers_array,
                                       parsed_text.num_lines,
                                       most_sig_bit);
    c02_rating = find_C02_rating(numbers_array,
                                 parsed_text.num_lines,
                                 most_sig_bit);
    printf("Part 2: Oxygen = %d, C02 = %d, O*C = %d\n",
           oxygen_rating,
           c02_rating,
           oxygen_rating * c02_rating);

    free(numbers_array);
    free_parsed_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}
