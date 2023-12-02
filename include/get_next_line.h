#ifndef GET_NEXT_LINE_H
#define GET_NEXT_LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>


/**
 ** @brief read next line from a file
 ** @param fd file to read from
 ** @param line buffer to read into
 ** @param line buffer to read into
 ** @return length of line read; -1 upon error, 0 if no more bytes
 **/
size_t get_next_line(
        int fd,
        char * line,
        const size_t linesize);

#endif  /* #ifndef GET_NEXT_LINE_H */
