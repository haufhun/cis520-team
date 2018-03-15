#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
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
void sys_exit_handle(int);
static pid_t sys_exec_handle(const char *);
int sys_wait_handle(pid_t);
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
static void * check_addr(const void *);
static struct file_descriptor * search_list(int, bool);
void close_all_files(struct list*);



/* Prototype for a syscall function. */
typedef int syscall_function (int, int, int);

/* A system call structure.*/
struct syscall
{
  size_t arg_cnt; /* Number of arguments. */
  syscall_function *func; /* Implementation. */
};

/* A file descriptor structure. */
struct file_descriptor 
{
  int count;
  struct file *fp;
  struct list_elem elem;
};



/* Table of system calls. This struct is taken from Project2Session.pdf. */
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

/* This method is taken from the Proj2Session.pdf document. 
    The handle methods are ipmlemented on our own accord. */
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
  
  /* Execute the system call, and set the return value. */
  f->eax = sc->func (args[0], args[1], args[2]);
}

static void sys_hault_handle(void)
{
  shutdown_power_off();
}

void sys_exit_handle(int status)
{
  struct list_elem *e;
  struct list * c_list;
  struct child *f;
  int i = 0;

  // while (i < 100)
  //   i++;

  c_list = &thread_current()->parent->child_list;

  for (e = list_begin (c_list); e != list_end (c_list); e = list_next (e))
  {
    struct child *f = list_entry (e, struct child, elem);

    if(f->tid == thread_current()->tid)
    {
      f->has_exited = true;
      f->exit_status= status;
    }
  }

	thread_current()->exit_status = status;

	if(thread_current()->parent->wait_child_pid == thread_current()->tid)
		sema_up(&thread_current()->parent->child_wait_sema);

  printf("%s: exit(%d)\n", thread_current()->name, status);
	thread_exit();
}

static pid_t sys_exec_handle(const char *file) 
{
  // check_addr(file);
  char i;
  size_t size = sizeof(file);
  for(i = 0; i < size; i++) // If you change this to size thhan it fails recurse, and passes syn-read
  {
    check_addr(file+i);    
  }
  

  lock_acquire(&fs_lock);
	char * fn_cp = malloc (strlen(file)+1);
  strlcpy(fn_cp, file, strlen(file)+1);
  
  char * save_ptr;
  fn_cp = strtok_r(fn_cp," ",&save_ptr);

  struct file* f = filesys_open (fn_cp);

  if(f==NULL)
  {
    lock_release(&fs_lock);
    return -1;
  }
  else
  {
    file_close(f);
    lock_release(&fs_lock);
    return process_execute(file);
  }
}

int sys_wait_handle(pid_t pid)
{
    struct list_elem *e;

  struct child *ch=NULL;
  struct list_elem *e1=NULL;

  for (e = list_begin (&thread_current()->child_list); e != list_end (&thread_current()->child_list); e = list_next (e))
  {
    struct child *f = list_entry (e, struct child, elem);
    if(f->tid == pid)
    {
      ch = f;
      e1 = e;
    }
  }


  if(!ch || !e1)
    return -1;

  thread_current()->wait_child_pid = ch->tid;
    
  if(!ch->has_exited)
    sema_down(&thread_current()->child_wait_sema);

  int temp = ch->exit_status;
  list_remove(e1);
  
  return temp;
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
  bool success;

  check_addr(file);

  lock_acquire(&fs_lock);
  success = filesys_remove(file);
  lock_release(&fs_lock);
  
  return success;
}

static int sys_open_handle (const char *file)
{
  struct file_descriptor *fd;
  struct thread * t;

  check_addr(file);

  if(!file || !is_user_vaddr(file))
    return -1;

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
  struct file_descriptor *fd;  
  int length;

  if(fd_num <= STDOUT_FILENO)
  return;


  lock_acquire(&fs_lock);
  fd = search_list(fd_num, false);
  if(fd)
  {
    length = file_length(fd->fp);
  }
  lock_release(&fs_lock);
  
  return length;
}

static int sys_read_handle(int fd_num, char * buffer, unsigned size)
{
  struct file_descriptor *fd;  
  int bytes_read = -1;

  check_addr(buffer);    
  
  if(fd_num == STDIN_FILENO)
  {
    int i;
    for (i = 0; i < size; i++)
      buffer[i] = input_getc();

    return size;
  }
  
  if(fd_num > STDOUT_FILENO)
  {
    lock_acquire(&fs_lock);
    fd = search_list(fd_num, false);
    if(fd)
    {
      bytes_read = file_read(fd->fp, buffer, size);
    }
    lock_release(&fs_lock);
  }
  return bytes_read;
}

static int sys_write_handle(int fd_num, char * buffer, unsigned size)
{
  struct file_descriptor *fd;   
  int bytes_read = -1;
  
  check_addr(buffer);
  
  if(fd_num == STDOUT_FILENO)
  {
    int i;
    for (i = 0; i < size; i++)
      printf("%c", buffer[i]);

    return size;
  }
  
  if(fd_num > STDOUT_FILENO)
  {
    lock_acquire(&fs_lock);
    fd = search_list(fd_num, false);
    if(fd)
    {
      bytes_read = file_write(fd->fp, buffer, size);
    }
    lock_release(&fs_lock);
  }
  return bytes_read;  
}

static void sys_seek_handle(int fd_num, unsigned position)
{
  struct file_descriptor *fd;
  
  if(fd_num <= STDOUT_FILENO)
    return;


  lock_acquire(&fs_lock);
  fd = search_list(fd_num, false);
  if(fd)
  {
    file_seek(fd->fp, position);
  }

  lock_release(&fs_lock);
  
  return;
}

static unsigned sys_tell_handle(int fd_num)
{
  struct file_descriptor *fd;  
  unsigned file_pos;

  if(fd_num <= STDOUT_FILENO)
    return;


  lock_acquire(&fs_lock);
  fd = search_list(fd_num, false);
  if(fd)
  {
    file_pos = file_tell(fd->fp);
  }
  lock_release(&fs_lock);
  
  return file_pos;
}

static void sys_close_handle(int fd_num)
{
  struct file_descriptor *fd;

  if(fd_num <= STDOUT_FILENO)
    return;


  lock_acquire(&fs_lock);
  fd = search_list(fd_num, true);
  if(fd)
  {
    file_close(fd->fp);
    free(fd);
  }
  lock_release(&fs_lock);
  
  return;
}

static void copy_in(int * argv, uint32_t *stp, size_t size)
{
  int i;
  
  for(i = 0; i < size; i++)
    check_addr(stp+i); //checks for sc-bad-ptr and sc-bad-arg

  memcpy(argv, stp, size);
  return;
}

/* This method checks if the parementer addr is a bad address/pointer */
static void* check_addr(const void *addr)
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

/* Searches through the lsit for the correspending file_descriptor then returns it */
static struct file_descriptor* search_list(int fd_num, bool remove_list_elem)
{
  struct list_elem *e;
	struct file_descriptor *fd;
  struct thread *t = thread_current ();

  for (e = list_begin (&t->fd_list); e != list_end (&t->fd_list); e = list_next (e))
  {
    fd = list_entry (e, struct file_descriptor, elem);

    if(fd->count == fd_num)
    {
      if(remove_list_elem) 
        list_remove(e);

      return fd;
    }
  }
  return NULL;
}

void close_all_files(struct list* files)
{

	struct list_elem *e;

	while(!list_empty(files))
	{
		e = list_pop_front(files);

		struct file_descriptor *fd = list_entry (e, struct file_descriptor, elem);
          
    file_close(fd->fp);
    list_remove(e);
    free(fd);
	}     
}