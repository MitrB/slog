# slog

A lightweight, thread-safe, header-only C++ logging library with configurable log levels, formatting, and output options.

## Features

- **Header-only**: Just include `slog.hpp` and you're ready to go
- **Multiple log levels**: DEBUG, INFO, WARN, ERROR, FATAL
- **Thread-safe**: Safe to use from multiple threads simultaneously
- **Configurable**: Control file/console output, log levels, file paths, and more
- **Formatted logging**: Python-style `{}` placeholder formatting
- **Automatic file rotation**: Configurable maximum file size with automatic truncation
- **Zero dependencies**: Uses only C++ standard library
- **High precision timestamps**: Millisecond-level timestamp accuracy

## Quick Start

### Basic Usage

```cpp
#include "slog.hpp"

int main() {
    // Simple logging
    SLOG_INFO("Application started");
    SLOG_DEBUG("Debug information");
    SLOG_WARN("Warning message");
    SLOG_ERROR("Something went wrong");
    SLOG_FATAL("Critical error!");

    return 0;
}
```

### Formatted Logging

```cpp
#include "slog.hpp"

int main() {
    std::string username = "john_doe";
    int user_id = 12345;
    double balance = 1234.56;

    // Python-style formatting with {} placeholders
    slog::Logger::get_instance().info("User {} logged in", username);
    slog::Logger::get_instance().debug("User ID: {}, Balance: ${}", user_id, balance);

    return 0;
}
```

## Installation

1. Copy `slog.hpp` to your project
2. Include it in your source files:
   ```cpp
   #include "slog.hpp"
   ```
3. Compile with C++17 or later:
   ```bash
   g++ -std=c++17 your_program.cpp -o your_program
   ```

## API Reference

### Log Levels

```cpp
slog::LogLevel::DEBUG   // Detailed diagnostic information
slog::LogLevel::INFO    // General informational messages
slog::LogLevel::WARN    // Warning messages
slog::LogLevel::ERROR   // Error messages
slog::LogLevel::FATAL   // Critical/fatal errors
slog::LogLevel::OFF     // Disable all logging
```

### Logging Methods

#### Using Macros (Recommended)

```cpp
SLOG_DEBUG("Debug message");
SLOG_INFO("Info message");
SLOG_WARN("Warning message");
SLOG_ERROR("Error message");
SLOG_FATAL("Fatal message");

// Legacy compatibility
slog("Message");  // Maps to INFO level
```

#### Using Logger Instance

```cpp
auto& logger = slog::Logger::get_instance();

// Simple logging
logger.debug("Debug message");
logger.info("Info message");
logger.warn("Warning message");
logger.error("Error message");
logger.fatal("Fatal message");

// Formatted logging
logger.info("User {} has {} items", username, count);
logger.error("Failed to process record {}", record_id);
```

### Configuration

#### Using Config Structure

```cpp
slog::Config config;
config.log_file_path = "app.log";           // Log file path
config.max_file_size = 10 * 1024 * 1024;    // 10 MB
config.min_log_level = slog::LogLevel::INFO; // Minimum level to log
config.enable_console_logging = true;        // Log to console
config.enable_file_logging = true;           // Log to file
config.include_timestamp = true;             // Include timestamps
config.include_level = true;                 // Include log level

slog::Logger::get_instance().configure(config);
```

#### Individual Settings

```cpp
auto& logger = slog::Logger::get_instance();

// Set minimum log level
logger.set_level(slog::LogLevel::WARN);

// Enable/disable console output
logger.set_console_logging(true);

// Enable/disable file output
logger.set_file_logging(false);

// Change log file path
logger.set_log_file("logs/application.log");
```

### Output Format

Default log format:
```
[2025-11-04 14:32:45.123][INFO ] Application started
[2025-11-04 14:32:45.456][DEBUG] Processing user data
[2025-11-04 14:32:45.789][ERROR] Connection failed
```

## Examples

### Example 1: Basic Application Logging

```cpp
#include "slog.hpp"

int main() {
    auto& logger = slog::Logger::get_instance();

    logger.info("Application starting...");
    logger.debug("Loading configuration");
    logger.info("Configuration loaded successfully");

    // Simulate some work
    logger.debug("Processing data...");
    logger.info("Data processed successfully");

    logger.info("Application shutting down");
    return 0;
}
```

### Example 2: Custom Configuration

```cpp
#include "slog.hpp"

int main() {
    auto& logger = slog::Logger::get_instance();

    // Configure logger
    slog::Config config;
    config.log_file_path = "logs/myapp.log";
    config.max_file_size = 5 * 1024 * 1024;  // 5 MB
    config.min_log_level = slog::LogLevel::INFO;
    logger.configure(config);

    logger.info("Logger configured");
    logger.debug("This won't appear (level is INFO)");
    logger.info("This will appear");

    return 0;
}
```

### Example 3: Different Log Levels

```cpp
#include "slog.hpp"

int main() {
    auto& logger = slog::Logger::get_instance();

    // Start with DEBUG level
    logger.set_level(slog::LogLevel::DEBUG);
    logger.debug("Detailed debug info");
    logger.info("General information");

    // Switch to INFO level
    logger.set_level(slog::LogLevel::INFO);
    logger.debug("This won't show");
    logger.info("This will show");

    // Switch to ERROR level
    logger.set_level(slog::LogLevel::ERROR);
    logger.warn("This won't show");
    logger.error("This will show");

    return 0;
}
```

### Example 4: Thread-Safe Logging

```cpp
#include "slog.hpp"
#include <thread>
#include <vector>

void worker_function(int worker_id) {
    auto& logger = slog::Logger::get_instance();
    for (int i = 0; i < 5; ++i) {
        logger.info("Worker {} - Task {}", worker_id, i);
    }
}

int main() {
    auto& logger = slog::Logger::get_instance();
    logger.info("Starting multi-threaded application");

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker_function, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    logger.info("All workers completed");
    return 0;
}
```

### Example 5: Console-Only Logging

```cpp
#include "slog.hpp"

int main() {
    auto& logger = slog::Logger::get_instance();

    // Disable file logging for console-only output
    logger.set_file_logging(false);

    logger.info("This only goes to console");
    logger.error("Errors also only go to console");

    return 0;
}
```

## Building the Examples

Compile the included example:

```bash
g++ -std=c++17 example.cpp -o example -pthread
./example
```

Or with CMake:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myapp main.cpp)
target_link_libraries(myapp pthread)
```

## Requirements

- C++17 or later
- Standard library with `<filesystem>` support
- pthread library (for thread safety)

## Performance Considerations

- **Thread-safe**: Uses mutex locks for thread safety; consider using separate logger instances per thread for high-performance scenarios
- **File I/O**: Logs are flushed immediately to ensure data persistence; disable file logging for maximum performance
- **Formatting**: Template-based formatting has minimal overhead

## Migration from Old Version

The library maintains backward compatibility with the simple `slog(message)` macro, which now maps to INFO level:

```cpp
// Old code still works
slog("My log message");  // Now treated as INFO level

// Equivalent to
SLOG_INFO("My log message");
```

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.
