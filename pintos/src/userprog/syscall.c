#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);

static void copy_in(int * argv, uint32_t * stp, size_t size);

static void sys_hault_handle(void);
static void sys_exit_handle(int status);
static int sys_open_handle (const char *ufile);
static void sys_write_handle(int fd, char * buffer, unsigned size);

static int sys_default (int arg0); // remove this later - after all sys calls implemented

// we may need a lock here.. for the file system
// struct lock fs_lock;


/* This Table and struct are taken from Project2Session.pdf
    Their implementations are done by ourselves. */

/* Prototype for a syscall function. */
typedef int syscall_function (int, int, int);

/* A system call. */
struct syscall
{
  size_t arg_cnt; /* Number of arguments. */
  syscall_function *func; /* Implementation. */
};

/* Table of system calls. */
static const struct syscall syscall_table[] =
{
  {1, (syscall_function *) sys_hault_handle},      // halt                                               
  {1, (syscall_function *) sys_exit_handle},       // exit
  {1, (syscall_function *) sys_default},           // exec
  {1, (syscall_function *) sys_default},           // wait
  {1, (syscall_function *) sys_default},           // create
  {1, (syscall_function *) sys_default},           // remove
  {1, (syscall_function *) sys_open_handle},       // open
  {1, (syscall_function *) sys_default},           // filesize
  {1, (syscall_function *) sys_default},           // read    
  {3, (syscall_function *) sys_write_handle},      // write
  {1, (syscall_function *) sys_default},           // seek        
  {1, (syscall_function *) sys_default},           // tell        
  {1, (syscall_function *) sys_default}            // close        
};

void syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  // lock_init(&fs_lock);
}

/* This method is taken from the Proj2Session.pdf 
    document. The handle methods are ipmlemented 
    on our own accord. */
static void syscall_handler (struct intr_frame *f)
{
  const struct syscall *sc;
  unsigned call_nr;
  int args[3];

  /* Get the system call. */
  copy_in (&call_nr, f->esp, sizeof(call_nr));
  
  if (call_nr >= sizeof (syscall_table) / sizeof (*syscall_table))
    thread_exit ();
  
  sc = syscall_table + call_nr;
  
  /* Get the system call arguments. */
  ASSERT (sc->arg_cnt <= sizeof (args) / sizeof (*args));
  memset (args, 0, sizeof (args));
  copy_in (args, (uint32_t *) f->esp + 1, sizeof(*args) * sc->arg_cnt);
  
  /* Execute the system call,
  and set the return value. */
  f->eax = sc->func (args[0], args[1], args[2]);
}

static void copy_in(int * argv, uint32_t *stp, size_t size)
{
  // need to add addressing checking here!!!!
  // look at pagedir.c and vaddr.h
  memcpy(argv, stp, size);
}

static void sys_hault_handle(void)
{
  shutdown_power_off();
}

static void sys_exit_handle(int status)
{
  struct thread *t = thread_current();

  t->parent->ex = true;
  t->exit_status = status;

  printf("%s: exit(%d)\n", t->name, status);
  thread_exit ();
}

static int sys_open_handle (const char *ufile)
{
// char *kfile = copy_in_string (ufile);
//
//  struct file_descriptor *fd;

// int handle = -1;
// fd = malloc (sizeof *fd);
// if (fd != NULL)
// {
// lock_acquire (&fs_lock);
// fd->file = filesys_open (kfile);
// if (fd->file != NULL)
// // ... add to list of fd's associated with thread
  return -1;
}

static void sys_write_handle(int fd, char * buffer, unsigned size)
{
  int i;

  for (i = 0; i < size; i++)
    printf("%c", buffer[i]);
}

static int sys_default (int arg0) // remove this later - after all sys calls implemented
{
  return -1;
}
