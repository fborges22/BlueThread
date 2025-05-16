#include <gtest/gtest.h>
#include "BlueThread.hpp"
#include <chrono>
#include <atomic>
#include <cmath>

TEST(BlueThreadTest, ExecutesFunction)
{
    BlueThread bt;

    std::atomic<bool> done = false;
    bt.set([&done]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        done = true;
    });

    bt.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_TRUE(done);
    bt.stop();
}

TEST(BlueThreadTest, CanInterruptFunction) {
    BlueThread bt;

    bt.set([&bt]() {
        for (int i = 0; i < 10; ++i) {
            if (bt.shouldStop()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    bt.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    bt.stop();
    EXPECT_FALSE(bt.isRunning());
}

TEST(BlueThreadTest, ReportsCpuTime) {
    BlueThread bt;

    bt.set([&bt]() {
        volatile double x = 0;
        for (int i = 0; i < 1e7; ++i) {
            if (bt.shouldStop()) break;
            x += std::sqrt(i);
        }
    });

    bt.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double cpuTime = bt.getThreadCpuTime();
    bt.stop();

    EXPECT_GT(cpuTime, 0.0) << "CPU time should be greater than zero";
}

TEST(BlueThreadTest, ReportsMemoryUsage) {
    BlueThread bt;

    bt.set([&bt]() {
        std::vector<int> largeVec(10'000'000, 42);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        (void)largeVec[0];  // use to prevent optimization
    });

    bt.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    size_t memUsage = bt.getThreadMemoryUsage();
    bt.stop();

    EXPECT_GT(memUsage, 0u) << "Memory usage should be greater than zero";
}
