# ifndef THREAD_POOL_CPP_
# define THREAD_POOL_CPP_

#include "threadpool.h"
#include <stdio.h>
#include <iostream>

ThreadPool::ThreadPool(size_t size) : _size(size) {
    _run.store(false);
}

ThreadPool::~ThreadPool() {
    _run.store(false);
    _cv.notify_all();
    for (auto& thread: _pool) {
        if (thread.joinable())
            thread.join();
    }
}

int ThreadPool::addTask(const Task& t) {
    if (!_run) {
        return -1;
    }
    std::unique_lock<std::mutex> lck(_mtx);
    while (_tasksQ.size() == _size) {
        _cv.wait(lck);
    }

    _tasksQ.push(t);
    _cv.notify_one();
    return 0;
}

int ThreadPool::start() {
    if (_run) {
        return -1;
    }

    for (int i = 0; i < _size; ++i) {
        _pool.push_back(std::thread(&ThreadPool::run, this));
    }
    _run.store(true);
    return 0;
}

void ThreadPool::run() {
    while(_run || !_tasksQ.empty()) {
        std::unique_lock<std::mutex> lck(_mtx);
        while (_tasksQ.size() == 0) {
            _cv.wait(lck);
        }

        Task t;
        t = std::move(_tasksQ.front());
        _tasksQ.pop();

        lck.unlock();
        t();
        std::cout << "run finish pid: " << std::this_thread::get_id() << std::endl;
        _cv.notify_one();
    }
}

int ThreadPool::stop() {
    if (!_run) {
        return -1;
    }
    _run.store(false);
    _cv.notify_all();
    for (auto& thread : _pool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    return 0;
}

// void ThreadPool::schedule() {
//     while (true) {
//         std::unique_lock<std::mutex> lock{_tasks};
//     }
// }

// void threadpool_init(threadpool_t* pool, int threads) {
//     condition_init(&pool->ready);
//     pool->first = nullptr;
//     pool->last = nullptr;
//     pool->counter = 0;
//     pool->idle = 0;
//     pool->max_threads = threads;
//     pool->quit = 0;
// }

// void thread_routine(void *args) {
//     struct timespec abstime;
//     int timeout;
//     printf("thread %d is starting\n", (int)getpid());
//     threadpool *pool = (threadpool_t *)args;
//     while (1) {
//         timeout = 0;
//         condition_lock(&pool->ready);
//         pool->idle++;

//         while (pool->first == nullptr && !pool->quit) {
//             printf("thread %d is waiting\n", (int)getpid());
//             clock_gettime(CLOCK_REALTIME, &abstime);
//             abstime.tv_sec += 2;
//             int status = condition_timewait(&pool->ready, &abstime);
//             if (status == ETIMEDOUT) {
//                 printf("thread %d wait timeout\n", (int)pthread_selt());
//                 timeout = 1;
//                 break;
//             }
//         }
//         pool->idle--;
//         if (pool->first) {
//             //取出等待队列最前的任务，移除任务，并执行任务
//             task_t *task = pool->first;
//             pool->first = task->next;
//             //由于任务执行需要消耗时间，先解锁让其他线程访问线程池
//             condition_unlock(&pool->ready);
            
//             // 执行
//             task->fun(task->args);

//             free(task);
//             condition_lock(&pool->ready);
//         }
//         //退出线程池
//         if (pool->quit && pool->first == nullptr) {
//             pool->counter--;
//             //若线程池中没有线程，通知等待线程（主线程）全部任务已经完成
//             if (pool->counter == 0) {
//                 condition_signal(&pool->ready);
//             }
//             condition_unlock(&pool->ready);
//             break;
//         }
//         if (timeout == 1) {
//             pool->counter--;
//             condition_unlock(&pool->ready);
//             break;
//         }
//         condition_unlock(&pool->ready);
//     }

//     printf("thread %d is exiting\n", (int)getpid());
//     return nullptr;
// }

// void threadpool_add_task(threadpool_t* pool, void* (*fun)(void* args), void* args) {
//     task_t* newtask = (task_t *)malloc(sizeof(task_t));
//     newtask->fun = fun;
//     newtask->args = args;
//     newtask->next = nullptr;

//     condition_lock(&pool->ready);

//     if (pool->first == nullptr) {
//         pool->first = newtask;
//     } else {
//         pool->last->next = newtask;
//     }
//     pool->last = newtask;

//     if (pool->idle > 0) {
//         condition_signal(&pool->ready);
//     } else if (pool->counter < pool->max_threads) {
//         pthread_t tid;
//         pthread_create(&tid, nullptr, thread_routine, pool);
//         pool->counter++;
//     }
//     condition_unlock(&pool->ready);
// }

// void threadpool_destory(threadpool_t *pool) {
//     if (pool->quit) {
//         return ;
//     }
    
//     condition_lock(&pool->ready);
//     pool->quit = 1;
//     if (pool->counter > 0) {
//         if (pool->idle > 0) {
//             condition_broadcast(&pool->ready);
//         }
//         while (pool->counter) {
//             condition_wait(&pool->ready);
//         }
//     }
//     condition_unlock(&pool->ready);
//     condition_destory(&pool->ready);
// }

# endif
