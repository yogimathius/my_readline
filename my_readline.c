#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int READLINE_READ_SIZE = 512;
char *READLINE_BUFFER = NULL;

void check_null(char *line) {
    if (line == NULL) {
        perror("Failed to allocate memory for readline line or buffer");
        exit(EXIT_FAILURE);
    }
}

void init_my_readline() {
    if (READLINE_BUFFER != NULL) free(READLINE_BUFFER);
    READLINE_BUFFER = malloc((READLINE_READ_SIZE + 1));
    check_null(READLINE_BUFFER);
    READLINE_BUFFER[0] = '\0';
}

void append_to_line(char **line, const char *str) {
    if (*line == NULL) {
        *line = strdup(str);
        check_null(*line);
    } else {
        *line = realloc(*line, strlen(*line) + strlen(str) + 1);
        check_null(*line);
        strcat(*line, str);
    }
}

void update_readline_buffer(const char *str) {
    init_my_readline();
    size_t current_length = (READLINE_BUFFER != NULL) ? strlen(READLINE_BUFFER) : 0;
    READLINE_BUFFER = realloc(READLINE_BUFFER, current_length +  strlen(str) + 1);
    check_null(READLINE_BUFFER);
    check_null(strncat(READLINE_BUFFER, str,  strlen(str)));
}

int has_newline(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '\n') return i;
    }
    return -1;
}

char *copy_side(int size, char *buffer, int copy_len) {
    char *side = malloc((size));
    check_null(side);
    strncpy(side, buffer, copy_len);
    side[copy_len] = '\0';
    return side;
}

void split_line(char *temp_buffer, int newline_pos, char **line) {
    int total_size = READLINE_READ_SIZE - newline_pos;
    char *right = copy_side(total_size, &temp_buffer[newline_pos + 1], total_size);
    char *left = copy_side(newline_pos + 1, temp_buffer, newline_pos);
    
    append_to_line(line, left);
    update_readline_buffer(right);

    free(left), free(right);
}

int check_line(int newline_pos, char *buffer, char **line) {
    if ((newline_pos = has_newline(buffer)) > 0) {
        split_line(buffer, newline_pos, line);
        return 1;
    } else {
        append_to_line(line, buffer);
        return 0;
    }
}

char *my_readline(int fd) {
    char *line = NULL;
    char *temp_buffer = malloc((READLINE_READ_SIZE + 1));
    int newline_pos = -1;

    if (READLINE_BUFFER[0] != '\0') {
        if (check_line(newline_pos, READLINE_BUFFER, &line) == 1) {
            free(temp_buffer);
            return line;
        }
        init_my_readline();
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fd, temp_buffer, READLINE_READ_SIZE)) > 0) {
        temp_buffer[bytesRead] = '\0';
        if (check_line(newline_pos, temp_buffer, &line) == 1) {
            free(temp_buffer);
            return line;
        }
    }

    int len = line != NULL ? strlen(line) : 0;
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
     
    free(temp_buffer);
    return line;
}

int main(int ac, char **av) {
    if (ac == 3) {
        int fd = open(av[1], O_RDONLY);

        READLINE_READ_SIZE = atoi(av[2]);

        char *line = NULL;
        init_my_readline();

        while ((line = my_readline(fd))) {
            printf("from main: %s\n", line);
            free(line);
        }
        close(fd);
    }
}