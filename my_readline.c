#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int READLINE_READ_SIZE = 512;

char *readline_buffer = NULL;

void init_my_readline() {
    readline_buffer = malloc((READLINE_READ_SIZE + 1) * sizeof(char));
    if (readline_buffer == NULL) {
        perror("Failed to allocate memory for readline buffer");
        exit(EXIT_FAILURE);
    }
    readline_buffer[0] = '\0';
}

void append_to_line(char **line, const char *str) {
    size_t new_length = strlen(str);
    if (*line == NULL) {
        *line = strdup(str);
        if (*line == NULL) {
            perror("Failed to allocate memory for line");
            exit(EXIT_FAILURE);
        }
    } else {
        size_t current_length = strlen(*line);
        *line = realloc(*line, current_length + new_length + 1);
        if (*line == NULL) {
            perror("Failed to reallocate memory for line");
            exit(EXIT_FAILURE);
        }
        strcat(*line, str);
    }
}

void append_to_readline_buffer(const char *str) {
    size_t current_length = (readline_buffer != NULL) ? strlen(readline_buffer) : 0;
    size_t new_length = strlen(str);
    readline_buffer = realloc(readline_buffer, current_length + new_length + 1);
    if (readline_buffer == NULL) {
        perror("Failed to reallocate memory for readline_buffer");
        exit(EXIT_FAILURE);
    }

    if (strncat(readline_buffer, str, new_length) == NULL) {
        perror("Failed to concatenate to readline_buffer");
        exit(EXIT_FAILURE);
    }
}

int has_newline(char *buffer){
  for (int i = 0; buffer[i] != '\0'; i ++) {
    if (buffer[i] == '\n') {
      return i;
    }
  }
  return -1;
}

void copy_sides(char *right, char *left, char *temp_buffer, int newline_pos) {
  strncpy(left, temp_buffer, newline_pos);
  left[newline_pos] = '\0';

  strncpy(right, &temp_buffer[newline_pos + 1], READLINE_READ_SIZE - newline_pos);
  right[READLINE_READ_SIZE - newline_pos - 1] = '\0';
}

// void *check_for_newlines(char *temp_buffer, int newline_pos, char **line) {
//     if ((newline_pos = has_newline(temp_buffer)) != -1) {
//       if (newline_pos == 0) {
//         printf("newline at beginning %s\n", temp_buffer);
//         memmove(temp_buffer, temp_buffer+1, strlen(temp_buffer));
//         append_to_readline_buffer(temp_buffer);
//       } else if (newline_pos == READLINE_READ_SIZE) {
//         printf("newline at end %s\n", temp_buffer);
//       } else {
//         printf("in the middle: %s\n", temp_buffer);
//         char *right = malloc((READLINE_READ_SIZE - newline_pos) * sizeof(char));
//         char *left = malloc((newline_pos + 1) * sizeof(char));

//         if (left == NULL || right == NULL) {
//             perror("Failed to allocate memory for left/right");
//             exit(EXIT_FAILURE);
//         }
//         copy_sides(right, left, temp_buffer, newline_pos);

//         append_to_line(line, left);
//         append_to_readline_buffer(right);
//         free(left);
//         free(right);
//         free(temp_buffer); 
//       }
//     } else {
//       append_to_line(line, temp_buffer);
//     }
// }

char *my_readline(int fd) {
  char *line = NULL;
  char *temp_buffer = malloc((READLINE_READ_SIZE + 1) * sizeof(char));
  int newline_pos = -1;
  if (readline_buffer[0] != '\0') {
      if ((newline_pos = has_newline(readline_buffer)) != -1) {
        char *right = malloc((READLINE_READ_SIZE - newline_pos) * sizeof(char));
        char *left = malloc((newline_pos + 1) * sizeof(char));

        if (left == NULL || right == NULL) {
            perror("Failed to allocate memory for left/right");
            exit(EXIT_FAILURE);
        }      
        copy_sides(right, left, readline_buffer, newline_pos);
        if (newline_pos == 0) {
            // append_to_line(&line, readline_buffer);
            strncpy(right, &readline_buffer[newline_pos + 1], READLINE_READ_SIZE - newline_pos);
            right[READLINE_READ_SIZE - newline_pos - 1] = '\0';
            free(readline_buffer);
            init_my_readline();

            append_to_readline_buffer(right);
        } else if (newline_pos == READLINE_READ_SIZE) {
            printf("newline at end %s\n", readline_buffer);
        } else {
            printf("in the middle: %s\n", readline_buffer);
            append_to_line(&line, left);
            free(readline_buffer);
            init_my_readline();

            append_to_readline_buffer(right);

            free(temp_buffer);
            return line;
        }
        free(left);
        free(right);
      } else {
        append_to_line(&line, readline_buffer);
      }
    free(readline_buffer);
    free(temp_buffer);
    init_my_readline();
    printf("returning\n");
    return line;

  }
  ssize_t bytesRead;
  while ((bytesRead = read(fd, temp_buffer, READLINE_READ_SIZE)) > 0) {
    temp_buffer[bytesRead] = '\0';


    if ((newline_pos = has_newline(temp_buffer)) != -1) {
      if (newline_pos == 0) {
        printf("newline at beginning %s\n", temp_buffer);
        memmove(temp_buffer, temp_buffer+1, strlen(temp_buffer));
        printf("removed newling: %s\n", temp_buffer);
        append_to_readline_buffer(temp_buffer);
      } else if (newline_pos == READLINE_READ_SIZE) {
        printf("newline at end %s\n", temp_buffer);
      } else {
        // printf("in the middle: %s\n", temp_buffer);
        char *right = malloc((READLINE_READ_SIZE - newline_pos) * sizeof(char));
        char *left = malloc((newline_pos + 1) * sizeof(char));

        if (left == NULL || right == NULL) {
            perror("Failed to allocate memory for left/right");
            exit(EXIT_FAILURE);
        }
        copy_sides(right, left, temp_buffer, newline_pos);

        append_to_line(&line, left);
        append_to_readline_buffer(right);
        free(left);
        free(right);
        free(temp_buffer); 

        return line;
      }
    } else {
      append_to_line(&line, temp_buffer);
    }
  }
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