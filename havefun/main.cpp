#include "threadpool.h"
#include <iostream>
#include <unistd.h>

void func(int i, int j) {
    std::cout << "num: " << i * j << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    sleep(1);
}

int main(void) {
    ThreadPool pool(5);
    pool.start();

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto task = std::bind(func, i, i);
        pool.addTask(task);
    }

    pool.stop();

    return 0;
}