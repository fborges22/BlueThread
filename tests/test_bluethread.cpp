#include <gtest/gtest.h>
#include "BlueThread.hpp"
#include <chrono>
#include <atomic>

TEST(BlueThreadTest, ExecutesFunction) {
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
