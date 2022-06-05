#pragma once
#include <thread>
#include "Awaitable.hpp"
class Thread : public Awaitable {
    std::thread* native_thread = nullptr;
public:
    void join() {
        if (native_thread != nullptr) {
            if (native_thread->joinable()) native_thread->join();
        }
    }
    bool joinable() {
        if (native_thread == nullptr) return false;
        return native_thread->joinable();
    }
    ~Thread() {
        detach();
    }
    void detach() {
        if (native_thread != nullptr) {
            if (native_thread->joinable()) {
                native_thread->detach();
            }
            delete native_thread;
            native_thread = nullptr;
        }
    }
    template <typename T, typename... Args>
    void create(T (*func)(Args...)) {
        detach();
        native_thread = new std::thread(func);
    }
    Thread(std::thread* thr) {
        native_thread = thr;
    }
    Thread() {}
};