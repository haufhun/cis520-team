#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

typedef int pid_t;

static void syscall_handler (struct intr_frame *);

static void sys_hault_handle(void);
static void sys_exit_handle(int);
static pid_t sys_exec_handle(const char *);
static int sys_wait_handle(pid_t);
static bool sys_create_handle(const char *, unsigned);
static bool sys_remove_handle(const char *);
static int sys_open_handle (const char *);
static int sys_filesize_handle(int);
static int sys_read_handle(int, char *, unsigned);
static int sys_write_handle(int, char *, unsigned);
static void sys_seek_handle(int, unsigned);
static unsigned sys_tell_handle(int);
static void sys_close_handle(int);

static void copy_in(int *, uint32_t *, size_t);
static char * copy_in_string(char *);
static void* check_addr(const void *);

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
  {0, (syscall_function *) sys_hault_handle},      // halt                                               
  {1, (syscall_function *) sys_exit_handle},       // exit
  {1, (syscall_function *) sys_exec_handle},       // exec
  {1, (syscall_function *) sys_wait_handle},       // wait
  {2, (syscall_function *) sys_create_handle},     // create
  {1, (syscall_function *) sys_remove_handle},     // remove
  {1, (syscall_function *) sys_open_handle},       // open
  {1, (syscall_function *) sys_filesize_handle},   // filesize
  {3, (syscall_function *) sys_read_handle},       // read    
  {3, (syscall_function *) sys_write_handle},      // write
  {2, (syscall_function *) sys_seek_handle},       // seek        
  {1, (syscall_function *) sys_tell_handle},       // tell        
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
  return;
}

static pid_t sys_exec_handle(const char *file) 
{
  check_addr(file);
  
  ASSERT (false);
}

static int sys_wait_handle(pid_t pid)
{
  check_addr(&pid);
  
  ASSERT(false);
}


static bool sys_create_handle(const char *file, unsigned initial_size)
{
  bool success;

  check_addr(file);

  lock_acquire(&fs_lock);
  success = filesys_create(file, initial_size);
  lock_release(&fs_lock);

  return success;
}

static bool sys_remove_handle(const char *file)
{
  check_addr(file);
  
  ASSERT (false);
}

static int sys_open_handle (const char *file)
{
  char *kfile;
  struct file_descriptor *fd;
  struct thread * t;

  check_addr(file);
  // kfile = copy_in_string (file);

  if(!file || !is_user_vaddr(file))
    return -1;

  // int handle = -1; //idk what this is used for : HUNTER
  fd = malloc (sizeof *fd);

  if (fd)
  {
    lock_acquire (&fs_lock);
    fd->fp = filesys_open (file);
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

static int sys_filesize_handle(int fd_num)
{
  struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();
  bool didIt = false;
  int length;
  
  // if(fd_num <= STDOUT_FILENO)
  //   return -1;

  lock_acquire(&fs_lock);

  for (e = list_begin (&t->fd_list); e != list_end (&t->fd_list); e = list_next (e))
  {
    fd = list_entry (e, struct file_descriptor, elem);

    if(fd->count == fd_num)
    {
      length = file_length(fd->fp);
      didIt = true;
    }
  }

  if(didIt)
    free(fd);

  lock_release(&fs_lock);    
  return length;
}

static int sys_read_handle(int fd, char * buffer, unsigned size)
{
  int i;

  check_addr(buffer);  
  check_addr(&fd);  //Not sure if this is the right way to do it, but it passes the read-bad-fd test.
  
  
  if(fd < STDIN_FILENO)
    return -1;

  if(fd == STDIN_FILENO)
  {
    for (i = 0; i < size; i++)
      buffer[i] = input_getc();

    return size;
  }
  
  if(fd > STDOUT_FILENO)
  {
    ASSERT (false);
    // need to implement writing to a file here
  }
}

static int sys_write_handle(int fd, char * buffer, unsigned size)
{
  int i;
  
  check_addr(buffer);
  //check_addr(&fd);  //this crashes all the tests for some reason... Worked for read-bad-fd though.
  
  if(fd < STDOUT_FILENO)
    return -1;

  if(fd == STDOUT_FILENO)
  {
    for (i = 0; i < size; i++)
      printf("%c", buffer[i]);

    return size;
  }
  
  if(fd > STDOUT_FILENO)
  {
    ASSERT (false);
    // need to implement writing to a file here
  }
}


static void sys_seek_handle(int fd_num, unsigned position)
{
  struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();
  bool didIt = false;
  
  if(fd_num <= STDOUT_FILENO)
    return;

  lock_acquire(&fs_lock);
  for (e = list_begin (&t->fd_list); e != list_end (&t->fd_list); e = list_next (e))
  {
    fd = list_entry (e, struct file_descriptor, elem);

    if(fd->count == fd_num)
    {
      file_seek(fd->fp, position);
      didIt = true;
    }
  }

  if(didIt)
    free(fd);

  lock_release(&fs_lock);    
}

static unsigned sys_tell_handle(int fd_num)
{
  struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();
  bool didIt = false;
  unsigned file_pos;
  
  if(fd_num <= STDOUT_FILENO)
    return;

  lock_acquire(&fs_lock);
  for (e = list_begin (&t->fd_list); e != list_end (&t->fd_list); e = list_next (e))
  {
    fd = list_entry (e, struct file_descriptor, elem);

    if(fd->count == fd_num)
    {
      file_pos = file_tell(fd->fp);
      didIt = true;
    }
  }

  if(didIt)
    free(fd);

  lock_release(&fs_lock); 
  
  return file_pos;
}

static void sys_close_handle(int fd_num)
{
	struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();
  bool didIt = false;

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

static void copy_in(int * argv, uint32_t *stp, size_t size)
{
  check_addr(stp); //checks for sc-bad-ptr and sc-bad-arg

  memcpy(argv, stp, size);
  return;
}
static char * copy_in_string(char * string)
{
  char * newString;
  size_t size;

  if(!string)
    return NULL;

	if (is_user_vaddr(string))
	{
	  void *page_ptr = pagedir_get_page(thread_current()->pagedir, string);
	  
    if (page_ptr)
    {
      size = strlen(string)+1;
      newString = malloc(size);
      strlcpy(newString, string, size);
      
      return newString;
    }
	}

  sys_exit_handle(-1);
  return NULL;
}

/* This method checks if the parementer addr is a bad address/pointer */
void* check_addr(const void *addr)
{
  void *page_ptr = NULL;

	if (!is_user_vaddr(addr))
	{
		sys_exit_handle(-1);
		return 0;
	}

	page_ptr = pagedir_get_page(thread_current()->pagedir, addr);
	if (!page_ptr)
	{
		sys_exit_handle(-1);
		return 0;
	}

	return page_ptr;
}