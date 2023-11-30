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
    // printf("=============\n");
    int buffer_start = readline_buffer[0];
    int buffer_end = readline_buffer[1];
    // printf("buffer_start: %d\n", buffer_start);
    // printf("buffer_end: %d\n", buffer_end);
    if (buffer_start == buffer_end) {
        buffer_end = read(fd, readline_buffer + 2, READLINE_READ_SIZE) + 2;
        buffer_start = 2;
        // printf("buffer_end: %d\n", buffer_end);

        if (buffer_end <= 2) {
            // printf("checks out\n");
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
            // printf("setting new line: %d\n", i);
            newline_pos = i;
            break;
        }
    }
    int line_length = (newline_pos != -1) ? newline_pos - buffer_start : buffer_end - buffer_start;
    // printf("line_length: %d\n", line_length);
    // printf("newline_pos: %d\n", newline_pos);
    char *line = malloc((line_length + 1) * sizeof(char));
    if (line == NULL) {
        return NULL;
    }

    // printf("buffer_start: %d, buffer_end: %d\n", buffer_end, buffer_start);
    memcpy(line, &readline_buffer[buffer_start], line_length);
    // if (newline_pos != -1) {
        line[line_length] = '\0';
    // }
    // printf("readline_buffer: %s\n", readline_buffer);

    buffer_start = (newline_pos != -1) ? newline_pos + 1 : buffer_end;
    // printf("buffer_start updated: %d, buffer_end: %d\n", buffer_end, buffer_start);

    readline_buffer[0] = buffer_start;
    readline_buffer[1] = buffer_end;

    return line;
}


  
// int main(int ac, char **av) {

//   if (ac == 3) {
//     int fd = open(av[1], O_RDONLY);

//     READLINE_READ_SIZE = atoi(av[2]);

//     char *line = NULL;
//     init_my_readline();

//     while ((line = my_readline(fd))) {
//       printf("%s\n", line);
//       free(line);
//     }
//     close(fd);
//   }
// }