#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chargrid.h"

/* part 2: weight never hits 64, as claimed in the example */


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 14
#define AOC_YEAR 2023

#define ROCK 'O'
#define EMPTY '.'
#define CUBE '#'

enum {
    NORTH = 0,
    WEST = 1,
    SOUTH = 2,
    EAST = 3
};

long roll_rocks_north(struct chargrid * g)
{
    int top_empty;
    long weight = 0;

    /* for each column */
    for(int x = 0; x < g->width; x++) {
        top_empty = g->height;
        /* for each row */
        for(int y = 0; y < g->height; y++) {
            DBGPRINT("  %d,%d\n", x, y);
            switch (chargrid_get_value(g, x, y)) {
                case ROCK: /* roll it! */
                    DBGPRINT("   ROCK\n");
                    if(top_empty < y) {
                        DBGPRINT("   ROLLING\n");
                        chargrid_set_value(g, x, top_empty, ROCK);
                        chargrid_set_value(g, x, y, EMPTY);
                        weight += g->height - top_empty;
                        top_empty++;
                    } else {
                        top_empty = y + 1;
                        weight += g->height - y;
                    }
                    break;
                case CUBE:
                    top_empty = y + 1;
                    break;
                case EMPTY:
                    if(top_empty >= y)
                        top_empty = y;
                    break;
                default:
                    break;
            }
        }
    }
    return weight;
}

void roll_rocks_south(struct chargrid * g)
{
    int bottom_empty;

    /* for each column */
    for(int x = 0; x < g->width; x++) {
        bottom_empty = -1;
        /* for each row */
        for(int y = g->height - 1; y >= 0; y--) {
            DBGPRINT("  %d,%d\n", x, y);
            switch (chargrid_get_value(g, x, y)) {
                case ROCK: /* roll it! */
                    DBGPRINT("   ROCK\n");
                    if(bottom_empty > y) {
                        DBGPRINT("   ROLLING\n");
                        chargrid_set_value(g, x, bottom_empty, ROCK);
                        chargrid_set_value(g, x, y, EMPTY);
                        bottom_empty--;
                    } else {
                        bottom_empty = y - 1;
                    }
                    break;
                case CUBE:
                    bottom_empty = y - 1;
                    break;
                case EMPTY:
                    if(bottom_empty <= y)
                        bottom_empty = y;
                    break;
                default:
                    break;
            }
        }
    }
}

long roll_rocks_east(struct chargrid * g)
{
    int east_empty;
    long weight = 0;

    /* for each row */
    for(int y = 0; y < g->width; y++) {
        east_empty = 0;
        /* for each spot in that row */
        for(int x = g->width - 1; x >= 0; x--) {
            DBGPRINT("  %d,%d\n", x, y);
            switch (chargrid_get_value(g, x, y)) {
                case ROCK: /* roll it! */
                    DBGPRINT("   ROCK\n");
                    if(east_empty > x) {
                        DBGPRINT("   ROLLING\n");
                        chargrid_set_value(g, east_empty, y, ROCK);
                        chargrid_set_value(g, x, y, EMPTY);
                        east_empty--;
                    } else {
                        east_empty = x - 1;
                    }
                    weight += g->width - y;
                    break;
                case CUBE:
                    east_empty = x - 1;
                    break;
                case EMPTY:
                    if(east_empty <= x)
                        east_empty = x;
                    break;
                default:
                    break;
            }
        }
    }
    return weight;
}

void roll_rocks_west(struct chargrid * g)
{
    int west_empty;

    /* for each row */
    for(int y = 0; y < g->width; y++) {
        west_empty = g->width;
        /* for each spot in that row */
        for(int x = 0; x < g->width; x++) {
            DBGPRINT("  %d,%d\n", x, y);
            switch (chargrid_get_value(g, x, y)) {
                case ROCK: /* roll it! */
                    DBGPRINT("   ROCK\n");
                    if(west_empty < x) {
                        DBGPRINT("   ROLLING\n");
                        chargrid_set_value(g, west_empty, y, ROCK);
                        chargrid_set_value(g, x, y, EMPTY);
                        west_empty++;
                    } else {
                        west_empty = x + 1;
                    }
                    break;
                case CUBE:
                    west_empty = x + 1;
                    break;
                case EMPTY:
                    if(west_empty >= x)
                        west_empty = x;
                    break;
                default:
                    break;
            }
        }
    }
}

long roll_rocks_part2(struct chargrid * g) {
    roll_rocks_north(g);
    roll_rocks_west(g);
    roll_rocks_south(g);
    return roll_rocks_east(g);
}

int main(int argc, char ** argv)
{
    int fd;
    struct chargrid g;
    long answer;
#define MIN_CYCLE_START 200
#define MIN_CYCLE_LENGTH 100
#define LONGEST_CYCLE 2000
    long cycle_answer[LONGEST_CYCLE];

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

    read_chargrid_by_line(fd, &g, NULL);
    close(fd);

    answer = roll_rocks_north(&g);
    printf("Part 1 answer: %ld\n", answer);

    /* finish 1st part2 cycle */
    roll_rocks_west(&g);
    roll_rocks_south(&g);
    cycle_answer[0] = roll_rocks_east(&g);

    long ncycles = 1000000000;
    long cycle_length;
    for(long cycle = 1; cycle < LONGEST_CYCLE; cycle++) {
        cycle_answer[cycle] = answer = roll_rocks_part2(&g);
        if((cycle - MIN_CYCLE_START) & 1 == 1)
            continue;
        cycle_length = (cycle - MIN_CYCLE_START)/2;
        /* check if cycle from MIN_CYCLE_START, of length cycle_length,
         * == cycle from MIN_CYCLE_START + cycle_length to cycle */
        if(cycle_length > MIN_CYCLE_LENGTH &&
                answer == cycle_answer[MIN_CYCLE_START + cycle_length]) {
            long i;
            for(i = 0; i < cycle_length; i++) {
                if(cycle_answer[MIN_CYCLE_START + i] != cycle_answer[
                       MIN_CYCLE_START + cycle_length + i]) {
                   break; 
                }
                printf("[%ld] == %ld, [%ld] == %ld\n",
                        MIN_CYCLE_START + i, cycle_answer[MIN_CYCLE_START + i],
                        cycle_length + MIN_CYCLE_START + i,
                        cycle_answer[MIN_CYCLE_START + cycle_length + i]);
            }
            if(i == cycle_length) {
                printf("FOUND CYCLE %ld at %ld length %ld\n", MIN_CYCLE_START, cycle, cycle_length);
                for(int i = 0; i < cycle_length; i++) {
                    printf(" %ld", cycle_answer[MIN_CYCLE_START + i]);
                }
                printf("\n");

                /* calcuate value at ncycles cycles */
                printf("offset into cycles: %ld\n", (ncycles - MIN_CYCLE_START) % cycle_length);
                printf("Part2 answer: %ld\n",
                    cycle_answer[MIN_CYCLE_START + ((ncycles - MIN_CYCLE_START) % cycle_length) - 1]);

                return 0;
            }
        }
    }
    printf("Part2 answer: %ld\n", answer);

    free_chargrid(&g);

    return 0;
}

