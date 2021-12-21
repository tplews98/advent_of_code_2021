/*
 * AoC 2021 Day 16 solution
 */

#include "utils.h"

/*
 * packet_type
 *
 * Element: version
 *     Version ID of the packet.
 * Element: type_id
 *     Type ID of the packet.
 * Element: length_type_id
 *     Length type ID of the packet. Should only be used if the packet is an
 *     operator type. Set to 0 for literal type packets.
 * Element: length_value
 *     Length value for the corresponding length type. Should only be used if
 *     the packet is an operator type. Set to 0 for literal type packets.
 * Element: literal_value
 *     Literal value of the packet. Should only be used if the packet is a
 *     literal type. Set to 0 for operator type packets.
 * Element: sub_packets
 *     Array of subpackets contained within this packet. Will be NULL if there
 *     are none.
 * Element: num_subpackets
 *     Number of packets in the subpackets array.
 */
typedef struct Packet {
    uint8_t        version;
    uint8_t        type_id;
    uint8_t        length_type_id;
    uint16_t       length_value;
    size_t         literal_value;
    struct Packet *sub_packets;
    size_t         num_subpackets;
} packet_type;

/*
 * free_packet_type
 *
 * Free the memory allocated by a packet_type struct.
 *
 * Argument: packet
 *     Packet struct to free the memory of.
 *
 * Return: void
 */
static void
free_packet_type(packet_type packet)
{
    uint16_t i;

    if (packet.sub_packets != NULL) {
        for (i = 0; i < packet.num_subpackets; i++) {
            free_packet_type(packet.sub_packets[i]);
        }
        free(packet.sub_packets);
        packet.sub_packets = NULL;
    }
}

/*
 * parse_packet
 *
 * Parse a string of binary digits into a packet.
 *
 * Argument: binary_str
 *     String of binary digits to parse.
 * Argument: is_subpacket
 *     Whether the packet beginning at the start of the string is a subpacket
 *     of another packet or not. This is used in recursive calls of this
 *     function and should be called with `false` by the user.
 * Argument: remaining_str
 *     Out pointer to the remaining part of the binary string after the packet
 *     has been parsed. This is used in recursive calls of this function and
 *     should be called with `NULL` by the user.
 *
 * Return: packet_type
 */
static packet_type
parse_packet(char *binary_str, bool is_subpacket, char **remaining_str)
{
    char         current_str[16] = "";
    char        *binary_str_start = NULL;
    char        *subpacket_start = NULL;
    bool         last_literal_group;
    size_t       i;
    bool         is_all_zeros_remaining;
    packet_type  packet;

    packet.version = 0;
    packet.type_id = 0;
    packet.length_type_id = 0;
    packet.length_value = 0;
    packet.literal_value = 0;
    packet.sub_packets = NULL;
    packet.num_subpackets = 0;

    binary_str_start = binary_str;

    /* First three bits are the version id */
    memset(current_str, '\0', 16);
    strncpy(current_str, binary_str, 3);
    packet.version = strtol(current_str, NULL, 2);
    binary_str += 3;

    /* Next three bits are type id */
    memset(current_str, '\0', 16);
    strncpy(current_str, binary_str, 3);
    packet.type_id = strtol(current_str, NULL, 2);
    binary_str += 3;

    /* Next bits depend on the type id */
    switch (packet.type_id) {
        case 4:
            /*
             * Literal type. Values are in groups of 5. First bit determines if
             * this is the last group or not, the next 4 are the bits of the
             * literal value.
             */
            last_literal_group = false;
            while (!last_literal_group) {
                last_literal_group = (binary_str[0] == '0');
                packet.literal_value = (packet.literal_value << 1)
                                       + binary_str[1] - '0';
                packet.literal_value = (packet.literal_value << 1)
                                       + binary_str[2] - '0';
                packet.literal_value = (packet.literal_value << 1)
                                       + binary_str[3] - '0';
                packet.literal_value = (packet.literal_value << 1)
                                       + binary_str[4] - '0';
                binary_str += 5;
            }
            break;
        default:
            /* Operator type. Next bit is the length type id */
            packet.length_type_id = binary_str[0] - '0';
            binary_str++;

            if (packet.length_type_id == 0) {
                /*
                 * Next 15 bits represent the total number of bits in the
                 * sub-packets contained by this packet.
                 */
                memset(current_str, '\0', 16);
                strncpy(current_str, binary_str, 15);
                packet.length_value = strtol(current_str, NULL, 2);
                binary_str += 15;

                /* Parse the subpackets */
                subpacket_start = binary_str;
                while (subpacket_start - binary_str < packet.length_value) {
                    packet.sub_packets = realloc_b(
                                                  packet.sub_packets,
                                                  sizeof(packet_type)
                                                  * ++(packet.num_subpackets));
                    packet.sub_packets[packet.num_subpackets - 1] =
                         parse_packet(subpacket_start, true, &subpacket_start);
                }
                binary_str = subpacket_start;
            } else {
                /*
                 * Next 11 bits represent the number of sub-packets contained
                 * by this packet.
                 */
                memset(current_str, '\0', 16);
                strncpy(current_str, binary_str, 11);
                packet.length_value = strtol(current_str, NULL, 2);
                binary_str += 11;

                /* Parse the subpackets */
                packet.num_subpackets = packet.length_value;
                packet.sub_packets = malloc_b(sizeof(packet_type)
                                              * packet.num_subpackets);
                for (i = 0; i < packet.num_subpackets; i++) {
                    packet.sub_packets[i] = parse_packet(
                                                binary_str, true, &binary_str);
                }
            }
            break;
    }

    if (!is_subpacket) {
        /*
         * Packet will have padding added to end to have a multiple of 4 bits
         */
        while ((binary_str - binary_str_start) % 4 != 0) {
            binary_str++;
        }
        /*
         * If binary string is only remaining zeros, this is padding which is
         * not part of any packet that can be ignored.
         */
        is_all_zeros_remaining = true;
        for (i = 0; i < strlen(binary_str) && is_all_zeros_remaining; i++) {
            is_all_zeros_remaining = (binary_str[i] == '0');
        }
        if (is_all_zeros_remaining) {
            /* Is all zeros remaining, skip */
            binary_str[0] = '\0';
        }
    }

    if (remaining_str != NULL) {
        if (binary_str[0] == '\0') {
            /* Reached end of binary string, all packets parsed */
            *remaining_str = NULL;
        } else {
            *remaining_str = binary_str;
        }
    }

    return (packet);
}

/*
 * find_sum_of_version_numbers_in_packet
 *
 * Find the sum of the version ids for this given packet and all its
 * subpackets.
 *
 * Argument: packet
 *     Packet to find the sum of the version ids for.
 *
 * Return: size_t
 */
static size_t
find_sum_of_version_numbers_in_packet(packet_type packet)
{
    size_t i;
    size_t version_number_sum = 0;

    version_number_sum += packet.version;
    for (i = 0; i < packet.num_subpackets; i++) {
        version_number_sum += find_sum_of_version_numbers_in_packet(
                                                        packet.sub_packets[i]);
    }

    return (version_number_sum);
}

/*
 * find_value_of_packet
 *
 * Find the value of the packet (considering all its subpackets) according to
 * the value rules determined by the packet type id.
 *
 * Argument: packet
 *     Packet to find the value of.
 *
 * Return: size_t
 */
static size_t
find_value_of_packet(packet_type packet)
{
    size_t i;
    size_t packet_value = 0;
    size_t subpacket_value;

    switch (packet.type_id) {
        case 0:
            /* Value is sum of all subpacket values */
            assert(packet.num_subpackets > 0);
            for (i = 0; i < packet.num_subpackets; i++) {
                packet_value += find_value_of_packet(packet.sub_packets[i]);
            }
            break;
        case 1:
            /* Value is product of all subpacket values */
            assert(packet.num_subpackets > 0);
            packet_value = 1;
            for (i = 0; i < packet.num_subpackets; i++) {
                packet_value *= find_value_of_packet(packet.sub_packets[i]);
            }
            break;
        case 2:
            /* Value is minimum of all subpacket values */
            assert(packet.num_subpackets > 0);
            packet_value = find_value_of_packet(packet.sub_packets[0]);
            for (i = 1; i < packet.num_subpackets; i++) {
                subpacket_value = find_value_of_packet(packet.sub_packets[i]);
                if (subpacket_value < packet_value) {
                    packet_value = subpacket_value;
                }
            }
            break;
        case 3:
            /* Value is maximum of all subpacket values */
            assert(packet.num_subpackets > 0);
            packet_value = find_value_of_packet(packet.sub_packets[0]);
            for (i = 1; i < packet.num_subpackets; i++) {
                subpacket_value = find_value_of_packet(packet.sub_packets[i]);
                if (subpacket_value > packet_value) {
                    packet_value = subpacket_value;
                }
            }
            break;
        case 4:
            /* Value is the literal value of the packet */
            assert(packet.num_subpackets == 0);
            packet_value = packet.literal_value;
            break;
        case 5:
            /*
             * Value is 1 if the first subpacket has a value greater than the
             * second subpacket, 0 otherwise.
             */
            assert(packet.num_subpackets == 2);
            packet_value = find_value_of_packet(packet.sub_packets[0]);
            subpacket_value = find_value_of_packet(packet.sub_packets[1]);
            packet_value = (packet_value > subpacket_value);
            break;
        case 6:
            /*
             * Value is 1 if the first subpacket has a value less than the
             * second subpacket, 0 otherwise.
             */
            assert(packet.num_subpackets == 2);
            packet_value = find_value_of_packet(packet.sub_packets[0]);
            subpacket_value = find_value_of_packet(packet.sub_packets[1]);
            packet_value = (packet_value < subpacket_value);
            break;
        case 7:
            /*
             * Value is 1 if the first subpacket has a value equal to the
             * second subpacket, 0 otherwise.
             */
            assert(packet.num_subpackets == 2);
            packet_value = find_value_of_packet(packet.sub_packets[0]);
            subpacket_value = find_value_of_packet(packet.sub_packets[1]);
            packet_value = (packet_value == subpacket_value);
            break;
        default:
            assert(false);
            break;
    }

    return (packet_value);
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
    char             *binary_str = NULL;
    packet_type       packet;

    parsed_text = parse_file(file_name);

    binary_str = hex_str_to_binary_str(parsed_text.lines[0].line);

    packet = parse_packet(binary_str, false, NULL);

    if (print_output) {
        printf("Part 1: Sum of all version numbers = %zu\n",
               find_sum_of_version_numbers_in_packet(packet));
    }

    if (print_output) {
        printf("Part 2: Value of packet = %zu\n",
               find_value_of_packet(packet));
    }

    free_packet_type(packet);
    free(binary_str);

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
