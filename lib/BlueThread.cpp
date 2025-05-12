#include "BlueThread.hpp"

BlueThread::BlueThread() : running(false), stopRequested(false) {}

BlueThread::~BlueThread() {
    stop();
}

void BlueThread::run() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!taskWrapper) return;

    stop();
    running = true;
    stopRequested = false;

    thread = std::thread([this]() {
        try {
            taskWrapper();
        } catch (...) {}
        running = false;
    });
}

void BlueThread::stop() {
    {
        std::lock_guard<std::mutex> lock(cvMutex);
        stopRequested = true;
        cv.notify_all();
    }

    if (thread.joinable()) {
        thread.join();
    }

    running = false;
}

bool BlueThread::isRunning() const {
    return running;
}

bool BlueThread::shouldStop() const {
    return stopRequested;
}
