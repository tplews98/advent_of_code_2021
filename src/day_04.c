/*
 * AoC 2021 Day 4 solution
 */

#include "utils.h"

/* Number of lines and columns in the bingo card */
#define NUM_BINGO_LINES   5
#define NUM_BINGO_COLUMNS 5

/* Macro for easily identifying emtpy strings */
#define IS_EMTPY_STR(str) (strncmp(str, "", 2) == 0)

/*
 * bingo_calls_type
 *
 * Element: calls
 *     Array of calls.
 * Element: num_calls
 *     The number of calls in the array.
 */
typedef struct Bingo_Calls {
    int *calls;
    int  num_calls;
} bingo_calls_type;

/*
 * bingo_square_type
 *
 * Element: num
 *     The bingo number of this square.
 * Element: called
 *     If this bingo number has been called.
 */
typedef struct Bingo_Square {
    int  num;
    bool called;
} bingo_square_type;

/*
 * bingo_card_type
 *
 * Element: lines
 *     2D array. First array is of the lines in the bingo card. Second array
 *     is the columns in that particular line.
 * Element: has_line
 *     If this bingo card has a completed line.
 */
typedef struct Bingo_Card {
    bingo_square_type lines[NUM_BINGO_LINES][NUM_BINGO_COLUMNS];
    bool              has_line;
} bingo_card_type;

/*
 * bingo_card_type
 *
 * Element: cards
 *     Array of bingo card structs.
 * Element: num_cards
 *     Number of bingo cards in array.
 */
typedef struct Bingo_Cards {
    bingo_card_type *cards;
    size_t           num_cards;
} bingo_cards_type;

/*
 * parse_lines_into_calls_and_cards
 *
 * Parse text into a bingo calls struct and an array of bingo card structs. The
 * first line should be a comma-separated list of integers, then each bingo
 * card is separated by a newline, each line is on a new line and each value in
 * the line is space separated.
 *
 * Argument: parsed_text
 *     Parsed text struct.
 * Argument: bingo_calls
 *     OUT: Bingo calls struct to fill. Memory for the `calls` struct element
 *     is allocated by this function and must be freed by the caller.
 * Argument: bingo_cards
 *     OUT: Array of bingo_card structs. Memory is allocated by this function
 *     and must be freed by the caller.
 *
 * Return: void
 */
static void
parse_lines_into_calls_and_cards(parsed_text_type   parsed_text,
                                 bingo_calls_type  *bingo_calls,
                                 bingo_cards_type  *bingo_cards)
{
    parsed_text_type  parsed_bingo_text;
    parsed_text_type  split_text;
    size_t            current_line;
    size_t            current_card;
    int              *parsed_line_ints;
    size_t            i, j;

    /* The first line is a comma separated list of intergers */
    parsed_bingo_text = split_string_on_char(parsed_text.lines[0].line, ',');
    bingo_calls->calls = parse_text_to_ints(parsed_bingo_text);
    bingo_calls->num_calls = parsed_bingo_text.num_lines;

    /* Find the number of bingo cards in the rest of the text. */
    for (bingo_cards->num_cards = 0, current_line = 2;
         current_line < parsed_text.num_lines; ) {
        if (IS_EMTPY_STR(parsed_text.lines[current_line].line)) {
            // Empty, must be a split between cards.
            current_line++;
            continue;
        }
        // Start of a card, skip over the next NUM_BINGO_LINES-1 lines;
        bingo_cards->num_cards++;
        current_line += NUM_BINGO_COLUMNS;
    }

    bingo_cards->cards = malloc_b(
                             bingo_cards->num_cards * sizeof(bingo_card_type));

    for (current_card = 0, current_line = 2;
         current_line < parsed_text.num_lines; ) {
        if (IS_EMTPY_STR(parsed_text.lines[current_line].line)) {
            // Empty, must be a split between cards.
            current_line++;
            continue;
        }
        // Start of a card, parse the next NUM_BINGO_LINES lines;
        bingo_cards->cards[current_card].has_line = false;
        for (i = 0; i < NUM_BINGO_LINES; i++) {
            split_text = split_string_on_char(
                                  parsed_text.lines[current_line++].line, ' ');
            parsed_line_ints = parse_text_to_ints(split_text);
            for (j = 0; j< NUM_BINGO_COLUMNS; j++) {
                bingo_cards->cards[current_card].lines[i][j].num =
                                                           parsed_line_ints[j];
                bingo_cards->cards[current_card].lines[i][j].called = false;
            }
            free(parsed_line_ints);
            parsed_line_ints = NULL;
            free_parsed_text(split_text);
        }
        current_card++;
    }

    free_parsed_text(parsed_bingo_text);
}

/*
 * check_off_bingo_number
 *
 * For the given bingo card, check off any squares matching the called number.
 *
 * Argument: bingo_card
 *     Bingo card to check for the called number.
 * Argument: called_num
 *     Number to check off if it is on the bingo card.
 *
 * Return: void
 */
static void
check_off_bingo_number(bingo_card_type *bingo_card, int called_num)
{
    size_t i, j;

    for (i = 0; i < NUM_BINGO_LINES; i++) {
        for (j = 0; j < NUM_BINGO_COLUMNS; j++) {
            if (bingo_card->lines[i][j].num == called_num) {
                bingo_card->lines[i][j].called = true;
            }
        }
    }
}

/*
 * check_for_winning_line
 *
 * Check if any of the given bingo cards have a winning line.
 *
 * Argument: bingo_cards
 *     Array of bingo cards to check. The first card in the array which has a
 *     line will be returned.
 *
 * Return: bingo_card_type *
 */
static bingo_card_type *
check_for_winning_line(bingo_cards_type bingo_cards)
{
    bool             has_line;
    bingo_card_type *first_winner = NULL;
    size_t           i, j, k;

    for (i = 0; i < bingo_cards.num_cards; i++) {
        /* Check for horizontal line */
        for (j = 0; j < NUM_BINGO_LINES; j++) {
            has_line = true;
            for (k = 0; k < NUM_BINGO_COLUMNS; k++) {
                if (!(bingo_cards.cards[i].lines[j][k].called)) {
                    has_line = false;
                }
            }
            if (has_line) {
                bingo_cards.cards[i].has_line = true;
                if (first_winner == NULL) {
                    first_winner = &(bingo_cards.cards[i]);
                }
            }
        }
        /* Check for vertical line */
        if (!(bingo_cards.cards[i].has_line)) {
            for (j = 0; j < NUM_BINGO_COLUMNS; j++) {
                has_line = true;
                for (k = 0; k < NUM_BINGO_LINES; k++) {
                    if (!(bingo_cards.cards[i].lines[k][j].called)) {
                        has_line = false;
                    }
                }
                if (has_line) {
                    bingo_cards.cards[i].has_line = true;
                    if (first_winner == NULL) {
                        first_winner = &(bingo_cards.cards[i]);
                    }
                }
            }
        }
    }

    return (first_winner);
}

/*
 * find_bingo_winner
 *
 * Find the first card to win out of the given cards.
 *
 * Argument: bingo_calls
 *     Bingo calls struct.
 * Argument: bingo_cards
 *     Array of bingo_card structs.
 * Argument: bingo_cards
 *     OUT: Number called which caused the first card to win.
 *
 * Return: bingo_card_type
 */
static bingo_card_type
find_bingo_winner(bingo_calls_type  bingo_calls,
                  bingo_cards_type  bingo_cards,
                  int              *winning_number)
{
    size_t           i, j;
    bingo_card_type *line_winner = NULL;

    /*
     * For each bingo call, check off the called number in all the cards and
     * check for any lines.
     */
    for (i = 0; i < bingo_calls.num_calls; i++) {
        for (j = 0; j < bingo_cards.num_cards; j++) {
            check_off_bingo_number(&(bingo_cards.cards[j]),
                                   bingo_calls.calls[i]);
        }
        line_winner = check_for_winning_line(bingo_cards);
        if (line_winner != NULL) {
            /* Found a line winner */
            *winning_number = bingo_calls.calls[i];
            break;
        }
    }
    /* We should find a winner above */
    assert(line_winner != NULL);

    return (*line_winner);
}

/*
 * find_sum_of_unmarked_numbers
 *
 * Calculate the sum of all the unmarked numbers on the bingo card.
 *
 * Argument: bingo_cards
 *     Bingo card to sum unmarked numbers of.
 *
 * Return: int
 */
static int
find_sum_of_unmarked_numbers(bingo_card_type bingo_card)
{
    size_t i, j;
    int    score = 0;

    for (i = 0; i < NUM_BINGO_LINES; i++) {
        for (j = 0; j < NUM_BINGO_COLUMNS; j++) {
            if (!(bingo_card.lines[i][j].called)) {
                score += bingo_card.lines[i][j].num;
            }
        }
    }

    return (score);
}

/*
 * filter_cards_to_non_winners
 *
 * Filter a bingo cards array to an array of non-winners only.
 *
 * Argument: bingo_cards
 *     Array of bingo_card structs.
 * Argument: non_winners
 *     OUT: Array of bingo_card structs which are non-winners. This must be
 *     freed by the caller.
 *
 * Return: bingo_card_type
 */
static void
filter_cards_to_non_winners(bingo_cards_type  bingo_cards,
                            bingo_cards_type *non_winners)
{
    size_t i, j;

    if (non_winners->cards != NULL) {
        free(non_winners->cards);
        non_winners->cards = NULL;
    }

    non_winners->num_cards = 0;
    for (i = 0; i < bingo_cards.num_cards; i++) {
        if (!(bingo_cards.cards[i].has_line)) {
            non_winners->num_cards++;
        }
    }

    non_winners->cards = malloc_b(
                             non_winners->num_cards * sizeof(bingo_card_type));

    for (i = 0, j = 0; i < bingo_cards.num_cards; i++) {
        if (!(bingo_cards.cards[i].has_line)) {
            non_winners->cards[j++] = bingo_cards.cards[i];
        }
    }
}

/*
 * find_last_winner
 *
 * Find the card which wins last out of the given cards.
 *
 * Argument: bingo_calls
 *     Bingo calls struct.
 * Argument: bingo_cards
 *     Array of bingo_card structs.
 * Argument: bingo_cards
 *     OUT: Number called which caused the last card to win.
 *
 * Return: bingo_card_type
 */
static bingo_card_type
find_last_winner(bingo_calls_type  bingo_calls,
                 bingo_cards_type  bingo_cards,
                 int              *winning_number)
{
    size_t            i, j;
    bingo_cards_type  non_winners;
    bingo_cards_type  non_winners_copy;
    bingo_cards_type  tmp;
    bingo_card_type  *last_winner = NULL;
    bingo_card_type  last_winner_ret;

    /* Create copies of original cards */
    non_winners.cards = NULL;
    non_winners_copy.cards = NULL;
    filter_cards_to_non_winners(bingo_cards, &non_winners);

    for (i = 0; i < bingo_calls.num_calls; i++) {
        /* Create copies of non-winners cards */
        filter_cards_to_non_winners(non_winners, &non_winners_copy);
        for (j = 0; j < non_winners.num_cards; j++) {
            check_off_bingo_number(&(non_winners.cards[j]),
                                   bingo_calls.calls[i]);
        }
        check_for_winning_line(non_winners);
        filter_cards_to_non_winners(non_winners, &non_winners_copy);
        tmp = non_winners_copy;
        non_winners_copy = non_winners;
        non_winners = tmp;
        if (non_winners.num_cards == 1) {
            /* All but one card has won */
            last_winner = &(non_winners.cards[0]);
        } else if (non_winners.num_cards == 0) {
            /* All cards have now won */
            *winning_number = bingo_calls.calls[i];
            break;
        }
    }
    /* All cards should have won above */
    assert(non_winners.num_cards == 0);
    assert(last_winner != NULL);
    last_winner_ret = *last_winner;

    free(non_winners.cards);
    free(non_winners_copy.cards);

    return (last_winner_ret);
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
    bingo_calls_type  bingo_calls;
    bingo_cards_type  bingo_cards;
    bingo_card_type   line_winner;
    int               winning_number;
    bingo_card_type   last_winner;
    int               last_number;
    int               card_score;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

    assert(argc == 2);
    file_name = argv[1];

    parsed_text = parse_file(file_name);

    parse_lines_into_calls_and_cards(parsed_text, &bingo_calls, &bingo_cards);

    line_winner = find_bingo_winner(bingo_calls, bingo_cards, &winning_number);
    card_score = find_sum_of_unmarked_numbers(line_winner);
    printf("Part 1: Winning number = %d, Card score = %d, N*S = %d\n",
           winning_number,
           card_score,
           winning_number * card_score);

    last_winner = find_last_winner(bingo_calls, bingo_cards, &last_number);
    card_score = find_sum_of_unmarked_numbers(last_winner);
    printf("Part 2: Last winning number = %d, Card score = %d, N*S = %d\n",
           last_number,
           card_score,
           last_number * card_score);

    free(bingo_cards.cards);
    bingo_cards.cards = NULL;
    free(bingo_calls.calls);
    bingo_calls.calls = NULL;
    free_parsed_text(parsed_text);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
    print_elapsed_time(end_time.tv_nsec - start_time.tv_nsec, "Runtime");

    return (rc);
}
