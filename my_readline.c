#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int READLINE_READ_SIZE = 512;

char readline_buffer[512];

void init_my_readline() { readline_buffer[0] = '\0'; }

char *my_readline(int fd) {
  // printf("%d\n", fd);
  int k = 0;
  //   int bytes_read;
  //   do {
  read(fd, readline_buffer, 10);
  k++;
  // for (int i = 0; i < 10; i++) {
  //   printf("%c", readline_buffer[i]);
  // }
  //   } while (bytes_read != 0);
  return readline_buffer;
}

// char *my_readline(int fd) {
//   // printf("%d\n", fd);
//   int k = 0;
//   int bytes_read;
//   do {
//     bytes_read = read(fd, readline_buffer, 10);
//     k++;
//     for (int i = 0; i < 10; i++) {
//       printf("%c", readline_buffer[i]);
//     }
//   } while (bytes_read != 0);
//   return readline_buffer;
// }

int main(int ac, char **av) {
  char *str = NULL;
  printf("ac: %d\n", ac);
  const char *file_path = av[1];
  const char *buffer_size = av[2];
  printf("file_path: %s\n", file_path);
  printf("buffer_size: %s\n", buffer_size);

  int fd = open(file_path, O_RDONLY);
  while ((str = my_readline(fd)) != NULL) {
    printf("%s\n", str);
    // if (str != NULL) {
    printf("trying to free: %s\n", str);
    free(str);
    // }
  }
  close(fd);
  //
  //  Yes it's also working with stdin :-)
  //    printf("%s", my_readline(0));
  //

  return 0;
}