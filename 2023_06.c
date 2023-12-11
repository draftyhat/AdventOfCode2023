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

#define AOC_DAY 6
#define AOC_YEAR 2023


/* time units: milliseconds
 * distance units: millimeters */
#define SPEED_INCREASE_PER_TIME 1

int main(int argc, char ** argv)
{
    char line[1000 + 1], * lineptr;
    size_t line_size = 0;
    int fd;
    long margin_of_error = 1;
    long times[10];
    long distances[10];
    int nraces = 0;
    long part2_time, part2_distance;

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

    /* read input file */
    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    /* read Time: line */
    line_size = get_next_line(fd, line, sizeof(line));
    lineptr = line + sizeof("Time:");
    while(lineptr < line + line_size - 1) {
        times[nraces] = strtoul(lineptr, &lineptr, 10);
        nraces++;
    }
    /*  read part 2 time */
    part2_time = 0;
    DBGPRINT("line \"%s\"\n", line);
    for(lineptr = line + sizeof("Time:");
            lineptr < line + line_size - 1;
            lineptr++) {
        if(lineptr[0] >= '0' && lineptr[0] <= '9') {
            part2_time *= 10;
            part2_time += (long) (lineptr[0] - '0');
            DBGPRINT("Found %c, adding %ld, now %ld\n", lineptr[0],
                    (long) (lineptr[0] - '0'), part2_time);
        }
    }

    /* read Distance: line */
    line_size = get_next_line(fd, line, sizeof(line));
    lineptr = line + sizeof("Distance:");
    for(int i = 0; i < nraces; i++) {
        distances[i] = strtoul(lineptr, &lineptr, 10);
    }
    /*  read part 2 distance */
    part2_distance = 0;
    for(lineptr = line + sizeof("Distance:");
            lineptr < line + line_size - 1;
            lineptr++) {
        if(lineptr[0] >= '0' && lineptr[0] <= '9') {
            part2_distance *= 10;
            part2_distance += (long) (lineptr[0] - '0');
        }
    }

    for(int i = 0; i < nraces; i++) {
        DBGPRINT("Race %d: time %ld record %ld\n", i, times[i], distances[i]);
        /* find first winning hold_time */
        for(long hold_time = 0; hold_time < (times[i]/2); hold_time++) {
            /* does this hold_time win? */
            if(hold_time * (times[i] - hold_time) > distances[i]) {
                DBGPRINT("Race %d found first winning hold_time %ld; nraces to win %ld\n",
                        i, hold_time, times[i] + 1 - hold_time * 2);
                /* times[i] + 1 adds in 0 hold time scenario */
                margin_of_error *= (times[i] + 1 - hold_time * 2);
                break;
            }
        }
    }
    printf("Part 1 answer: %ld\n", margin_of_error);

    DBGPRINT("Part 2 race: time %ld record %ld\n", part2_time, part2_distance);
    for(long hold_time = 0; hold_time < (part2_time/2); hold_time++) {
        /* does this hold_time win? */
        if(hold_time * (part2_time - hold_time) > part2_distance) {
            DBGPRINT("Part 2 race found first winning hold_time %ld; nraces to win %ld\n",
                    hold_time, part2_time + 1 - hold_time * 2);
            /* part2_time + 1 adds in 0 hold time scenario */
            margin_of_error = (part2_time + 1 - hold_time * 2);
            break;
        }
    }

    printf("Part 2 answer: %ld\n", margin_of_error);

    close(fd);
    return 0;
}

