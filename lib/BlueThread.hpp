#ifndef BLUETHREAD_HPP
#define BLUETHREAD_HPP

#include <thread>
#include <atomic>
#include <functional>
#include <future>
#include <tuple>
#include <mutex>
#include <condition_variable>

class BlueThread {
public:
    BlueThread();
    ~BlueThread();

    template<typename Func, typename... Args>
    void set(Func&& func, Args&&... args);

    void run();

    template<typename Func, typename... Args>
    auto run(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>;

    void stop();
    bool isRunning() const;
    bool shouldStop() const;

private:
    std::thread thread;
    std::atomic<bool> running;
    std::atomic<bool> stopRequested;
    std::mutex mtx;

    std::function<void()> taskWrapper;

    std::condition_variable cv;
    std::mutex cvMutex;
};

#include "BlueThread.inl"

#endif // BLUETHREAD_HPP
