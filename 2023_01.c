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

/* returns next sum */
int find_digit(const char * line, const size_t line_size)
{
    int retval = 0;
    size_t first_found_index, second_found_index;

    /* work from start of line to find first digit */
    for(first_found_index = 0;
        first_found_index < line_size &&
            (line[first_found_index] < '0' ||
             line[first_found_index] > '9');
        first_found_index++) {
    }
    if(first_found_index < line_size) {
        retval += 10 * (line[first_found_index] - '0');
    } else {
        DBGPRINT("ERROR: could not find first digit in line \"%s\"\n",
                line);
        return -1;
    }

    /* work from end of line to find second digit */
    for(second_found_index = line_size - 1;
        second_found_index >= first_found_index &&
            (line[second_found_index] < '0' ||
             line[second_found_index] > '9');
        second_found_index--) {
    }
    if(second_found_index >= first_found_index) {
        retval += line[second_found_index] - '0';
    } else {
        DBGPRINT("ERROR: could not find second digit in line \"%s\"\n",
                line);
        return -1;
    }

    DBGPRINT("line %s Returning value %d\n", line, retval);
    return retval;
}

void convert_spelled_digits(char * line, const size_t line_size)
{
    struct {
        char * name;
        int value;
    } spelled_digits[] = {
        { "one", 1 },
        { "two", 2 },
        { "three", 3 },
        { "four", 4 },
        { "five", 5 },
        { "six", 6 },
        { "seven", 7 },
        { "eight", 8 },
        { "nine", 9 },
    };

    for(int i = 0; i < line_size; i++) {
        for(int j = 0; j < sizeof(spelled_digits)/sizeof(spelled_digits[0]);
                j++) {
            if(strncmp(&line[i], spelled_digits[j].name,
                        strlen(spelled_digits[j].name)) == 0) {
                line[i] = '0' + spelled_digits[j].value;
                break;
            }
        }
    }
    DBGPRINT("Converted line: \"%s\"\n", line);
}

int main(int argc, char ** argv)
{
    char line[1000 + 1];
    size_t line_size = 0;
    int fd;
    int sum = 0, next_value;

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

    line_size = get_next_line(fd, line, sizeof(line));
    while(line_size > 0) {
#ifdef PART2
        convert_spelled_digits(line, line_size);
#endif
        next_value = find_digit(line, line_size);
        if(next_value < 0) {
            break;
        }
        sum += next_value;

        line_size = get_next_line(fd, line, sizeof(line));
    }

    close(fd);
    printf("Answer: %d\n", sum);
    return 0;
}

