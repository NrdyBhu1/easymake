/* thread.h - simple, lightweight, cross platform threading
 *
 * Copyright (c) 2021 Cleanware
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef THREAD_H
#define THREAD_H

#include <stdlib.h>

typedef void * thread_t;
typedef void * mutex_t;

/**
 * Creates a running thread and returns its handle.
*/
static thread_t create_thread(void *(*thread_func)(void *), void *arg);

/**
 * Waits until specified thread terminates.
*/
static void *join_thread(thread_t thread);

/**
 * Terminates the calling thread.
*/
static void exit_thread(void *data);

/**
 * Creates a mutex.
*/
static mutex_t create_mutex(void);

/**
 * Destroys a mutex.
*/
static int destroy_mutex(mutex_t mutex);

/**
 * Locks a mutex.
*/
static int lock_mutex(mutex_t mutex);

/**
 * Unlocks a mutex.
*/
static int unlock_mutex(mutex_t mutex);

/*----------------------------------------------------------------------------*/
/*                           Windows Implementation                           */
/*----------------------------------------------------------------------------*/

#if defined(_WIN32)

#include <windows.h>

static thread_t create_thread(void *(*func)(void *), void *arg)
{
    HANDLE thread_id;
    thread_id = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);

    return (thread_t)thread_id;
}

static void *join_thread(thread_t thread)
{
    return (void *)WaitForSingleObject((HANDLE)thread, INFINITE);
}

static void exit_thread(void *data)
{
    ExitThread((DWORD)data);
}

static mutex_t create_mutex(void)
{
    return (mutex_t)CreateMutex(NULL, FALSE, NULL);
}

static int destroy_mutex(mutex_t mutex)
{
    return CloseHandle((HANDLE)mutex);
}

static int lock_mutex(mutex_t mutex)
{
    return (int)WaitForSingleObject((HANDLE)mutex, INFINITE);
}

static int unlock_mutex(mutex_t mutex)
{
    return (int)ReleaseMutex((HANDLE)mutex);
}

#endif

/*---------------------------------------------------------------------------*/
/*                            Unix Implementation                            */
/*---------------------------------------------------------------------------*/

#if defined(unix) || defined(__unix__) || defined(__unix)

#include <pthread.h>

static thread_t create_thread(void *(*func)(void *), void *arg)
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, func, arg);

    return (thread_t)thread_id;
}

static void *join_thread(thread_t thread)
{
    void *data;
    pthread_join((pthread_t)thread, &data);

    return data;
}

static void exit_thread(void *data)
{
    pthread_exit(data);
}

static mutex_t create_mutex(void)
{
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    return (mutex_t)mutex;
}

static int destroy_mutex(mutex_t mutex)
{
    int result = pthread_mutex_destroy((pthread_mutex_t *)mutex);
    free(mutex);

    return result;
}

static int lock_mutex(mutex_t mutex)
{
    return pthread_mutex_lock((pthread_mutex_t *)mutex);
}

static int unlock_mutex(mutex_t mutex)
{
    return pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

#endif

#endif /* THREAD.H */
