#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chargrid.h"
#include <linux/list.h>


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 3
#define AOC_YEAR 2023

int is_symbol(const char ch)
{
    return ((ch < '0' || ch > '9') && ch != '.');
}

struct part {
    /* location of '*' */
    int x;
    int y;
    long part_number;
    struct list_head list;
};

long get_new_gear_number(int x, int y,
        long part_number, struct list_head * saved_parts) {
    struct part * part_ptr;

    /* check saved parts for this location */
    list_for_each_entry(part_ptr, saved_parts, list) {
        if(part_ptr->x == x && part_ptr->y == y) {
            /* remove this part from the list and return gear number */
            long gear_number = part_ptr->part_number * part_number;
            list_del(&part_ptr->list);
            free(part_ptr);
            return gear_number;
        }
    }

    /* allocate new part and save it */
    part_ptr = (struct part *) malloc(sizeof(*part_ptr));
    if(!part_ptr) {
        fprintf(stderr, "ERROR: cannot allocate part\n");
        _exit(-1);
    }
    part_ptr->x = x;
    part_ptr->y = y;
    part_ptr->part_number = part_number;
    list_add(&part_ptr->list, saved_parts);

    return 0;
}

int main(int argc, char ** argv)
{
    FILE * fp;
    struct chargrid g;
    long part_number = 0;
    long part_number_sum = 0;
    int valid_part = 0;
    char ch, symbol;
    int x, y, newx, newy, symbolx, symboly;
    long gear_number_sum = 0;
    struct list_head saved_parts = LIST_HEAD_INIT(saved_parts);
    struct part * part_ptr, *part_save_ptr;


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
    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }
    read_chargrid(fp, &g, NULL);

    /* for each element in the grid */
    for(y = 0; y < g.height; y++) {
        for(x = 0; x < g.width; x++) {
            ch = chargrid_get_value(&g, x, y);
            if(ch >= '0' && ch <= '9') {
                part_number = part_number * 10 + (long) (ch - '0');
                /* check neighbors to see if this is a valid part number */
                for(newx = x - 1; !valid_part && newx < x + 2; newx++) {
                    if(newx < 0 || newx >= g.width)
                        continue;
                    for(newy = y - 1; !valid_part && newy < y + 2; newy++) {
                        if(newy < 0 || newy >= g.height ||
                                (newx == x && newy == y))
                            continue;
                        ch = chargrid_get_value(&g, newx, newy);
                        if(is_symbol(ch)) {
                            valid_part = 1;
                            if(ch == '*') {
                                symbol = '*';
                                symbolx = newx;
                                symboly = newy;
                            }
                        }
                    }
                }
            } else { /* not in a part number */
                if(valid_part) {
                    part_number_sum += part_number;
                    DBGPRINT("----- adding part number %ld\n", part_number);
                    if(symbol == '*') {
                        gear_number_sum += get_new_gear_number(
                                symbolx, symboly, part_number, &saved_parts);
                    }
                }
                part_number = 0;
                valid_part = 0;
                symbol = '.';
            }
        }
    }

    fclose(fp);
    list_for_each_entry_safe(part_ptr, part_save_ptr, &saved_parts, list) {
        list_del(&part_ptr->list);
        free(part_ptr);
    }

    printf("Answer: %ld\n", part_number_sum);
    printf("Part 2 answer: %ld\n", gear_number_sum);
    return 0;
}

