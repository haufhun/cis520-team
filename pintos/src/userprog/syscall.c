#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *);

static void copy_in(int *, uint32_t *, size_t);

static void sys_hault_handle(void);
static void sys_exit_handle(int );
static int sys_open_handle (const char *);
static void sys_write_handle(int, char *, unsigned);
static void sys_close_handle(int);

static int sys_default (int); // remove this later - after all sys calls implemented

static struct lock fs_lock;


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

/* A file descriptor */
struct file_descriptor 
{
  int count;
  struct file *fp;
  struct list_elem elem;
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
  {1, (syscall_function *) sys_close_handle}       // close        
};

void syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&fs_lock);
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
	if (is_user_vaddr(stp))
	{
	  void *page_ptr = pagedir_get_page(thread_current()->pagedir, stp);
	  
    if (page_ptr)
    {
      memcpy(argv, stp, size);
      return;
    }
	}

  sys_exit_handle(-1);
  return 0;
}

static void sys_hault_handle(void)
{
  shutdown_power_off();
}

static void sys_exit_handle(int status)
{
  struct thread *t = thread_current();

  t->parent->ex = true;

  printf("%s: exit(%d)\n", t->name, status);
  thread_exit ();
}

static int sys_open_handle (const char *ufile)
{
  char *kfile;
  struct file_descriptor *fd;
  struct thread * t;

  // kfile = copy_in_string (ufile);

  // int handle = -1; //idk what this is used for : HUNTER
  fd = malloc (sizeof *fd);

  if (fd)
  {
    lock_acquire (&fs_lock);
    fd->fp = filesys_open (ufile);
    lock_release (&fs_lock);
    
    if (fd->fp)
    {
      t = thread_current();
      fd->count = t->fd_count++;
      list_push_back (&t->fd_list, &fd->elem);
      return fd->count;
    }
  }
  
  return -1;
}

static void sys_write_handle(int fd, char * buffer, unsigned size)
{
  int i;
  
  
  if(fd < STDOUT_FILENO)
    return;

  if(fd == STDOUT_FILENO)
  {
    for (i = 0; i < size; i++)
      printf("%c", buffer[i]);

    return;
  }
  
  if(fd > STDOUT_FILENO)
  {
    ASSERT (false);
    // need to implement writing to a file here
  }
}

static void sys_close_handle(int fd_num)
{
	struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();
  bool didIt = false;

  printf("FD # = %d\n", fd_num);
  
  if(fd_num <= STDOUT_FILENO)
    return;

  lock_acquire(&fs_lock);
  
  for (e = list_begin (&t->fd_list); e != list_end (&t->fd_list); e = list_next (e))
  {
    fd = list_entry (e, struct file_descriptor, elem);

    if(fd->count == fd_num)
    {
      file_close(fd->fp);
      list_remove(e);
      didIt = true;
    }
  }

  if(didIt)
    free(fd);

  lock_release(&fs_lock);    
}

static int sys_default (int arg0) // remove this later - after all sys calls implemented
{
  return -1;
}
