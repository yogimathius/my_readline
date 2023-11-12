#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int READLINE_READ_SIZE = 512;

char *my_readline(int fd) {
    return 0;
}

int main(int ac, char **av) {
  char *str = NULL;

  int fd = open("./file.txt", O_RDONLY);
  while ((str = my_readline(fd)) != NULL)
  {
      printf("%s\n", str);
      free(str);
  }
  close(fd);
  //
  //  Yes it's also working with stdin :-)
  //  printf("%s", my_readline(0));
  //

  return 0;
}