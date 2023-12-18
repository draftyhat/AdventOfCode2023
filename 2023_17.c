#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include "grid.h"
#include "location.h"

#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 17
#define AOC_YEAR 2023

#define MAX_LINE_LENGTH 3

struct vector17 {
    struct location loc;
    int dir;
    int straight_steps;
};

int traverse_city(
        struct grid * map,
        int start_x,
        int start_y,
        int end_x,
        int end_y,
        int max_straight_steps)
{
    struct grid min_heat;
#define STACK_SIZE 100
    struct vector17 stack[STACK_SIZE], this_loc;
    struct vector17 * stackptr = &stack[0], * stack_end = &stack[STACK_SIZE - 2];
    int answer;
    struct location next_loc;
    int current_heat;

    stackptr->loc.x = start_x;
    stackptr->loc.y = start_y;
    stackptr->dir = EAST;
    stackptr->straight_steps = 0;

    create_grid(&min_heat, map->width, map->height, INT_MAX);
    grid_set_value(&min_heat, start_x, start_y, 0);

    while(stackptr >= stack) {
        memcpy(&this_loc, stackptr, sizeof(this_loc));
        current_heat = grid_get_value(&min_heat,
                this_loc.loc.x, this_loc.loc.y);
        DBGPRINT("at %d,%d going %d, heat %d\n", this_loc.loc.x, this_loc.loc.y,
                this_loc.dir, current_heat);
        stackptr--;
        /* process next step for vector on top of the stack */
        for(int next_dir = 0; next_dir < 4; next_dir++) {
            /* if we aren't coming from that direction */
            if((this_loc.dir != (next_dir + 4) % 2)
                    /* and that wouldn't take us off the map */
                    && go_direction_with_bounds(&next_loc,
                        &this_loc.loc, next_dir, 1,
                        map->width, map->height)
                    /* and haven't gone straight too much */
                    && !((this_loc.dir == next_dir)
                        && (this_loc.straight_steps >= max_straight_steps))
                    /* and we haven't already traversed that
                     * square with less heat */
                    && current_heat + grid_get_value(map,
                        next_loc.x, next_loc.y) < grid_get_value(&min_heat,
                            next_loc.x, next_loc.y)) {
                /* go! */
                stackptr++;
                stackptr->loc.x = next_loc.x;
                stackptr->loc.y = next_loc.y;
                stackptr->dir = next_dir;
                if(next_dir == this_loc.dir)
                    stackptr->straight_steps++;
                else
                    stackptr->straight_steps = 1;
                grid_set_value(&min_heat, next_loc.x, next_loc.y,
                        current_heat + grid_get_value(map, next_loc.x, next_loc.y));
                DBGPRINT("  going %d to %d,%d next_heat %d\n",
                        next_dir, next_loc.x, next_loc.x, 
                        current_heat + grid_get_value(map, next_loc.x, next_loc.y));
            }
        }
        if(stackptr >= stack_end) {
            fprintf(stderr, "ERROR: blew out stack\n");
            exit(-1);
        }
    }

    answer = grid_get_value(&min_heat, end_x, end_y);
    free_grid(&min_heat);
    return answer;
}


int main(int argc, char ** argv)
{
    FILE * fp;
    struct grid map;

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
    fp = fopen(argv[1], "r");
    if(!fp) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    read_grid(fp, &map, NULL);

    printf("Answer: %d\n",
            traverse_city(&map, 0, 0, map.width - 1, map.height - 1, 3));

    free_grid(&map);
    fclose(fp);
    return 0;
}

