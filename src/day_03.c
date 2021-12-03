/*
 * AoC 2021 Day 2 solution
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
 *
 * Return: int
 */
static int
find_most_common_least_significant_bit(int    *numbers_array,
                                       size_t  len,
                                       size_t  n)
{
    int    most_common;
    int    num_ones;
    size_t i;

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
    if (num_ones > len / 2) {
        most_common = 1;
    } else if (num_ones < len / 2) {
        most_common = 0;
    } else {
        // Equal number of 1s and 0s, this shouldn't happen.
        assert(false);
    }

    return (most_common);
}

/*
 * find_most_common_number_of_each_bit
 *
 * For an array of integers, for each bit in the numbers, find the most common
 * bit. This most common bit forms the same-significance bit in the returned
 * number.
 *
 * Argument: numbers_array
 *     Array of integers.
 * Argument: len
 *     Length of numbers_array.
 * Argument: most_sig_bit
 *     The maximum bit to consider, all higher bits will be 0.
 *
 * Return: void
 */
static int
find_most_common_number_of_each_bit(int    *numbers_array,
                                    size_t  len,
                                    size_t  most_sig_bit)
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
                                                                current_bit);
        final_num = final_num << 1;
        final_num += most_common_sig_bit;
    }

    return (final_num);
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
    int               most_common_total_num;
    int               least_common_total_num;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    numbers_array = parse_binary_num_text_to_ints(parsed_text);
    most_sig_bit = find_most_significant_bit_from_array(numbers_array,
                                                        parsed_text.num_lines);

    most_common_total_num = find_most_common_number_of_each_bit(
                                                        numbers_array,
                                                        parsed_text.num_lines,
                                                        most_sig_bit);
    least_common_total_num = invert_bits_in_number_up_to_max_bit(
                                                         most_common_total_num,
                                                         most_sig_bit);
    printf("Part 1: Most common num = %d, Least common num = %d, M*L = %d\n",
           most_common_total_num,
           least_common_total_num,
           most_common_total_num * least_common_total_num);

    free_parse_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}
