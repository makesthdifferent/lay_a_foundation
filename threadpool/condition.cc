# ifndef _CONDITION_CPP_
# define _CONDITION_CPP_

# include "condition.h"

int condition_init(condition_t *cond) {
    int status;
    if ((status = pthread_mutex_init(&cond->pmutex, NULL))) {
        return status;
    }
    if ((status = pthread_cond_init(&cond->pcond, NULL))) {
        return status;
    }
    return 0;
}

int condition_lock(condition_t *cond) {
    return pthread_mutex_lock(&cond->pmutex);
}

int condition_unlock(condition_t *cond) {
    return pthread_mutex_unlock(&cond->pmutex);
}

int condition_wait(condition_t *cond) {
    return pthread_cond_wait(&cond->pcond, &cond->pmutex);
}

int condition_timewait(condition_t *cond, const struct timespec *abstime) {
    return pthread_cond_timedwait(&cond->pcond, &cond->pmutex, abstime);
}

//唤醒一个睡眠线程
int condition_signal(condition_t *cond) {
    return pthread_cond_signal(&cond->pcond);
}

//唤醒所有睡眠线程
int condition_broadcast(condition_t *cond) {
    return pthread_cond_broadcast(&cond->pcond);
}

//释放
int condition_destory(condition_t *cond) {
    int status;
    if ((status = pthread_mutex_destroy(&cond->pmutex))) {
        return status;
    }
    if ((status = pthread_cond_destroy(&cond->pcond))) {
        return status;
    }
    return 0;
}

# endif