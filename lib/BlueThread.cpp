#include "BlueThread.hpp"

#ifdef __linux__
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

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

#ifdef __linux__
double BlueThread::getThreadCpuTime() {
    if (!thread.joinable()) return 0.0;

    std::ostringstream path;
    path << "/proc/self/task/" << thread.native_handle() << "/stat";

    std::ifstream file(path.str());
    if (!file.is_open()) return 0.0;

    std::string token;
    for (int i = 0; i < 13; ++i) file >> token;
    long utime, stime;
    file >> utime >> stime;

    long clock_ticks = sysconf(_SC_CLK_TCK);
    return static_cast<double>(utime + stime) / clock_ticks;
}

size_t BlueThread::getThreadMemoryUsage() {
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("VmRSS:") == 0) {
            std::istringstream iss(line);
            std::string label;
            size_t mem;
            std::string kb;
            iss >> label >> mem >> kb;
            return mem;
        }
    }
    return 0;
}
#endif

#ifdef _WIN32
double BlueThread::getThreadCpuTime() {
    if (!thread.joinable()) return 0.0;

    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (GetThreadTimes((HANDLE)thread.native_handle(), &creationTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER u, k;
        u.LowPart = userTime.dwLowDateTime;
        u.HighPart = userTime.dwHighDateTime;
        k.LowPart = kernelTime.dwLowDateTime;
        k.HighPart = kernelTime.dwHighDateTime;
        return (u.QuadPart + k.QuadPart) / 10000000.0;
    }
    return 0.0;
}

size_t BlueThread::getThreadMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
}
#endif
