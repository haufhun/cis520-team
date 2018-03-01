#include <stdio.h>
#include <syscall.h>


// #define syscall17(NUMBER, ARG0)                                           \
        // ({                                                               \
        //   int retval;                                                    \
        //   asm volatile                                                   \
        //     ("pushl %[arg0]; pushl %[number]; int $0x30; addl $8, %%esp" \
        //        : "=a" (retval)                                           \
        //        : [number] "i" (NUMBER),                                  \
        //          [arg0] "g" (ARG0)                                       \
        //        : "memory");                                              \
        //   retval;                                                        \
        // })

void prints(const char *s)
{
  char next_char;
      for(; *s; s++)
      {
        next_char = *s;
        __asm__ __volatile__ \
        (" movl $0xff, %%eax\n\
        int $0x30"
        : /* no output registers */
        : "b" (next_char)
        : "%eax"
        );
      }
}
void exitp(void)
{
   __asm__ __volatile__ \
   ("movl $0x00, %eax\n\
     int $0x30");
}
int
main (int argc, char **argv)
{
  // syscall17(0, 0); // test code here and above
  //int i;
  prints("Hello World");

  prints((char*)&argc);
  exitp();
        //__asm__ __volatile__("int $0x30");//his stuff
         //printf("Hello world");
        //__asm__ __volatile__("int $0x30"); // his stuff

  // for (i = 0; i < argc; i++)
  //   printf ("%s ", argv[i]);
  // printf ("\n");


  return EXIT_SUCCESS;
}
