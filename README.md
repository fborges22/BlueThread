# BlueThread

**BlueThread** is a C++ class that enables asynchronous execution of arbitrary functions in a separate thread. It supports thread interruption, deferred execution, and future-based result retrieval. This project is built as a static library and includes unit tests using the [Google Test](https://github.com/google/googletest) framework.

---

## ✨ Features

- 🧵 Run any function or callable object in a separate thread
- 🛑 Interrupt thread execution safely
- 🔁 `set()` and `run()` interface for flexible scheduling
- 🔮 Supports `std::future` return values from threaded functions
- 🧪 Integrated Google Test support
- 🧰 CMake-based build system

---

## 📦 Build Instructions

### ✅ Requirements

- C++17 compatible compiler
- CMake ≥ 3.14
- Git (for cloning submodules or fetching dependencies)

### 🛠 Build and Test

```bash
# Clone the repository
git clone https://github.com/yourusername/BlueThread.git
cd BlueThread

# Create build directory
mkdir build && cd build

# Generate build system
cmake ..

# Build the project
make

# Run tests
ctest
