#ifndef BLUETHREAD_INL
#define BLUETHREAD_INL

template<typename Func, typename... Args>
void BlueThread::set(Func&& func, Args&&... args) {
    std::lock_guard<std::mutex> lock(mtx);
    taskWrapper = [this, func = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
        stopRequested = false;
        func(std::forward<Args>(args)...);
    };
}

template<typename Func, typename... Args>
auto BlueThread::run(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    stop();

    using ReturnType = decltype(func(args...));
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [this, func = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
            stopRequested = false;
            return func(std::forward<Args>(args)...);
        }
    );

    std::future<ReturnType> result = task->get_future();

    running = true;
    thread = std::thread([this, task]() mutable {
        (*task)();
        running = false;
    });

    return result;
}

#endif // BLUETHREAD_INL
