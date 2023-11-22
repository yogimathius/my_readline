#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int READLINE_READ_SIZE = 512;

char *readline_buffer = NULL;

void init_my_readline() {
    readline_buffer = malloc((READLINE_READ_SIZE + 2) * sizeof(char));
    if (readline_buffer == NULL) {
        perror("Failed to allocate memory for readline buffer");
        exit(EXIT_FAILURE);
    }
    readline_buffer[0] = 0; // buffer_start
    readline_buffer[1] = 0; // buffer_end
}

char *my_readline(int fd) {
    int buffer_start = readline_buffer[0];
    int buffer_end = readline_buffer[1];

    if (buffer_start == buffer_end) {
        buffer_end = read(fd, readline_buffer + 2, READLINE_READ_SIZE) + 2;
        buffer_start = 2;

        if (buffer_end <= 2) {
            free(readline_buffer);
            readline_buffer = NULL;
            return NULL;
        }

        readline_buffer[0] = buffer_start;
        readline_buffer[1] = buffer_end;
    }

    int newline_pos = -1;
    for (int i = buffer_start; i < buffer_end; i++) {
        if (readline_buffer[i] == '\n') {
            newline_pos = i;
            break;
        }
    }

    int line_length = (newline_pos != -1) ? newline_pos - buffer_start : buffer_end - buffer_start;
    char *line = malloc((line_length + 1) * sizeof(char));
    if (line == NULL) {
        return NULL;
    }

    memcpy(line, &readline_buffer[buffer_start], line_length);
    line[line_length] = '\0';

    buffer_start = (newline_pos != -1) ? newline_pos + 1 : buffer_end;

    readline_buffer[0] = buffer_start;
    readline_buffer[1] = buffer_end;

    return line;
}

int main(int ac, char **av) {
    char *str = NULL;
    init_my_readline();
    int fd = open(av[1], O_RDONLY);
    while ((str = my_readline(fd)) != NULL) {
        printf("%s\n", str);
        free(str);
    }
    close(fd);
    return 0;
}