#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int READLINE_READ_SIZE = 512;

char *readline_buffer = NULL;

void init_my_readline()
{
    if (readline_buffer != NULL)
    {
        free(readline_buffer);
    }
    readline_buffer = malloc((READLINE_READ_SIZE + 1) * sizeof(char));
    if (readline_buffer == NULL)
    {
        perror("Failed to allocate memory for readline buffer");
        exit(EXIT_FAILURE);
    }
    readline_buffer[0] = '\0';
}

void append_to_line(char **line, const char *str)
{
    size_t new_length = strlen(str);
    if (*line == NULL)
    {
        *line = strdup(str);
        if (*line == NULL)
        {
            perror("Failed to allocate memory for line");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        size_t current_length = strlen(*line);
        *line = realloc(*line, current_length + new_length + 1);
        if (*line == NULL)
        {
            perror("Failed to reallocate memory for line");
            exit(EXIT_FAILURE);
        }
        strcat(*line, str);
    }
}

void update_readline_buffer(const char *str)
{
    size_t current_length =
        (readline_buffer != NULL) ? strlen(readline_buffer) : 0;
    size_t new_length = strlen(str);
    readline_buffer = realloc(readline_buffer, current_length + new_length + 1);
    if (readline_buffer == NULL)
    {
        perror("Failed to reallocate memory for readline_buffer");
        exit(EXIT_FAILURE);
    }

    if (strncat(readline_buffer, str, new_length) == NULL)
    {
        perror("Failed to concatenate to readline_buffer");
        exit(EXIT_FAILURE);
    }
}

int has_newline(char *buffer)
{
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '\n')
        {
            return i;
        }
    }
    return -1;
}

void split_line(char *temp_buffer, int newline_pos, char **line)
{
    char *right = malloc((READLINE_READ_SIZE - newline_pos) * sizeof(char));
    char *left = malloc((newline_pos + 1) * sizeof(char));

    if (left == NULL || right == NULL)
    {
        perror("Failed to allocate memory for left/right");
        exit(EXIT_FAILURE);
    }

    strncpy(left, temp_buffer, newline_pos);
    left[newline_pos] = '\0';

    strncpy(right, &temp_buffer[newline_pos + 1],
            READLINE_READ_SIZE - newline_pos);
    right[READLINE_READ_SIZE - newline_pos - 1] = '\0';

    append_to_line(line, left);
    init_my_readline();
    update_readline_buffer(right);

    free(left), free(right);
}

int split_lines(int newline_pos, char *buffer, char **line)
{
    if ((newline_pos = has_newline(buffer)) > 0)
    {
        split_line(buffer, newline_pos, line);
        return 1;
    }
    else
    {
        append_to_line(line, buffer);
        return 0;
    }
}

char *my_readline(int fd)
{
    char *line = NULL;
    char *temp_buffer = malloc((READLINE_READ_SIZE + 1) * sizeof(char));
    int newline_pos = -1;

    if (readline_buffer[0] != '\0')
    {
        if (split_lines(newline_pos, readline_buffer, &line) == 1)
        {
            free(temp_buffer);

            return line;
        }
        init_my_readline();
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fd, temp_buffer, READLINE_READ_SIZE)) > 0)
    {
        temp_buffer[bytesRead] = '\0';

        if (split_lines(newline_pos, temp_buffer, &line) == 1)
        {
            free(temp_buffer);

            return line;
        }
    }

    free(temp_buffer);

    return line;
}

int main(int ac, char **av)
{
    if (ac == 3)
    {
        int fd = open(av[1], O_RDONLY);

        READLINE_READ_SIZE = atoi(av[2]);

        char *line = NULL;
        init_my_readline();

        while ((line = my_readline(fd)))
        {
            printf("%s\n", line);
            free(line);
        }
        close(fd);
    }
}