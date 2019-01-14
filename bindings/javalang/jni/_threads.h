/**
 * @file _threads.h
 * @author Gregg Reynolds
 * @date December 2016
 *
 * @brief macros to generalize thread operations
 */

/* TODO: use tinycthread? https://github.com/tinycthread/tinycthread */

#ifndef _threads_h
#define _threads_h

#include "openocf.h"

/* #if defined(HAVE_PTHREAD_H) */
#if defined(HAVE_LIBPTHREAD)
#include <pthread.h>

#define THREAD_T pthread_t

#define THREAD_LOCAL _Thread_local /* C11 */

/* pthread_t pthread_self(void); */
#define THREAD_ID    pthread_self()

#define THREAD_EXIT_T void*
#define THREAD_EXIT_OK NULL

#define THREAD_START_T THREAD_EXIT_T (*)(void *)

/* int pthread_create(pthread_t *restrict thread, */
/* 		   const pthread_attr_t *restrict attr, */
/* 		   void *(*start_routine)(void *), */
/* 		   void *restrict arg); */
#define THREAD_CREATE_DEFAULT(tid, start_routine, args) pthread_create(tid, NULL, start_routine, args)

/* void pthread_exit(void *value_ptr); */
#define THREAD_EXIT(val_ptr) pthread_exit(val_ptr)

/* int pthread_join(pthread_t thread, void **value_ptr); */
#define THREAD_JOIN pthread_join

/* end HAVE_PTHREAD_h */
/* **************************************************************** */
#elif defined(HAVE_C11_THREAD_H) || defined(HAVE_TINYCTHREAD)

/* UNTESTED */

#include <thread.h>

#define THREAD_T thrd_t

#define THREAD_EXIT_T int
#define THREAD_RETURN_OK 0

/* thrd_start_t = int (*)(void*)  */
#define THREAD_START_T thrd_start_t

#define THREAD_LOCAL _Thread_local

/* thrd_t thrd_current(void); */
#define THREAD_ID    thrd_current()

/* int thrd_create(thrd_t *thr, thrd_start_t func, void *arg); */
#define THREAD_CREATE_DEFAULT(tid, start_routine, args) thrd_create(tid, start_routine, args)

/* _Noreturn void thrd_exit(int res); */
#define THREAD_EXIT(val) thrd_exit(val)

/* int thrd_join(thrd_t thr, int *res); */
#define THREAD_JOIN thrd_join

/* end HAVE_C11_THREAD or HAVE_TINYCTHREAD */
#endif



#endif
