#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <kernel/list.h>
#include "devices/pit.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"
  
/* See [8254] for hardware details of the 8254 timer chip. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

//#define TIME_EVENT 1

/* Number of timer ticks since OS booted. */
static int64_t ticks;

/* **> Our implementation. List of all blocked threads */
struct list sleep_list;

/* Number of loops per timer tick.
   Initialized by timer_calibrate(). */
static unsigned loops_per_tick;

static intr_handler_func timer_interrupt;
static bool too_many_loops (unsigned loops);
static void busy_wait (int64_t loops);
static void real_time_sleep (int64_t num, int32_t denom);
static void real_time_delay (int64_t num, int32_t denom);

/* **> Our implementation.*/
static bool sleep_ticks_less (const struct list_elem *a, 
                              const struct list_elem *b,
                              void *aux UNUSED);

/* Sets up the timer to interrupt TIMER_FREQ times per second,
   and registers the corresponding interrupt. */
void
timer_init (void) 
{
  pit_configure_channel (0, 2, TIMER_FREQ);
  intr_register_ext (0x20, timer_interrupt, "8254 Timer");
  list_init (&sleep_list); // initilized the list.(Was our main problem.)
}

/* Calibrates loops_per_tick, used to implement brief delays. */
void
timer_calibrate (void) 
{
  unsigned high_bit, test_bit;

  ASSERT (intr_get_level () == INTR_ON);
  printf ("Calibrating timer...  ");

  /* Approximate loops_per_tick as the largest power-of-two
     still less than one timer tick. */
  loops_per_tick = 1u << 10;
  while (!too_many_loops (loops_per_tick << 1)) 
    {
      loops_per_tick <<= 1;
      ASSERT (loops_per_tick != 0);
    }

  /* Refine the next 8 bits of loops_per_tick. */
  high_bit = loops_per_tick;
  for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
    if (!too_many_loops (high_bit | test_bit))
      loops_per_tick |= test_bit;

  printf ("%'"PRIu64" loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* Returns the number of timer ticks since the OS booted. */
int64_t
timer_ticks (void) 
{
  enum intr_level old_level = intr_disable ();
  int64_t t = ticks;
  intr_set_level (old_level);
  return t;
}

/* Returns the number of timer ticks elapsed since THEN, which
   should be a value once returned by timer_ticks(). */
int64_t
timer_elapsed (int64_t then) 
{
  return timer_ticks () - then;
}

/**************> Added this method <*****************/
static bool
sleep_ticks_less (const struct list_elem *a, const struct list_elem *b,
                  void *aux UNUSED)
{
  ASSERT (a != NULL);
  ASSERT (b != NULL);
  const struct thread *ta = list_entry (a, struct thread, wait_elem);
  const struct thread *tb = list_entry (b, struct thread, wait_elem);

  return ta->sleep_ticks < tb->sleep_ticks;
}

/* Sleeps for approximately TICKS timer ticks.  Interrupts must
   be turned on. */
void
timer_sleep (int64_t ticks) 
{
/************> old way <*************/
  //int64_t start = timer_ticks ();
  // while (timer_elapsed (start) < ticks) 
  // thread_yield ();
/***********>end old way <***********/ 

/*************> Our implementation <*****************/
  //thread_sleep (ticks);

  enum intr_level old_level;
  struct thread *t = thread_current ();

  ASSERT (intr_get_level () == INTR_ON);


  if (ticks <= 0)
    return;

  
  ASSERT (t->status == THREAD_RUNNING);

  //printf("\nThread is; %s", t->tid);
  //printf("\nThread to be awaken at: %llu\n", ticks+ timer_ticks());
  t->sleep_ticks = ticks + timer_ticks ();
  old_level = intr_disable (); // comment this out for semma
  //list_wakeup_ticks_insert(&sleep_list,&t->wait_elem);
  //list_wakeup_ticks_insert(&sleep_list,&t->elem);

  list_insert_ordered (&sleep_list, &t->wait_elem, sleep_ticks_less, NULL);

  sema_init (&t->sleep_sema, 0);
  sema_down (&t->sleep_sema);

  //thread_block (); // comment this out for semma

  intr_set_level (old_level);
/**********>End of our implementation <***************/
}

/* Sleeps for approximately MS milliseconds.  Interrupts must be
   turned on. */
void
timer_msleep (int64_t ms) 
{
  real_time_sleep (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts must be
   turned on. */
void
timer_usleep (int64_t us) 
{
  real_time_sleep (us, 1000 * 1000);
}

/* Sleeps for approximately NS nanoseconds.  Interrupts must be
   turned on. */
void
timer_nsleep (int64_t ns) 
{
  real_time_sleep (ns, 1000 * 1000 * 1000);
}

/* Busy-waits for approximately MS milliseconds.  Interrupts need
   not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_msleep()
   instead if interrupts are enabled. */
void
timer_mdelay (int64_t ms) 
{
  real_time_delay (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts need not
   be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_usleep()
   instead if interrupts are enabled. */
void
timer_udelay (int64_t us) 
{
  real_time_delay (us, 1000 * 1000);
}

/* Sleeps execution for approximately NS nanoseconds.  Interrupts
   need not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_nsleep()
   instead if interrupts are enabled.*/
void
timer_ndelay (int64_t ns) 
{
  real_time_delay (ns, 1000 * 1000 * 1000);
}

/* Prints timer statistics. */
void
timer_print_stats (void) 
{
  printf ("Timer: %"PRId64" ticks\n", timer_ticks ());
}

/* Timer interrupt handler. */
static void
timer_interrupt (struct intr_frame *args UNUSED)
{
  struct thread * next_thread;
  struct list_elem * el;

  ticks++;
  thread_tick ();

  while (!list_empty(&sleep_list))
  {
    el = list_pop_front(&sleep_list);
    next_thread = list_entry(el,struct thread, wait_elem);
    if(ticks < next_thread->sleep_ticks)
     { 
       list_push_front(&sleep_list,el);
       break;
     }
      //printf("timer_interupt(): Thread ready to be unblocked\n");
      sema_up(&next_thread->sleep_sema);
      //list_remove(&next_thread->wait_elem);
  } //his code from yesterday.

/*****************> Our implementation <***************/ 
/********> Checks and wakes up sleeping threads <******/
  // struct list_elem *elem_cur;
  // struct thread *t;
  // bool preempt = false;

  // while (!list_empty(&sleep_list))
  // {
  //   elem_cur = list_front (&sleep_list);
  //   t = list_entry (elem_cur, struct thread, wait_elem);
  //   if (t->sleep_ticks > ticks)
  //     break;

  //   list_remove (elem_cur);
  //   //thread_unblock (t);
  //   sema_up (&t->sleep_sema);
  //   preempt = true;
  // }

  //  if (preempt)
  //    intr_yield_on_return ();


/***> Below this is the original copy from thread.c <***/
/*******************> Also works <**********************/
   
  // struct list_elem *elem_cur; 
  // struct list_elem *elem_next;
  // struct thread *t;
  // enum intr_level old_level;

  // if (list_empty (&sleep_list))
  //   return;

  // elem_cur = list_begin (&sleep_list);
  // while (elem_cur != list_end (&sleep_list))
  // {
  //   elem_next = list_next (elem_cur);
  //   t = list_entry (elem_cur, struct thread, elem);
  //   if (t->sleep_ticks > timer_ticks())
  //     break;
    
  //   old_level = intr_disable (); // comment this out for semma
  //   list_remove (elem_cur);
  //   //sema_up (&t->sleep_sema);
  //   thread_unblock (t); // comment this out for semma
  //   intr_set_level (old_level); // comment this out for semma

  //   elem_cur = elem_next;    
  // }



  /*************> End of our implementation <***************/

 }

/* Returns true if LOOPS iterations waits for more than one timer
   tick, otherwise false. */
static bool
too_many_loops (unsigned loops) 
{
  /* Wait for a timer tick. */
  int64_t start = ticks;
  while (ticks == start)
    barrier ();

  /* Run LOOPS loops. */
  start = ticks;
  busy_wait (loops);

  /* If the tick count changed, we iterated too long. */
  barrier ();
  return start != ticks;
}

/* Iterates through a simple loop LOOPS times, for implementing
   brief delays.

   Marked NO_INLINE because code alignment can significantly
   affect timings, so that if this function was inlined
   differently in different places the results would be difficult
   to predict. */
static void NO_INLINE
busy_wait (int64_t loops) 
{
  while (loops-- > 0)
    barrier ();
}

/* Sleep for approximately NUM/DENOM seconds. */
static void
real_time_sleep (int64_t num, int32_t denom) 
{
  /* Convert NUM/DENOM seconds into timer ticks, rounding down.
          
        (NUM / DENOM) s          
     ---------------------- = NUM * TIMER_FREQ / DENOM ticks. 
     1 s / TIMER_FREQ ticks
  */
  int64_t ticks = num * TIMER_FREQ / denom;

  ASSERT (intr_get_level () == INTR_ON);
  if (ticks > 0)
    {
      /* We're waiting for at least one full timer tick.  Use
         timer_sleep() because it will yield the CPU to other
         processes. */                
      timer_sleep (ticks); 
    }
  else 
    {
      /* Otherwise, use a busy-wait loop for more accurate
         sub-tick timing. */
      real_time_delay (num, denom); 
    }
}

/* Busy-wait for approximately NUM/DENOM seconds. */
static void
real_time_delay (int64_t num, int32_t denom)
{
  /* Scale the numerator and denominator down by 1000 to avoid
     the possibility of overflow. */
  ASSERT (denom % 1000 == 0);
  busy_wait (loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000)); 
}
