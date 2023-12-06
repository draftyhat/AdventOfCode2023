#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>


/* returns a new line
 * -1 upon error, 0 if no more bytes
 */
ssize_t get_next_line(int fd, char * line, const size_t linesize)
{
    ssize_t read_bytes;

    /* read all data into line */
    read_bytes = read(fd, line, linesize);
    if(read_bytes < 0) {
        perror("Could not read file");
        return -1;
    } else if(read_bytes == 0) {
        return 0;
    }

    /* find first newline */
    for(ssize_t i=0; i < read_bytes; i++) {
        if(line[i] == '\n') {
            line[i] = '\0';
            /* seek file back to start of next line */
            lseek(fd, i - read_bytes + 1, SEEK_CUR);
            return i + 1;
        }
    }

    return -1;
}

