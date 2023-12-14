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

#define AOC_DAY 13
#define AOC_YEAR 2023


int reflects_horizontal(struct chargrid * g, int axis, int ndesired_smudges)
{
    int reflect_width;
    int nsmudges = 0;

    reflect_width = g->width - axis;
    if(axis < reflect_width) {
        reflect_width = axis;
    }
    DBGPRINT("Checking axis %d, width %d\n", axis, reflect_width);
    if(reflect_width < 1)
        return 0;

    for(int reflect=0; reflect < reflect_width; reflect++) {
        for(int y=0; y < g->height; y++) {
            if(chargrid_get_value(g, axis - reflect - 1, y)
                    != chargrid_get_value(g, axis + reflect, y)) {
                nsmudges++;
                if(nsmudges > ndesired_smudges) {
                    DBGPRINT("Found mismatch on axis %d at %d,%d/%d,%d : %c,%c\n",
                            axis, axis - reflect - 1, y, axis + reflect, y,
                            chargrid_get_value(g, axis - reflect - 1, y),
                            chargrid_get_value(g, axis + reflect, y));
                    return 0;
                }
            }
        }
    }
    if(nsmudges == ndesired_smudges)
        return 1;
    return 0;
}

int reflects_vertical(struct chargrid * g, int axis, int ndesired_smudges)
{
    int reflect_height;
    int nsmudges = 0;

    reflect_height = g->height - axis;
    if(axis < reflect_height) {
        reflect_height = axis;
    }
    DBGPRINT("Checking axis %d, height %d\n", axis, reflect_height);
    if(reflect_height < 1)
        return 0;

    for(int reflect=0; reflect < reflect_height; reflect++) {
        for(int x=0; x < g->width; x++) {
            if(chargrid_get_value(g, x, axis - reflect - 1)
                    != chargrid_get_value(g, x, axis + reflect)) {
                nsmudges++;
                if(nsmudges > ndesired_smudges) {
                    DBGPRINT("Found mismatch on axis %d at %d,%d/%d,%d : %c,%c\n",
                            axis, x, axis - reflect - 1, x, axis + reflect,
                            chargrid_get_value(g, x, axis - reflect - 1),
                            chargrid_get_value(g, x, axis + reflect));
                    return 0;
                }
            }
        }
    }
    if(nsmudges == ndesired_smudges)
        return 1;
    return 0;
}


int main(int argc, char ** argv)
{
    int fd;
    struct chargrid g;
    long answer = 0;
    int axis = 0;
    int smudges = 0;

#ifdef PART2
    smudges = 1;
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

    read_chargrid_by_line(fd, &g, 0);
    while(g.width > 0) {
        axis = 0;
        DBGPRINT("Read chargrid %d x %d\n", g.width, g.height);
        print_chargrid(&g);

        for(int i=0; i < g.width && !axis; i++) {
            if(reflects_horizontal(&g, i, smudges)) {
                answer += i;
                axis = i;
                printf("Found reflects_horizontal at %d!\n", i);
                break;
            }
        }
        for(int i=0; i < g.height && !axis; i++) {
            if(reflects_vertical(&g, i, smudges)) {
                answer += i * 100;
                axis = i;
                printf("Found reflects_vertical at %d!\n", i);
                break;
            }
        }
        read_chargrid_by_line(fd, &g, 0);
    }

    printf("answer%s: %ld\n", PART2_STRING, answer);

    close(fd);
    return 0;
}

