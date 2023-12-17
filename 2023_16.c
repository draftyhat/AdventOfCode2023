#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chargrid.h"
#include "grid.h"
#include "location.h"


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 16
#define AOC_YEAR 2023

int count_energized(
        struct chargrid * map,
        int x,
        int y, 
        int dir)
{
    struct grid visited, visited_count;
    struct vector pos_stack[100];
    int pos_stack_size = 0;
    int visited_bitvector;
    int answer;

    /* start at top left moving right */
    pos_stack[0].loc.x = x;
    pos_stack[0].loc.y = y;
    pos_stack[0].dir = dir;
    pos_stack_size = 1;

    /* mark this spot visited */
    create_grid(&visited, map->width, map->height, 0);
    create_grid(&visited_count, map->width, map->height, 0);
    grid_set_value(&visited_count, x, y, 1);
    grid_set_value(&visited, x, y, 1 << dir);

    while(pos_stack_size > 0) {
        /* change direction as indicated */
        switch(chargrid_get_value(map,
                    pos_stack[pos_stack_size - 1].loc.x,
                    pos_stack[pos_stack_size - 1].loc.y)) {
            case '/':
                switch(pos_stack[pos_stack_size - 1].dir) {
                    case NORTH: pos_stack[pos_stack_size - 1].dir = EAST;
                                break;
                    case EAST:  pos_stack[pos_stack_size - 1].dir = NORTH;
                                break;
                    case SOUTH: pos_stack[pos_stack_size - 1].dir = WEST;
                                break;
                    case WEST:  pos_stack[pos_stack_size - 1].dir = SOUTH;
                                break;
                }
                break;
            case '\\':
                switch(pos_stack[pos_stack_size - 1].dir) {
                    case NORTH: pos_stack[pos_stack_size - 1].dir = WEST;
                                break;
                    case EAST:  pos_stack[pos_stack_size - 1].dir = SOUTH;
                                break;
                    case SOUTH: pos_stack[pos_stack_size - 1].dir = EAST;
                                break;
                    case WEST:  pos_stack[pos_stack_size - 1].dir = NORTH;
                                break;
                }
                break;
            case '-':
                switch(pos_stack[pos_stack_size - 1].dir) {
                    case NORTH:
                    case SOUTH:
                        /* split! */
                        pos_stack[pos_stack_size - 1].dir = EAST;
                        memcpy(&pos_stack[pos_stack_size].loc,
                               &pos_stack[pos_stack_size - 1].loc,
                               sizeof(pos_stack[pos_stack_size].loc));
                        pos_stack[pos_stack_size].dir = WEST;
                        pos_stack_size++;
                        break;
                    case EAST:
                    case WEST:
                        break;
                }
                break;
            case '|':
                switch(pos_stack[pos_stack_size - 1].dir) {
                    case EAST:
                    case WEST:
                        /* split! */
                        pos_stack[pos_stack_size - 1].dir = NORTH;
                        memcpy(&pos_stack[pos_stack_size].loc,
                               &pos_stack[pos_stack_size - 1].loc,
                               sizeof(pos_stack[pos_stack_size].loc));
                        pos_stack[pos_stack_size].dir = SOUTH;
                        pos_stack_size++;
                        break;
                    case NORTH:
                    case SOUTH:
                        break;
                }
                break;
            default:
                break;
        }
        DBGPRINT("%d] %d,%d going %d\n", pos_stack_size,
                pos_stack[pos_stack_size - 1].loc.x,
                pos_stack[pos_stack_size - 1].loc.y,
                pos_stack[pos_stack_size - 1].dir);
        /* move top pos one step */
        if(go_direction_with_bounds(
                    &pos_stack[pos_stack_size - 1].loc,
                    &pos_stack[pos_stack_size - 1].loc,
                    pos_stack[pos_stack_size - 1].dir,
                    1, map->width, map->height)) {
            /* mark new loc visited */
            grid_set_value(&visited_count, pos_stack[pos_stack_size - 1].loc.x,
                    pos_stack[pos_stack_size - 1].loc.y, 1);
            /* check for already visited going this direction */
            visited_bitvector = grid_get_value(&visited, pos_stack[pos_stack_size - 1].loc.x,
                pos_stack[pos_stack_size - 1].loc.y);
            if(visited_bitvector & (1 << pos_stack[pos_stack_size - 1].dir)) {
                /* been here, done that */
                pos_stack_size--;
            } else {
                /* set visited */
                grid_set_value(&visited, pos_stack[pos_stack_size - 1].loc.x,
                        pos_stack[pos_stack_size - 1].loc.y,
                        visited_bitvector | (1 << pos_stack[pos_stack_size - 1].dir));
            }
        } else {
            /* could not take this step. Pop the pos. */
            pos_stack_size--;
        }
        if(pos_stack_size == sizeof(pos_stack)/sizeof(pos_stack[0]) - 1) {
            fprintf(stderr, "ERROR: blew out pos_stack\n");
            exit(1);
        }
    }

    answer = sum_grid(&visited_count);
    free_grid(&visited);
    free_grid(&visited_count);

    DBGPRINT("Start %d,%d going %d: %d\n", x, y, dir, answer);
    return answer;
}


int main(int argc, char ** argv)
{
    int fd;
    struct chargrid map;
    int visited_count, max_visited_count = 0;

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

    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    read_chargrid_by_line(fd, &map, NULL);
    close(fd);

    printf("Part 1 answer: %d\n", count_energized(&map, 0, 0, EAST));
    /* try top and bottom starting positions */
    for(int x=0; x < map.width; x++) {
        visited_count = count_energized(&map, x, 0, SOUTH);
        if(visited_count > max_visited_count)
            max_visited_count = visited_count;

        visited_count = count_energized(&map, x, map.height - 1, NORTH);
        if(visited_count > max_visited_count)
            max_visited_count = visited_count;
    }

    /* try left and right starting positions */
    for(int y=0; y < map.height; y++) {
        visited_count = count_energized(&map, 0, y, EAST);
        if(visited_count > max_visited_count)
            max_visited_count = visited_count;

        visited_count = count_energized(&map, map.width - 1, y, WEST);
        if(visited_count > max_visited_count)
            max_visited_count = visited_count;
    }

    printf("\n");
    count_energized(&map, 3, 0, SOUTH);
    printf("\n");

    printf("Part 2 answer: %d\n", max_visited_count);
    free_chargrid(&map);

    return 0;
}

