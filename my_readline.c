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
    int null_line = *line == NULL;
    *line = null_line ? 
        strdup(str) : realloc(*line, strlen(*line) + strlen(str) + 1);
    check_null(*line);
    if (!null_line) strcat(*line, str);
}

void update_readline_buffer(const char *str) {
    init_my_readline();
    size_t new_length = strlen(READLINE_BUFFER) + strlen(str) + 1;
    READLINE_BUFFER = realloc(READLINE_BUFFER, new_length);
    check_null(READLINE_BUFFER);
    strcat(READLINE_BUFFER, str);
}

int has_newline(char *buffer) {
    char *newline_pos = strchr(buffer, '\n');
    return newline_pos ? newline_pos - buffer : -1;
}

void split_line_and_update_buffer(char **line, char *temp_buffer, int newline_pos) {
    char *right = malloc((READLINE_READ_SIZE - newline_pos));	
    char *left = malloc((newline_pos + 1));	
    check_null(left);	
    check_null(right);	

    strncpy(left, temp_buffer, newline_pos);	
    left[newline_pos] = '\0';	

    strncpy(right, &temp_buffer[newline_pos + 1], READLINE_READ_SIZE - newline_pos);	
    right[READLINE_READ_SIZE - newline_pos - 1] = '\0';	


    append_to_line(line, left);
    init_my_readline();
    update_readline_buffer(right);

    free(left), free(right);
}

int check_and_process_line(char *buffer, char **line) {
    int newline_pos;
    if ((newline_pos = has_newline(buffer)) > 0) {
        split_line_and_update_buffer(line, buffer, newline_pos); return 1;
    } else {
        append_to_line(line, buffer); return 0;
    }
}

char *my_readline(int fd) {
    char *line = NULL;
    char *temp_buffer = malloc((READLINE_READ_SIZE + 1));

    if (READLINE_BUFFER[0] != '\0') {
        if (check_and_process_line(READLINE_BUFFER, &line) == 1) {
            free(temp_buffer); return line;
        }
        init_my_readline();
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fd, temp_buffer, READLINE_READ_SIZE)) > 0) {
        temp_buffer[bytesRead] = '\0';
        if (check_and_process_line(temp_buffer, &line) == 1) {
            free(temp_buffer); return line;
        }
    }

    int len = line != NULL ? strlen(line) : 0;
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
     
    free(temp_buffer); return line;
}