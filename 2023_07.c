#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "get_next_line.h"
#include "linux/list.h"



#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 1
#define AOC_YEAR 2023

#define HAND_SIZE 5
struct hand {
    struct list_head list;
    char cards[5];
    long bid;
    int strength;
};

#ifdef PART2
char cards_order[] = "J23456789TQKA";
#else
char cards_order[] = "23456789TJQKA";
#endif

#define HAND_STRENGTH_5_OF_A_KIND 6
#define HAND_STRENGTH_4_OF_A_KIND 5
#define HAND_STRENGTH_FULL_HOUSE 4
#define HAND_STRENGTH_3_OF_A_KIND 3
#define HAND_STRENGTH_2_PAIR 2
#define HAND_STRENGTH_1_PAIR 1
#define HAND_STRENGTH_HIGH_CARD 0

int calculate_strength(struct hand * hand) {
    char cards[HAND_SIZE];
    int n, firstn = 0;   /* n-of-a-kind */
    int i;
#ifdef PART2
    int njokers = 0;
#endif

    /* scratch space */
    memcpy(cards, hand->cards, HAND_SIZE);

    hand->strength = HAND_STRENGTH_HIGH_CARD;
    for(i=0; i < sizeof(cards); i++) {
        /* if we've already accounted for this card, move on */
        if(cards[i] == 'o')
            continue;
#ifdef PART2
        if(cards[i] == 'J') {
            njokers++;
            continue;
        }
#endif
        n = 1;
        /* how many of this card in the hand? */
        for(int j = i+1; j < sizeof(cards); j++) {
            if(cards[j] == cards[i]) {
                cards[j] = 'o';
                n++;
            }
        }
        switch(n) {
            case 5: hand->strength = HAND_STRENGTH_5_OF_A_KIND;
                    goto done;
                    break;
            case 4: hand->strength = HAND_STRENGTH_4_OF_A_KIND;
                    goto done;
                    break;
            case 3: if(firstn == 2) {  /* full house */
                        hand->strength = HAND_STRENGTH_FULL_HOUSE;
                        goto done;
                    } else {
                        hand->strength = HAND_STRENGTH_3_OF_A_KIND;
                        firstn = 3;
                    }
                    break;
            case 2: if(firstn == 2) {  /* 2 pair */
                        hand->strength = HAND_STRENGTH_2_PAIR;
                        goto done;
                    } else if(firstn == 3) {
                        hand->strength = HAND_STRENGTH_FULL_HOUSE;
                        goto done;
                    } else {
                        hand->strength = HAND_STRENGTH_1_PAIR;
                        firstn = 2;
                    }
                    break;
            default: /* nothing interesting */
                    break;
        }
    }
done:

#ifdef PART2
    for(; i < sizeof(cards); i++) {
        if(hand->cards[i] == 'J')
            njokers++;
    }
    DBGPRINT("Found %d jokers\n", njokers);
    /* yes, this is silly */
    switch(hand->strength) {
        case HAND_STRENGTH_4_OF_A_KIND:
            if(njokers == 1)
                hand->strength = HAND_STRENGTH_5_OF_A_KIND ;
            break;
        case HAND_STRENGTH_3_OF_A_KIND:
            if(njokers == 2)
                hand->strength = HAND_STRENGTH_5_OF_A_KIND;
            else if(njokers == 1)
                hand->strength = HAND_STRENGTH_4_OF_A_KIND;
            break;
        case HAND_STRENGTH_2_PAIR:
            if(njokers == 1)
                hand->strength = HAND_STRENGTH_FULL_HOUSE;
            break;;
        case HAND_STRENGTH_1_PAIR:
            if(njokers == 3)
                hand->strength = HAND_STRENGTH_5_OF_A_KIND;
            else if(njokers == 2)
                hand->strength = HAND_STRENGTH_4_OF_A_KIND;
            else if(njokers == 1)
                hand->strength = HAND_STRENGTH_3_OF_A_KIND;
            break;
        case HAND_STRENGTH_HIGH_CARD:
            if(njokers == 4 || njokers == 5)
                hand->strength = HAND_STRENGTH_5_OF_A_KIND;
            else if(njokers == 3)
                hand->strength = HAND_STRENGTH_4_OF_A_KIND;
            else if(njokers == 2)
                hand->strength = HAND_STRENGTH_3_OF_A_KIND;
            else if(njokers == 1)
                hand->strength = HAND_STRENGTH_1_PAIR;
            break;
        default: break;
    }
#endif

    return hand->strength;
}

void print_hand(const struct hand * h)
{
    DBGPRINT("%*s bid %8ld strength %3d\n",
            HAND_SIZE, h->cards, h->bid, h->strength);
}

int handcmp(struct hand * h0, struct hand * h1) {
    int card_index, h0_index, h1_index;
    if(h0->strength > h1->strength)
        return 1;
    if(h0->strength < h1->strength)
        return -1;

    /* equal strengths, time to compare the hands */
    for(card_index = 0;
            card_index < HAND_SIZE && h0->cards[card_index] == h1->cards[card_index];
            card_index++) ;
    if(card_index >= HAND_SIZE)
        return 0;

    for(h0_index = 0;
            h0->cards[card_index] != cards_order[h0_index] && h0_index < sizeof(cards_order);
            h0_index++);
    for(h1_index = 0;
            h1->cards[card_index] != cards_order[h1_index] && h1_index < sizeof(cards_order);
            h1_index++);
    if(h0_index > h1_index)
        return 1;
    if(h0_index < h1_index)
        return -1;
    return 0;
}

int main(int argc, char ** argv)
{
    char line[1000 + 1];
    size_t line_size = 0;
    int fd;
    long rank = 1, winnings = 0;
    struct list_head hands = LIST_HEAD_INIT(hands);
    struct hand * handptr, * handptr_save;
    int added;

#ifdef PART2
# define PART2_STRING " part 2"
#else
# define PART2_STRING ""
#endif
    if(argc != 2) {
        DBGPRINT("Usage: %s <input file>\n", argv[0]);
        DBGPRINT("  solves Advent of Code %d day %d%s\n",
                AOC_YEAR, AOC_DAY, PART2_STRING);
        return -1;
    };

    /* read each line */
    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    line_size = get_next_line(fd, line, sizeof(line));
    while(line_size > 0) {
        handptr = (struct hand *) malloc(sizeof(*handptr));
        if(!handptr) {
            fprintf(stderr, "Malloc error\n");
            exit(-1);
        }
        memcpy(handptr->cards, line, sizeof(handptr->cards));
        handptr->bid = strtoul(line + sizeof(handptr->cards), NULL, 10);
        calculate_strength(handptr);

        /* add sorted */
        added = 0;
        list_for_each_entry(handptr_save, &hands, list) {
            if(handcmp(handptr, handptr_save) > 0) {
                list_add_tail(&handptr->list, &handptr_save->list);
                added = 1;
                break;
            }
        }
        if(!added)
            list_add_tail(&handptr->list, &hands);
        print_hand(handptr);
        line_size = get_next_line(fd, line, sizeof(line));
    }

    /* sum hands by rank */
    rank = 1;
    winnings = 0;
    DBGPRINT("---------------------------------------\n");

    list_for_each_entry_safe_reverse(handptr, handptr_save, &hands, list) {
        winnings += rank * handptr->bid;
        print_hand(handptr);
        free(handptr);
        rank++;
    }

    printf("Answer: %ld\n", winnings);

    close(fd);
    return 0;
}

