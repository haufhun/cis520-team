#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  int i;
  printf("Hello World\n");
  // printf("size: %d\n", argc);

  // printf("Addr of argv = %x\n", &argv);
  // printf("Addr of argv = %x\n", argv);  
  // printf("Addr of argv = %x\n", argv-1);

  // for (i = 0; i < argc; i++)
  //   printf ("%s ", argv[i]);
  // printf ("\n");

  return EXIT_SUCCESS;
}
