#ifdef _MSC_VER
#include "pxt.h"

pthread_mutex_t::pthread_mutex_t()
{
    InitializeCriticalSection(&mHandle);
}

pthread_mutex_t::~pthread_mutex_t()
{
    DeleteCriticalSection(&mHandle);
}

int pthread_mutex_lock(pthread_mutex_t* mutex)
{
    EnterCriticalSection(&mutex->mHandle);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
    LeaveCriticalSection(&mutex->mHandle);
    return 0;
}

#define _CONDITION_EVENT_ONE 0
#define _CONDITION_EVENT_ALL 1

pthread_cond_t::pthread_cond_t()
    : mWaitersCount(0)
{
    InitializeCriticalSection(&mWaitersCountLock);
    mEvents[_CONDITION_EVENT_ONE] = CreateEvent(NULL, FALSE, FALSE, NULL);
    mEvents[_CONDITION_EVENT_ALL] = CreateEvent(NULL, TRUE, FALSE, NULL);
}

pthread_cond_t::~pthread_cond_t()
{
    CloseHandle(mEvents[_CONDITION_EVENT_ONE]);
    CloseHandle(mEvents[_CONDITION_EVENT_ALL]);
    DeleteCriticalSection(&mWaitersCountLock);
}

int pthread_cond_signal(pthread_cond_t* cond)
{
    int haveWaiters;

    EnterCriticalSection(&cond->mWaitersCountLock);
    haveWaiters = (cond->mWaitersCount > 0);
    LeaveCriticalSection(&cond->mWaitersCountLock);

    if (haveWaiters)
    {
        if (SetEvent(cond->mEvents[_CONDITION_EVENT_ONE]) == 0)
        {
            return -1;
        }
    }

    return 0;
}

int pthread_cond_broadcast(pthread_cond_t* cond)
{
    int haveWaiters;

    EnterCriticalSection(&cond->mWaitersCountLock);
    haveWaiters = (cond->mWaitersCount > 0);
    LeaveCriticalSection(&cond->mWaitersCountLock);

    if (haveWaiters)
    {
        if (SetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0)
        {
            return -1;
        }
    }

    return 0;
}

static int pthread_cond_wait_common(pthread_cond_t* cond, pthread_mutex_t* mtx, DWORD timeout)
{
    DWORD result;
    int lastWaiter;

    EnterCriticalSection(&cond->mWaitersCountLock);
    ++cond->mWaitersCount;
    LeaveCriticalSection(&cond->mWaitersCountLock);

    pthread_mutex_unlock(mtx);

    result = WaitForMultipleObjects(2, cond->mEvents, FALSE, timeout);
    if (result == WAIT_TIMEOUT)
    {
        pthread_mutex_lock(mtx);
        return -2;
    }
    else if (result == WAIT_FAILED)
    {
        pthread_mutex_lock(mtx);
        return -1;
    }

    EnterCriticalSection(&cond->mWaitersCountLock);
    --cond->mWaitersCount;
    lastWaiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) &&
        (cond->mWaitersCount == 0);
    LeaveCriticalSection(&cond->mWaitersCountLock);

    if (lastWaiter)
    {
        if (ResetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0)
        {
            pthread_mutex_lock(mtx);
            return -1;
        }
    }

    pthread_mutex_lock(mtx);

    return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
    return pthread_cond_wait_common(cond, mutex, INFINITE);
}

int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* ts)
{
    struct timespec now;
    if (timespec_get(&now, TIME_UTC) == TIME_UTC)
    {
        unsigned long long nowInMilliseconds = now.tv_sec * 1000 + now.tv_nsec / 1000000;
        unsigned long long tsInMilliseconds = ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
        DWORD delta = (tsInMilliseconds > nowInMilliseconds) ?
            (DWORD)(tsInMilliseconds - nowInMilliseconds) : 0;
        return pthread_cond_wait_common(cond, mutex, delta);
    }
    else
        return -1;
}

typedef void* (*thread_fn)(void*);

struct thread_start_info {
    thread_fn fn;
    void* arg;
};

static DWORD WINAPI _thread_wrapper(LPVOID lpvarg)
{
    thread_start_info* tsi = (thread_start_info*)lpvarg;
    thread_fn fn = tsi->fn;
    void* arg = tsi->arg;
    delete tsi;

    void* res = fn(arg);

    return (DWORD)res;
}

int pthread_create(pthread_t* __restrict thread,
    const pthread_attr_t* __restrict attr,
    void* (*start_routine)(void*),
    void* __restrict arg)
{
    thread_start_info* tsi = new thread_start_info;
    tsi->fn = start_routine;
    tsi->arg = arg;

    *thread = CreateThread(NULL, 0, _thread_wrapper, (LPVOID)tsi, 0, NULL);

    if (!(*thread))
    {
        delete tsi;
        return -1;
    }
    return 0;
}

int pthread_join(pthread_t thread, void** res)
{
    DWORD dwRes;

    if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED)
    {
        return -1;
    }

    if (res)
    {
        if (GetExitCodeThread(thread, &dwRes) != 0)
        {
            *res = (void*)dwRes;
        }
        else
        {
            return -1;
        }
    }
    CloseHandle(thread);
    return 0;
}

__declspec(noreturn) void pthread_exit(void* res)
{
    ExitThread((DWORD)res);
}

pthread_t pthread_self()
{
    return GetCurrentThread();
}



#endif
