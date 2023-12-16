#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>


/* returns a new field
 * -1 upon error, 0 if no more bytes
 */
ssize_t get_next_delimited(
        int fd,
        char * line,
        const ssize_t linesize,
        const char delimiter)
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

    /* find first delimiter */
    for(ssize_t i=0; i < read_bytes; i++) {
        if(line[i] == delimiter) {
            line[i] = '\0';
            /* seek file back to start of next line */
            lseek(fd, i - read_bytes + 1, SEEK_CUR);
            return i + 1;
        }
    }

    return -1;
}

ssize_t get_next_line(int fd, char * line, const size_t linesize)
{
    return get_next_delimited(fd, line, linesize, '\n');
}
