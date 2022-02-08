# ifndef THREAD_POOL_H_
# define THREAD_POOL_H_

# include <thread>
# include <vector>
# include <queue>
# include <atomic>

class ThreadPool {

public:
    ThreadPool(size_t size = 4);
    ~ThreadPool();

    using Task = std::function<void()>;

    int addTask(const Task& f);
    int start();
    int stop();

private:
    void run();

private:
    unsigned int _size;

    std::vector<std::thread> _pool;
    std::queue<Task> _tasksQ;
    std::mutex _mtx;
    std::condition_variable _cv;

    std::atomic<bool> _run;
};

// # include "condition.h"


// typedef struct task {
//     void* (*fun)(void *args); // 函数指针，线程要执行的任务
//     void* args; // 参数
//     struct task* next;
// } task_t;

// typedef struct threadpool {
//     condition_t ready;
//     task_t *first;  //任务队列中第一个任务
//     task_t *last;   //任务队列中最后一个任务
//     int counter;    //线程池中已有线程数
//     int idle;       //线程池中空闲线程数
//     int max_threads;//线程池最大线程数
//     int quit;       //是否退出标志
// } threadpool_t;

// void threadpool_init(threadpool_t *pool, int threads);

// void threadpool_add_task(threadpool_t *pool, void *(*fun)(void* args), void *args);

// void threadpool_destory(threadpool_t *pool);

# endif
