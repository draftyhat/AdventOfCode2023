#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "get_next_line.h"
#include "linux/list.h"

/* 226816 */


#ifdef DEBUG
#define DBGPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define AOC_DAY 15
#define AOC_YEAR 2023

long hash(char * string, ssize_t length)
{
    long retval = 0;

    for(ssize_t i=0; i < length; i++) {
        retval = ((retval + ((long) string[i])) * 17) & 0xff;
    }

    DBGPRINT("Hash of \"%s\", length %ld: %ld\n", string, length, retval & 0xff);
    return (retval & 0xff);
}

struct lens {
    struct list_head list;
    char label[10];
    int focal_length;
};

void add_lens(struct list_head * boxes, int nboxes, char * label, ssize_t label_size)
{
    ssize_t operator_index;
    long hash_value;
    int found = 0;
    struct lens * lensptr;

    /* find '=' or '-' */
    for(operator_index = 0;
        operator_index < label_size && label[operator_index] != '=' && label[operator_index] != '-';
        operator_index++);

    hash_value = hash(label, operator_index) % nboxes;

    /* find matching lens in this box, if there is one */
    list_for_each_entry(lensptr, &boxes[hash_value], list) {
        if(strncmp(lensptr->label, label, operator_index - 1) == 0
                && strlen(lensptr->label) == operator_index) {
            /* found matching lens */
            if(label[operator_index] == '=') {
                /* replace the lens */
                lensptr->focal_length = atoi(&label[operator_index + 1]);
                DBGPRINT("Replaced lens %s in box %ld\n", label, hash_value);
            }
            else {
                /* remove this lens */
                list_del(&lensptr->list);
                free(lensptr);
                DBGPRINT("Removing lens %s from box %ld\n", label, hash_value);
            }

            found = 1;
            break;
        }
    }

    if(!found && label[operator_index] == '=') {
        /* add this lens to the box */
        lensptr = (struct lens *) malloc(sizeof(*lensptr));
        if(!lensptr) {
            fprintf(stderr, "malloc error\n");
            exit(1);
        }
        memcpy(lensptr->label, label, operator_index);
        lensptr->label[operator_index] = '\0';
        lensptr->focal_length = atoi(&label[operator_index + 1]);
        list_add_tail(&lensptr->list, &boxes[hash_value]);
        DBGPRINT("Added lens %s to box %ld\n", label, hash_value);
    }
}

long calculate_focusing_power(struct list_head * boxes, const int nboxes)
{
    long retval = 0;
    struct lens * lensptr;
    long slot;

    for(int box_number = 0 ; box_number < nboxes; box_number++) {
        slot = 1;
        list_for_each_entry(lensptr, &boxes[box_number], list) {
            DBGPRINT("%d] %ld) %s=%d\n",
                    box_number, slot, lensptr->label, lensptr->focal_length);
            retval += (box_number + 1) * slot * lensptr->focal_length;
            slot++;
        }
    }

    return retval;
}


int main(int argc, char ** argv)
{
    char line[1000 + 1];
    ssize_t line_size = 0;
    int fd;
    long sum = 0;
#define NBOXES 256
    struct list_head boxes[NBOXES];
    struct lens * lensptr, * lensptr_save;

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

    /* init box lists */
    for(int i = 0; i < NBOXES; i++) {
        INIT_LIST_HEAD(&boxes[i]);
    }

    /* read each line */
    fd = open(argv[1], O_RDONLY, 0);
    if(fd < 0) {
        DBGPRINT("Error opening file \"%s\": %s\n",
                argv[1], strerror(errno));
        return -1;
    }

    line_size = get_next_delimited(fd, line, sizeof(line), ',');
    while(line_size > 0) {
        sum += hash(line, line_size - 1);
        add_lens(boxes, NBOXES, line, line_size - 1);
        line_size = get_next_delimited(fd, line, sizeof(line), ',');
    }

    close(fd);
    printf("Part 1 answer: %ld\n", sum);
    printf("Part 2 answer: %ld\n", calculate_focusing_power(boxes, NBOXES));


    for(int i = 0; i < NBOXES; i++) {
        list_for_each_entry_safe(lensptr, lensptr_save, &boxes[i], list) {
            free(lensptr);
        }
    }
    return 0;
}

