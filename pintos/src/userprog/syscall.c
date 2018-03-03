#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);
static void sys_write_handle(int fd, char * buffer, unsigned size);
static void sys_exit_handle(int status);
// we may need a lock here.. for the file system
// struct lock fs_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  // lock_init(&fs_lock);
}

static void
syscall_handler (struct intr_frame *f) 
{
  int * stack_pointer;

  stack_pointer = f->esp;

  switch(*stack_pointer) {
    case SYS_HALT: // 0
      shutdown_power_off();
      break;
    case SYS_EXIT: // 1
      // f->eax = (uint32_t) *(f->esp + 1); // this is needed if we need to return the exit status. Idk if we do
      sys_exit_handle(*(stack_pointer+1));
      break;
    case SYS_WRITE: // 9
      sys_write_handle((int) *(stack_pointer+1), (char *) *(stack_pointer+2), (unsigned) *(stack_pointer +3));
      break;
    default:
      printf ("system call!\n");
      break;
  }
}

static void sys_write_handle(int fd, char * buffer, unsigned size)
{
  int i;

  for (i = 0; i < size; i++)
    printf("%c", buffer[i]);
}

static void sys_exit_handle(int status)
{
  struct thread *t = thread_current();

  t->parent->ex = true;
  t->exit_status = status;
  
  thread_exit ();
}