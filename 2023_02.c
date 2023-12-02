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

#define AOC_DAY 1
#define AOC_YEAR 2023

struct game {
    int id;
    int red;
    int green;
    int blue;
};

int parse_game_line(
        int * game_id,
        long * power,
        const char * line,
        const int line_size,
        const struct game * desired_game)
{
    const char * lineptr;
    char * ncubes_end;
    long ncubes;
    struct game required_game = {0};

    /* parse game ID */
    if(strncmp(line, "Game ", sizeof("Game ")) - 1 != 0) {
        DBGPRINT("ERROR parsing game line \"%s\"\n", line);
        return -1;
    }
    lineptr = line + sizeof("Game ") - 1;
    *game_id = atoi(lineptr);
    /* skip game ID digits */
    while(lineptr[0] >= '0' && lineptr[0] < '9')
        lineptr++;
    DBGPRINT("Parsing game %d\n", *game_id);

    /* read each turn in the game and evaluate against desired */
    for(lineptr++; lineptr < line + line_size; lineptr++) {
        ncubes = strtoul(lineptr, &ncubes_end, 10);
        DBGPRINT("line \"%s\" found %d cubes\n", lineptr, ncubes);
        if(ncubes > 0) {
            /* found a number of cubes. Skip to cube color. */
            lineptr = ncubes_end + 1;

            /* figure out cube color */
            if(strncmp(lineptr, "red", sizeof("red") - 1) == 0) {
                /* check against desired */
                if(ncubes > desired_game->red) {
                    DBGPRINT("game %d needs %d red cubes, but have only %d\n",
                            game_id, ncubes, desired_game->red);
                    *game_id = 0;
                }
                /* check against required */
                if(ncubes > required_game.red)
                    required_game.red = ncubes;
                /* go to next */
                lineptr += sizeof("red");
            }
            if(strncmp(lineptr, "green", sizeof("green") - 1) == 0) {
                if(ncubes > desired_game->green) {
                    DBGPRINT("game %d needs %d green cubes, but have only %d\n",
                            game_id, ncubes, desired_game->green);
                    *game_id = 0;
                }
                /* check against required */
                if(ncubes > required_game.green)
                    required_game.green = ncubes;
                /* go to next */
                lineptr += sizeof("green");
            }
            if(strncmp(lineptr, "blue", sizeof("blue") - 1) == 0) {
                if(ncubes > desired_game->blue) {
                    DBGPRINT("game %d needs %d blue cubes, but have only %d\n",
                            game_id, ncubes, desired_game->blue);
                    *game_id = 0;
                }
                /* check against required */
                if(ncubes > required_game.blue)
                    required_game.blue = ncubes;
                /* go to next */
                lineptr += sizeof("blue");
            }
        }
    }

    *power = required_game.red * required_game.blue * required_game.green;

    return 0;
}

int main(int argc, char ** argv)
{
    char line[1000 + 1];
    size_t line_size = 0;
    int fd;
    struct game desired_game = {
        .red = 12,
        .green = 13,
        .blue = 14
    };
    int this_game_sum, game_sum = 0;
    long this_game_power, power_sum = 0;

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
        close(fd);
        return -1;
    }

    /* read games */
    line_size = get_next_line(fd, line, sizeof(line));
    while(line_size > 0) {
        parse_game_line(&this_game_sum, &this_game_power,
                line, line_size, &desired_game);
        game_sum += this_game_sum;
        power_sum += this_game_power;
        line_size = get_next_line(fd, line, sizeof(line));
    }

    printf("Answer: %d\n", game_sum);
    printf("Part 2 answer: %ld\n", power_sum);

    close(fd);
    return 0;
}

