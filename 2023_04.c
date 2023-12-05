#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "get_next_line.h"


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 4
#define AOC_YEAR 2023


int main(int argc, char ** argv)
{
    char line[1000 + 1];
    char * lineptr;
    size_t line_size = 0;
    int fd;
    unsigned int have_numbers[101];
    unsigned long x;
    int score = 0, card_score;
    int ncards = 0;
    int card_copies[30] = {0};

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
        card_score = 0;
        /* add this card (well, it was easier than always initializing
         * card_copies to 1) */
        card_copies[0]++;
        memset(have_numbers, 0, sizeof(have_numbers));

        /* read which card we're on */
        lineptr = line + 4;
        strtoul(lineptr, &lineptr, 10);
        lineptr++;  /* skip colon */
        DBGPRINT("Reading have numbers %s\n", lineptr);

        /* read numbers we have */
        x = strtoul(lineptr, &lineptr, 10);
        while(x > 0) {
            if(x > 99) {
                fprintf(stderr,
                        "ERROR: line \"%s\", read number %ld\n",
                        line, x);
                exit(-1);
            }
            have_numbers[(int) x] = 1;
            DBGPRINT("Have number %ld\n", x);
            x = strtoul(lineptr, &lineptr, 10);
        }
        lineptr += 2;  /* skip '|' */

        DBGPRINT("Reading card numbers %s\n", lineptr);
        /* read card numbers */
        x = strtoul(lineptr, &lineptr, 10);
        while(x > 0) {
            if(x > 99) {
                fprintf(stderr,
                        "ERROR: line \"%s\", read number %ld\n",
                        line, x);
                exit(-1);
            }
            if(have_numbers[(int) x]) {
                card_score += 1;
            }
            x = strtoul(lineptr, &lineptr, 10);
        }

        /* calculate score */
        ncards += card_copies[0];
        if(card_score > 0) {
            score += 1 << (card_score - 1);
            for(int i=1; i < card_score + 1; i++)
                card_copies[i] += card_copies[0];
        }
        memmove(card_copies, &card_copies[1],
                sizeof(card_copies) - sizeof(card_copies[0]));
        for(int i=0; i < sizeof(card_copies)/sizeof(card_copies[0]); i++) {
            printf(" %2d", card_copies[i]);
        }
        line_size = get_next_line(fd, line, sizeof(line));
    }

    close(fd);
    printf("Answer: %d\n", score);
    printf("Part 2 answer: %d\n", ncards);
    return 0;
}

