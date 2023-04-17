#ifndef PENGUINS_THREADING_H
#define PENGUINS_THREADING_H

#ifdef _WIN32
# include <Windows.h>
#else
# include <pthread.h>
#endif

//lock
#ifdef _WIN32
# define peng_mutex_t CRITICAL_SECTION

# define peng_mutex_init(x) InitializeCriticalSection((x))
# define peng_mutex_destroy(x) DestroyCriticalSection((x))
# define peng_mutex_lock(x) EnterCriticalSection((x))
# define peng_mutex_trylock(x) TryEnterCriticalSection((x))
# define peng_mutex_unlock(x) LeaveCriticalSection((x))
#else
# define peng_mutex_t pthread_mutex_t

# define peng_mutex_init(x) pthread_mutex_init((x), NULL)
# define peng_mutex_destroy(x) pthread_mutex_destroy((x))
# define peng_mutex_lock(x) pthread_mutex_lock((x))
# define peng_mutex_trylock(x) (pthread_mutex_trylock((x)) != 0)
# define peng_mutex_unlock(x) pthread_mutex_unlock((x))
#endif


// thread
#ifdef _WIN32
# define peng_thread_t HANDLE
# define peng_thread_create(x, start, args) (x) = CreateThread(NULL,0, (start), (args), 0, &ThreadId)
# define peng_thread_exit(x, y) ExitThread((x))
# define peng_thread_join(x, y) do {                   \
        WaitForSingleObject((x), INFINITE);     \
        GetExitCodeThread((x), (y));            \
    }while(0)

#else
# define peng_thread_t pthread_t
# define peng_thread_create(x, start, args) pthread_create(&(x), NULL, (start), (args))
# define peng_thread_exit(x) pthread_exit((x))
# define peng_thread_join(x, y) pthread_join((x), (y))
#endif


// condition variable
#ifdef _WIN32
# define peng_condition_t CONDITION_VARIABLE

# define peng_cond_init(x) InitializeConditionVariable((x))
# define peng_cond_destroy(x) // nothing
# define peng_cond_signal(x) WakeConditionVariable((x))
# define peng_cond_wait(cond, mut) SleepConditionVariableCS((cond), (mut), INFINITE)

#else
# define peng_condition_t pthread_cond_t

# define peng_cond_init(x) pthread_cond_init((x), NULL)
# define peng_cond_destroy(x) pthread_cond_destroy((x))
# define peng_cond_signal(x) pthread_cond_signal((x))
# define peng_cond_wait(cond, mut) pthread_cond_wait((cond), (mut))
#endif



#endif // PENGUINS_THREADING_H
