#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chargrid.h"


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 10
#define AOC_YEAR 2023


int get_start_spots(
        struct chargrid * g,
        int * x0, int * y0,
        int * x1, int * y1,
        int start_x, int start_y) {
    int next_x, next_y, got_first = 0;
    char ch, first_ch = 'v';

    chargrid_set_value(g, start_x, start_y, 'v');
    if(chargrid_get_up(&ch, &next_x, &next_y, g, start_x, start_y)
            && (ch == '7' || ch == '|' || ch == 'F')) {
        *x0 = next_x;
        *y0 = next_y;
        got_first = 1;
    }

    if(chargrid_get_right(&ch, &next_x, &next_y, g, start_x, start_y)
            && (ch == 'J' || ch == '-' || ch == '7')) {
        if(got_first) {
            *x1 = next_x;
            *y1 = next_y;
            /* this pipe exits only to the right */
            chargrid_set_value(g, start_x, start_y, 'r');
            return 0;
        } else {
            *x0 = next_x;
            *y0 = next_y;
            got_first = 1;
            first_ch = 'r';
        }
    }

    if(chargrid_get_down(&ch, &next_x, &next_y, g, start_x, start_y)
            && (ch == 'J' || ch == '|' || ch == 'L')) {
        if(got_first) {
            *x1 = next_x;
            *y1 = next_y;
            if(first_ch == 'r') {
                /* pipe exits to the right */
                chargrid_set_value(g, start_x, start_y, 'r');
            } else {
                /* pipe is vertical */
                chargrid_set_value(g, start_x, start_y, 'V');
            }
            return 0;
        } else {
            *x0 = next_x;
            *y0 = next_y;
            got_first = 1;
        }
    }

    if(chargrid_get_left(&ch, &next_x, &next_y, g, start_x, start_y)
            && (ch == 'F' || ch == '-' || ch == 'L')) {
        if(got_first) {
            *x1 = next_x;
            *y1 = next_y;
            if(first_ch != 'r')
                /* this pipe exits only to the left */
                chargrid_set_value(g, start_x, start_y, 'l');
            /* else, this pipe crosses this column; already marked 'v' */
            return 0;
        } else {
            fprintf(stderr, "ERROR: could not find 2 starting positions.\n");
            exit(-1);
        }
    }

    return -1;
}

int get_next_spot(
        struct chargrid * g,
        int * x, int * y,
        int * last_x, int * last_y)
{
    int next_x0, next_y0, next_x1, next_y1;
    char ch, next_ch;
    int retval = 0;

    ch = chargrid_get_value(g, *x, *y);
    switch(ch) {
        case '|': chargrid_get_up(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_down(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        case '-': chargrid_get_left(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_right(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        case 'L': chargrid_get_up(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_right(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        case 'J': chargrid_get_left(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_up(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        case '7': chargrid_get_left(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_down(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        case 'F': chargrid_get_right(&next_ch, &next_x0, &next_y0, g, *x, *y);
                  chargrid_get_down(&next_ch, &next_x1, &next_y1, g, *x, *y);
                  break;
        default:  fprintf(stderr, "ERROR: started on spot %c\n", ch);
                  retval = -1;
                  exit(-1);
                  break;
    }
    if(next_x0 == *last_x && next_y0 == *last_y) {
        *last_x = *x;
        *last_y = *y;
        *x = next_x1;
        *y = next_y1;
    } else {
        *last_x = *x;
        *last_y = *y;
        *x = next_x0;
        *y = next_y0;
    }
    DBGPRINT("   returning next %d,%d\n", *x, *y);

    return retval;
}

void set_inside_value(
        struct chargrid * g,
        int x,
        int y)
{
    char ch;
    ch = chargrid_get_value(g, x, y);
    switch(ch) {
            case '|': /* pipe doesn't cross, don't care */
                      chargrid_set_value(g, x, y, 'V');
                      break;
            case 'J':
            case '7': /* pipe exits to the left */
                      chargrid_set_value(g, x, y, 'l');
                      break;
            case 'L':
            case 'F': /* pipe exits to the right */
                      chargrid_set_value(g, x, y, 'r');
                      break;
            case '-': chargrid_set_value(g, x, y, 'v');
                      break;
            default:  fprintf(stderr,
                              "ERROR: found unexpected visited-pipe char '%c'\n",
                              ch);
                      break;
    };
}

/* this fails the harder testcase */
int get_ninside(struct chargrid * g)
{
    int currently_inside;
    int ninside = 0;
    char ch;
    char last_ch;

    for(int x = 0; x < g->width; x++) {
        currently_inside = 0;
        last_ch = 'v';
        for(int y = 0; y < g->height; y++) {
            ch = chargrid_get_value(g, x, y);
            switch(ch) {
                case 'v': currently_inside ^= 1;
                          break;
                case 'l': if(last_ch == 'r') {
                              /* pipe crossed this column */
                              currently_inside ^= 1;
                              last_ch = 'v';
                          } else if(last_ch == 'l') {
                              /* pipe stayed to the left, never crossed */
                              last_ch = 'v';
                          } else {
                              last_ch = 'l';
                          }
                          break;
                case 'r': if(last_ch == 'l') {
                              /* pipe crossed this column */
                              currently_inside ^= 1;
                              last_ch = 'v';
                          } else if(last_ch == 'r') {
                              /* pipe stayed to the right, never crossed */
                              last_ch = 'v';
                          } else {
                              last_ch = 'r';
                          }
                          break;
                case 'V': break;
                default:  ninside += currently_inside;
                          if(currently_inside) {
                              DBGPRINT("Setting inside on '%c'\n", ch);
                              chargrid_set_value(g, x, y, 'O');
                          }
                          break;
            }
        }
    }

    return ninside;
}

int main(int argc, char ** argv)
{
    FILE * fp;
    struct chargrid g;
    int x, y, x0, y0, last_x, last_y, last_x0, last_y0;
    int nsteps = 0;

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

    /* read chargrid */
    fp = fopen(argv[1], "rb");
    if(!fp) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }
    read_chargrid(fp, &g, NULL);
    fclose(fp);

    /* find start position */
    for(x = 0; x < g.width; x++) {
        for(y = 0; y < g.height; y++) {
            if(chargrid_get_value(&g, x, y) == 'S')
                goto found;
        }
    }
found:

    if(x >= g.width || y >= g.height) {
        fprintf(stderr, "ERROR: could not find grid start position\n");
        print_chargrid(&g);
        exit(-1);
    }

    x0 = last_x = last_x0 = x;
    y0 = last_y = last_y0 = y;
    DBGPRINT("Found starting position %d,%d\n", x, y);

    /* get first two non-starting positions */
    get_start_spots(&g, &x, &y, &x0, &y0, x, y);

    /* try all neighbors; find two next steps in loop */
    nsteps = 1;
    do {
        get_next_spot(&g, &x, &y, &last_x, &last_y);
        get_next_spot(&g, &x0, &y0, &last_x0, &last_y0);

        /* set values for 'inside' count */
        set_inside_value(&g, last_x, last_y);
        set_inside_value(&g, last_x0, last_y0);
        nsteps++;
        DBGPRINT("[%d]: %d,%d  %d,%d\n", nsteps, x, y, x0, y0);
    } while(!((x == x0 && y == y0)
              || (x == last_x0 && y == last_y0)
              || (x0 == last_x && y0 == last_y)));
    set_inside_value(&g, x, y);

    printf("Part 1 answer: %d\n", nsteps);
    printf("Part 2 answer: %d\n", get_ninside(&g));
    //print_chargrid(&g);

    free_chargrid(&g);
    return 0;
}

