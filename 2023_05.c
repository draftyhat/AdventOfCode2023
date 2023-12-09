#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "get_next_line.h"

/* TODO: segfault in part 2. Passes provided test case. */


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 1
#define AOC_YEAR 2023


int main(int argc, char ** argv)
{
    char line[1000 + 1];
    char * lineptr;
    ssize_t line_size = 0;
    int fd;
#ifdef PART2
    long seeds[1000], next_seeds[1000], smallest_seed;
    long seed_range_lengths[1000], next_seed_range_lengths[1000];
    int nnext_seeds;
#else
    long seeds[30], next_seeds[30], smallest_seed;
#endif
    int nseeds = 0;
    long destination_range_start, source_range_start, range_length;

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

    /* read seeds */
    line_size = get_next_line(fd, line, sizeof(line)) - 1;
    if(strncmp("seeds: ", line, sizeof("seeds: ") - 1) != 0) {
        fprintf(stderr, "ERROR: unrecognized seeds line\n");
        exit(-1);
    }
    lineptr = line + sizeof("seeds: ") - 1;
    while(lineptr < (line + line_size)) {
        if(nseeds > sizeof(seeds)/sizeof(seeds[0])) {
            fprintf(stderr, "ERROR: found more than %d seeds\n",
                    nseeds);
            exit(-1);
        }
        seeds[nseeds] = strtoul(lineptr, &lineptr, 10);
#ifdef PART2
        seed_range_lengths[nseeds] = strtoul(lineptr, &lineptr, 10);
#endif
        nseeds++;
    }

    DBGPRINT("seeds:");
    for(int i=0; i < nseeds; i++) {
        DBGPRINT(" %ld", seeds[i]);
#ifdef PART2
        DBGPRINT(":%ld", seed_range_lengths[i]);
#endif
    }
    DBGPRINT("\n");

    memcpy(next_seeds, seeds, sizeof(seeds));
#ifdef PART2
    memcpy(next_seed_range_lengths, seed_range_lengths,
            sizeof(seed_range_lengths));
    nnext_seeds = nseeds;
#endif
    for(
        line_size = get_next_line(fd, line, sizeof(line));
        line_size > 0;
        line_size = get_next_line(fd, line, sizeof(line))) {
        if(line_size == 0)   /* blank line */
            continue;

        /* check for map line */
        if(strcmp("map:", &line[line_size - sizeof("map:")]) == 0) {
            DBGPRINT("Reading %s\n", line);
            memcpy(seeds, next_seeds, sizeof(seeds));
#ifdef PART2
            memcpy(seed_range_lengths, next_seed_range_lengths,
                    sizeof(next_seed_range_lengths));
            nseeds = nnext_seeds;
#endif
            DBGPRINT("seeds: ");
            for(int i=0; i < nseeds; i++) {
                DBGPRINT(" %ld", seeds[i]);
#ifdef PART2
                DBGPRINT(":%ld", seed_range_lengths[i]);
#endif
            }
            DBGPRINT("\n");
            continue;
        }

        /* read map */
        destination_range_start = strtoul(line, &lineptr, 10);
        source_range_start = strtoul(lineptr, &lineptr, 10);
        range_length = strtoul(lineptr, &lineptr, 10);

        /* process each of our seeds through the map */
        for(int i = 0; i < nseeds; i++) {
#ifdef PART2
            if(seeds[i] < source_range_start) {
                if(seeds[i] + seed_range_lengths[i] >= source_range_start) {
                    /* some of this range is before the source range, some
                     * inside, and maybe some after */
                    /*  part of the range before the source range */
                    DBGPRINT("  original %ld:%ld\n",
                            seeds[i], seed_range_lengths[i]);
                    DBGPRINT("  transform range %ld:%ld -> %ld\n",
                            source_range_start, range_length,
                            destination_range_start);
                    next_seed_range_lengths[i] = source_range_start - seeds[i];
                    DBGPRINT("    area before the transform range %ld:%ld\n",
                            next_seeds[i], next_seed_range_lengths[i]);
                    if(seeds[i] + seed_range_lengths[i] < source_range_start + range_length) {
                        /* no parts of this range after the source range */
                        /*  part of the range inside the source range */
                        next_seeds[nnext_seeds] = destination_range_start;
                        next_seed_range_lengths[nnext_seeds] = seed_range_lengths[i] - (source_range_start - seeds[i]);
                        nnext_seeds++;
#if 0
                        DBGPRINT("range %ld:%ld -> %ld:%ld, %ld:%ld\n",
                                seeds[i], seed_range_lengths[i],
                                next_seeds[i], next_seed_range_lengths[i],
                                next_seeds[nnext_seeds - 1],
                                next_seed_range_lengths[nnext_seeds - 1]);
                        DBGPRINT("  next range length calculation:\n");
                        DBGPRINT("     range length %ld\n", range_length);
                        DBGPRINT("     source_range_start %ld\n", source_range_start);
                        DBGPRINT("     seeds[i] %ld\n", seeds[i]);
                        DBGPRINT("     source_range_start - seeds[i] %ld\n", source_range_start - seeds[i]);
#endif
                    } else {
                        /* this range consumes the source range */
                        /*  the source range */
                        next_seeds[nnext_seeds] = destination_range_start;
                        next_seed_range_lengths[nnext_seeds] = range_length;
                        nnext_seeds++;
DBGPRINT("    area intersecting %ld:%ld\n", next_seeds[nnext_seeds - 1], next_seed_range_lengths[nnext_seeds - 1]);
                        /*  part of the range after the source range */
                        next_seeds[nnext_seeds] = source_range_start + range_length;
                        next_seed_range_lengths[nnext_seeds] = seeds[i] + seed_range_lengths[i] - (source_range_start + range_length);
                        nnext_seeds++;
DBGPRINT("    area to right %ld:%ld\n", next_seeds[nnext_seeds - 1], next_seed_range_lengths[nnext_seeds - 1]);
                    }
                } /* else the last seed in this range is below the source range */
            } else {
                if(seeds[i] < source_range_start + range_length) {
                    /* first seed is in source range */
                    if(seeds[i] + seed_range_lengths[i] < source_range_start + range_length) {
                        /* this range is entirely in source range */
                        next_seeds[i] = seeds[i] - source_range_start
                            + destination_range_start;
                    } else {
                        /* this range goes beyond end of source range */
                        /*  part of the range within the source range */
                        next_seeds[i] = destination_range_start + (seeds[i] - source_range_start);
                        next_seed_range_lengths[i] = source_range_start + range_length - seeds[i];
DBGPRINT("  range %ld:%ld  intersecting and after %ld:%ld, dest %ld\n",
        seeds[i], seed_range_lengths[i], source_range_start, range_length, destination_range_start);
DBGPRINT("    area inside %ld:%ld\n", next_seeds[i], next_seed_range_lengths[i]);
                        /*  part of the range after the source range */
                        next_seeds[nnext_seeds] = source_range_start + range_length;
                        next_seed_range_lengths[nnext_seeds] =
                            (seeds[i] + seed_range_lengths[i])
                            - (source_range_start + range_length);
                        nnext_seeds++;
DBGPRINT("    area on top %ld:%ld\n", next_seeds[nnext_seeds - 1], next_seed_range_lengths[nnext_seeds - 1]);
                    }
                } /* else first seed is beyond range */
            }
#else
            if(seeds[i] >= source_range_start &&
                    seeds[i] < source_range_start + range_length) {
                next_seeds[i] = seeds[i] - source_range_start +
                    destination_range_start;
            }
#endif
        }
    }

    DBGPRINT("%i seeds: ", nseeds);
    for(int i=0; i < nseeds; i++) {
        DBGPRINT("[%d]: %ld", i, next_seeds[i]);
#ifdef PART2
        DBGPRINT(":%ld\n", seed_range_lengths[i]);
#endif
    }
    DBGPRINT("\n");

    smallest_seed = next_seeds[0];
    for(int i=1; i < nseeds; i++) {
        if(next_seeds[i] < smallest_seed)
            smallest_seed = next_seeds[i];
    }

    printf("Answer: %ld\n", smallest_seed);

    close(fd);
    return 0;
}

