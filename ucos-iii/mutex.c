
#include "../panic.h"
#include "../mutex.h"
#include <os_cfg_app.h>

void os_mutex_init(mutex_t *mutex)
{
    OS_ERR err;

    OSMutexCreate(&mutex->ucos_mutex, "mutex", &err);

    if (err != OS_ERR_NONE) {
        PANIC("Mutex init: %d", err);
    }
}

void os_mutex_take(mutex_t *mutex)
{
    OS_ERR err;

    OSMutexPend(&mutex->ucos_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

    switch (err) {
        case OS_ERR_NONE:
        case OS_ERR_MUTEX_OWNER:
            return;
        default:
            PANIC("Mutex take: %d", err);
    }
}

bool os_mutex_try(mutex_t *mutex)
{
    OS_ERR err;

    OSMutexPend(&mutex->ucos_mutex, 0, OS_OPT_PEND_NON_BLOCKING, NULL, &err);

    switch (err) {
        case OS_ERR_NONE:
        case OS_ERR_MUTEX_OWNER:
            return true;
        case OS_ERR_PEND_WOULD_BLOCK:
            return false;
        default:
            PANIC("Mutex try: %d", err);
            return false;
    }
}

bool os_mutex_try_timeout(mutex_t *mutex, uint32_t timeout)
{
    OS_ERR err;

    /* convert timeout to ticks, rounding up */
    OS_TICK ticks = (OS_TICK) (timeout - 1) * OS_CFG_TICK_RATE_HZ / 1000000 + 1;

    OSMutexPend(&mutex->ucos_mutex, ticks, OS_OPT_PEND_BLOCKING, NULL, &err);

    switch (err) {
        case OS_ERR_NONE:
        case OS_ERR_MUTEX_OWNER:
            return true;
        case OS_ERR_TIMEOUT:
            return false;
        default:
            PANIC("Mutex try: %d", err);
            return false;
    }
}

void os_mutex_release(mutex_t *mutex)
{
    OS_ERR err;

    OSMutexPost(&mutex->ucos_mutex, OS_OPT_POST_NONE, &err);

    switch (err) {
        case OS_ERR_NONE:
        case OS_ERR_MUTEX_NESTING:
            return;
        case OS_ERR_MUTEX_NOT_OWNER:
            PANIC("Mutex not owned: %d", err);
        default:
            PANIC("Mutex release: %d", err);
    }
}
