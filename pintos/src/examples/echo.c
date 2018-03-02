#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  int i;
  printf("Hello World\n");
  // printf("First char: %s\n", argv[0]);

  // for (i = 0; i < argc; i++)
  //   printf ("%s ", argv[i]);
  // printf ("\n");

  return EXIT_SUCCESS;
}
