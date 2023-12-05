#pragma once

#include <iostream>
#include <atomic>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>
#include <utility>

// we want to lock a thread to a specifc core to avoid context switching
// this function sets the thread affinity to a specific core_id
inline auto setThreadCore(int core_id) noexcept {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0);
}

// template that can take a functiona and variadic template arguments to the function
template<typename T, typename... A>
inline auto createAndStartThread(int core_id, const std::string &name, 
        T &&func, A &&... args) noexcept {
    std::atomic<bool> running(false), failed(false);
    auto thread_body = [&] {
        // attempts to set the core affinity of the thread
        if (core_id >= 0 && !setThreadCore(core_id)) {
            std::cerr << "Failed to set core affinity for " << name << " " 
                << pthread_self() << " to " << core_id << std::endl;
            failed = true;
            return;
        }
        std::cout << "Set core affinity for " << name << " " <<
            pthread_self() << " to " << core_id << std :: endl;
        running = true;

        // forwards function and its arguments to execution using perfect forwarding
        // perfect forwarding preserves lvalue/rvalue for compiler optimizations
        std::forward<T>(func)((std::forward<A>(args))...);
    };

    // starts the thread, waits for completion
    // periodically checks the status without busy waiting
    auto t = new std::thread(thread_body);
    while (!running && !failed) {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    // if setThreadCore() fails
    if (failed) {
        t->join();
        delete t;
        t = nullptr;
    }
    return t;
}
