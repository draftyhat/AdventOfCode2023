#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chargrid.h"
#include "linux/list.h"


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 11
#define AOC_YEAR 2023

#define EMPTY '.'
#define GALAXY '#'
#define DOUBLE_WIDE 'w'
#define DOUBLE_TALL 't'
#define DOUBLE 'd'

struct galaxy {
    struct list_head list;
    int x;
    int y;
};

/* returns # galaxies */
int convert_double_rows(struct list_head * galaxies, struct chargrid * g)
{
    int local_ngalaxies = 0;
    int is_empty;
    struct galaxy * new_galaxy;
    char ch;

    /* find empty rows and convert them to DOUBLE_TALL by marking
     * the first column DOUBLE_TALL */
    /* also create a list of galaxies */
    for(int y = 0; y < g->width; y++) {
        is_empty = 1;
        for(int x = 0; x < g->width; x++) {
            ch = chargrid_get_value(g, x, y);
            switch(ch) {
                case EMPTY: break;
                case GALAXY: is_empty = 0;
                             local_ngalaxies++;
                             new_galaxy = malloc(sizeof(*new_galaxy));
                             if(!new_galaxy) {
                                 fprintf(stderr, "malloc error\n");
                                 exit(-1);
                             }
                             new_galaxy->x = x;
                             new_galaxy->y = y;
                             list_add(&new_galaxy->list, galaxies);
                             break;
                default:
                    fprintf(stderr, "Unexpected char %c at %d,%d\n",
                            ch, x, y);
                    exit(-1);
                    break;
            };
        }
        if(is_empty) {
            chargrid_set_value(g, 0, y, DOUBLE_TALL);
        }
    }

    return local_ngalaxies;
}
void convert_double_columns(struct chargrid * g)
{
    char ch;
    int is_empty;

    /* find empty rows and convert them to DOUBLE_WIDE */
    for(int x = 0; x < g->width; x++) {
        is_empty = 1;
        for(int y = 0; y < g->width; y++) {
            ch = chargrid_get_value(g, x, y);
            switch(ch) {
                case EMPTY:
                case DOUBLE_TALL:
                    break;
                case GALAXY:
                    is_empty = 0;
                    break;
                default:
                    fprintf(stderr, "Unexpected char %c at %d,%d\n",
                            ch, x, y);
                    exit(-1);
                    break;
            };
        }
        if(is_empty) {
            ch = chargrid_get_value(g, x, 0);
            if(DOUBLE_TALL == ch)
                chargrid_set_value(g, x, 0, DOUBLE);
            else
                chargrid_set_value(g, x, 0, DOUBLE_WIDE);
        }
    }
}

long get_shortest_path_length(
        struct chargrid * g,
        struct galaxy * galaxy0,
        struct galaxy * galaxy1)
{
    long retval = 0;
    int step_x = 1, step_y = 1;
    char ch;

    /* if galaxy0 is below galaxy1, step y by -1 */
    if(galaxy0->y > galaxy1->y)
        step_y = -1;
    /* if galaxy0 is right of galaxy1, step y by -1 */
    if(galaxy0->x > galaxy1->x)
        step_x = -1;

    /* count # steps for each column */
    for(int x = galaxy0->x;
            x != galaxy1->x;
            x += step_x) {
        ch = chargrid_get_value(g, x, 0);
        retval++;
        if(ch == DOUBLE_WIDE || ch == DOUBLE)
            retval++;
    }

    /* count # steps for each row */
    for(int y = galaxy0->y;
            y != galaxy1->y;
            y += step_y) {
        ch = chargrid_get_value(g, 0, y);
        retval++;
        if(ch == DOUBLE_TALL || ch == DOUBLE)
            retval++;
    }

    return retval;
}

int main(int argc, char ** argv)
{
    FILE * fp;
    struct chargrid g;
    struct list_head galaxies = LIST_HEAD_INIT(galaxies);
    struct galaxy * galaxy_ptr, * galaxy_ptr_save, * galaxy_pair;
    long path_sum = 0;

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

    /* read grid */
    fp = fopen(argv[1], "rb");
    if(fp < 0) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    read_chargrid(fp, &g, NULL);
    fclose(fp);

    convert_double_rows(&galaxies, &g);
    convert_double_columns(&g);

    /* for each galaxy */
    list_for_each_entry_safe(galaxy_ptr, galaxy_ptr_save, &galaxies, list) {
        /* remove this galaxy from the list */
        list_del(&galaxy_ptr->list);

        /* pair it with every other galaxy in the list */
        list_for_each_entry(galaxy_pair, &galaxies, list) {
            path_sum += get_shortest_path_length(&g, galaxy_ptr, galaxy_pair);
        }

        free(galaxy_ptr);
    }

    free_chargrid(&g);
    list_for_each_entry_safe(galaxy_ptr, galaxy_ptr_save, &galaxies, list) {
        free(galaxy_ptr);
    }

    printf("Part 1 answer: %ld\n", path_sum);
    return 0;
}

