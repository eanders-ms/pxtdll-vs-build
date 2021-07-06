#ifndef __PXT_THREAD_H__
#define __PXT_THREAD_H__

#if defined(__GNUC__)
#include <pthread.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

struct pthread_mutex_t {
    CRITICAL_SECTION mHandle;
    pthread_mutex_t();
    ~pthread_mutex_t();
};

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

struct pthread_cond_t {
    HANDLE mEvents[2];
    unsigned int mWaitersCount;
    CRITICAL_SECTION mWaitersCountLock;

    pthread_cond_t();
    ~pthread_cond_t();
};

int pthread_cond_signal(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);
int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* abstime);

typedef HANDLE pthread_t;
typedef void pthread_attr_t;

int pthread_create(pthread_t* __restrict thread,
    const pthread_attr_t* __restrict attr,
    void* (*start_routine)(void*),
    void* __restrict arg);
int pthread_join(pthread_t thread, void** retval);
__declspec(noreturn) void pthread_exit(void* retval);
pthread_t pthread_self();

#endif
#endif