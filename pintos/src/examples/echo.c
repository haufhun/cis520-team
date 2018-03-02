#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  printf("Hello World\n");

  // for (i = 0; i < argc; i++)
  //   printf ("%s ", argv[i]);
  // printf ("\n");

  return EXIT_SUCCESS;
}
