#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <tuple>

class Awaitable {
public:
    void wait() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck);
    }
    void wait(long time) {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait_for(lck,std::chrono::milliseconds(time));
    }
    void notify_one() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.notify_one();
    }
    void notify_all() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.notify_all();
    }
private:
    std::mutex mtx;
    std::condition_variable cv;
};
template<typename... Args>
class ArgumentedAwaitable : private Awaitable {
    std::tuple<Args...> args;
public:
    std::tuple<Args...> wait() {
        Awaitable::wait();
        return args;
    }
    void notify_one(Args... args) {
        this->args = std::make_tuple(args...);
        Awaitable::notify_one();
    }
    void notify_all(Args... args) {
        this->args = std::make_tuple(args...);
        Awaitable::notify_all();
    }
};