/* Tests timer_sleep(-100).  Only requirement is that it not crash. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"

void thread1();
void thread2();
void thread3();
struct lock lock1;

void
test_my_alarm (void) 
{
  timer_sleep(1000);
  printf("test_my_alarm(): Thread actually awoken at: %llu\n",timer_ticks());
  timer_sleep(1000);
  printf("test_my_alarm(): Thread actually awoken at: %llu\n",timer_ticks());
  timer_sleep(1000);
  printf("test_my_alarm(): Thread actually awoken at: %llu\n",timer_ticks());
  
  pass ();
}
void test_my_thread()
{
  thread_set_priority(PRI_MAX);

  lock_init(&lock1);

  thread_create ("Thread1", PRI_DEFAULT, thread1, NULL);
  timer_sleep(10); // main thread goes to sleep

  thread_create ("Thread2", PRI_DEFAULT+1, thread2, NULL);
  timer_sleep(10);// main thread goes to sleep
  thread_create ("Thread3", PRI_MAX, thread3, NULL);
  printf("All threads created! main thread now exiting..\n");

  return;
}
void thread1()
{
  static uint8_t once = 1;
  while(1){
    printf("thread 1 executing...\n" );
    my_lock_acquire(&lock1);
    printf("thread 1: lock acquried!\n" );
    if(once)
    {
      printf("thread 1 going to sleep \n");
      timer_sleep(100);
      once = 0;
    }
    printf("thead 1 tyring to release the lock");
    lock_release(&lock1);
    printf("thead 1 relaesed the lock!\n");
  }
}
void thread2()
{
  while(1){
    // printf("thread 2 executing...\n" );
    // timer_sleep(10);
  }
}
void thread3()
{
  while(1){
    printf("thread 3 executing...\n" );
    my_lock_acquire(&lock1);
    printf("thread 3: lock acquired!\n");
    lock_release(&lock1);
    printf("Thread 3 released the lock and now exiting..\n");
    break;
  }
}