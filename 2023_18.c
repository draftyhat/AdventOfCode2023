#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "get_next_line.h"
#include "grid.h"
#include "location.h"


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 18
#define AOC_YEAR 2023


#define GRIDSIZE 1000
#define UNMARKED 2
#define INSIDE 1
#define OUTSIDE 0


void read_lagoon_map(
    struct grid * map,
    FILE * fp)
{
    int scanf_retval;
    char read_dir;
    int nsteps, color, dir;
    struct location loc;

    /* arbitrary */
    create_grid(map, GRIDSIZE, GRIDSIZE, UNMARKED);
    loc.x = 100;
    loc.y = 100;
    grid_set_value(map, loc.x, loc.y, 1);

    scanf_retval = fscanf(fp, "%c %d (#%x)\n",
        &read_dir, &nsteps, &color);
    while(scanf_retval == 3) {
        /* translate read direction */
        switch(read_dir) {
            case 'U': dir = NORTH;
                      break;
            case 'R': dir = EAST;
                      break;
            case 'D': dir = SOUTH;
                      break;
            case 'L': dir = WEST;
                      break;
            default:
                fprintf(stderr, "ERROR: unknown direction '%c'\n", read_dir);
                exit(-1);
        }

        /* would this take us outside the grid? */
        if(!go_direction_with_bounds(NULL, &loc, dir, nsteps,
                    map->width, map->height)) {
            fprintf(stderr,
                "ERROR: exceeded grid bounds, %d %d going %d steps at dir %d\n",
                loc.x, loc.y, nsteps, dir);
            exit(-1);
        }

        /* mark trench */
        for(int step=0; step < nsteps; step++) {
            go_direction(&loc, &loc, dir, 1);
            grid_set_value(map, loc.x, loc.y, INSIDE);
        }

        /* read next direction */
        scanf_retval = fscanf(fp, "%c %d (#%x)\n",
            &read_dir, &nsteps, &color);
    }
}

int fill_lagoon(struct grid * map) {
    struct location this_loc;
    struct location stack[GRIDSIZE * GRIDSIZE], * stackptr, * stack_end;
    int inside_count = 0;

    stackptr = stack;
    stack_end = &stack[sizeof(stack)/sizeof(stack[0]) - 4];

    /* Mark all squares connected to an outside square "0",
     * meaning "outside". Assume no islands. */
    /*  first mark outside squares */
    for(int x=0; x < map->width; x++) {
        grid_set_value(map, x, 0, OUTSIDE);
        stackptr->x = x;
        stackptr->y = 0;
        stackptr++;
        grid_set_value(map, x, map->height - 1, OUTSIDE);
        stackptr->x = x;
        stackptr->y = map->height - 1;
        stackptr++;
    }
    for(int y=0; y < map->height; y++) {
        grid_set_value(map, 0, y, OUTSIDE);
        stackptr->x = 0;
        stackptr->y = y;
        stackptr++;
        grid_set_value(map, map->width - 1, y, OUTSIDE);
        stackptr->x = map->width - 1;
        stackptr->y = y;
        stackptr++;
    }
    /*  mark connected squarse */
    while(stackptr > stack) {
        this_loc.x = stackptr->x;
        this_loc.y = stackptr->y;
        stackptr--;
        /* add all unvisited neighbors */
        for(int dir = 0; dir < 4; dir++) {
            stackptr++;
            if(go_direction_with_bounds(stackptr,
                        &this_loc, dir, 1,
                        map->width, map->height)
                    && grid_get_value(map,
                        stackptr->x, stackptr->y) == UNMARKED) {
                /* this is an OUTSIDE spot! */
                grid_set_value(map,
                        stackptr->x, stackptr->y, OUTSIDE);
                /* leave it on the stack for more neighbor visits */
            } else {
                /* either this location is out of bounds, or it's already
                 * marked. */
                stackptr--;
            }
        }

        if(stackptr > stack_end) {
            fprintf(stderr, "ERROR: blew out stack\n");
            exit(-1);
        }
    }
    
    /* count INSIDE or unfilled spaces */
    inside_count = 0;
    for(int x = 0; x < map->width; x++) {
        for(int y = 0; y < map->height; y++) {
            if(grid_get_value(map, x, y) != OUTSIDE)
                inside_count++;
        }
    }
    return inside_count;
};


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

    read_lagoon_map(&map, fp);
    printf("Answer: %d\n", fill_lagoon(&map));

    fclose(fp);
    return 0;
}

