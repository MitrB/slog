#include "slog.hpp"
#include <thread>
#include <vector>
#include <chrono>

void basic_usage_example() {
    std::cout << "\n=== Basic Usage Example ===\n" << std::endl;

    // Simple logging with default configuration
    slog::Logger::get_instance().info("Application started");
    slog::Logger::get_instance().debug("Debug information");
    slog::Logger::get_instance().warn("Warning message");
    slog::Logger::get_instance().error("Error occurred");
    slog::Logger::get_instance().fatal("Fatal error!");

    // Using convenience macros
    SLOG_INFO("This is an info message using macro");
    SLOG_DEBUG("This is a debug message using macro");
    SLOG_WARN("This is a warning using macro");
    SLOG_ERROR("This is an error using macro");

    // Legacy compatibility
    slog("Legacy log message (maps to INFO level)");
}

void formatted_logging_example() {
    std::cout << "\n=== Formatted Logging Example ===\n" << std::endl;

    // Template-based formatted logging (Python-style {} placeholders)
    int user_id = 12345;
    std::string username = "john_doe";
    double balance = 1234.56;

    slog::Logger::get_instance().info("User {} logged in", username);
    slog::Logger::get_instance().debug("User ID: {}, Balance: ${}", user_id, balance);
    slog::Logger::get_instance().warn("User {} has low balance: ${}", username, balance);
}

void configuration_example() {
    std::cout << "\n=== Configuration Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();

    // Create custom configuration
    slog::Config config;
    config.log_file_path = "custom_app.log";
    config.max_file_size = 10 * 1024 * 1024; // 10 MB
    config.min_log_level = slog::LogLevel::INFO;
    config.enable_console_logging = true;
    config.enable_file_logging = true;
    config.include_timestamp = true;
    config.include_level = true;

    // Apply configuration
    logger.configure(config);
    logger.info("Logger configured with custom settings");

    // Modify individual settings
    logger.set_level(slog::LogLevel::WARN);
    logger.warn("Now only WARN and above will be logged");
    logger.debug("This debug message won't appear");

    // Reset to DEBUG level
    logger.set_level(slog::LogLevel::DEBUG);
    logger.debug("Debug messages are back");
}

void file_and_console_toggle_example() {
    std::cout << "\n=== File and Console Toggle Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();

    // Disable console logging (only log to file)
    logger.set_console_logging(false);
    logger.info("This goes to file only");

    // Re-enable console logging
    logger.set_console_logging(true);
    logger.info("This goes to both file and console");

    // Disable file logging (only log to console)
    logger.set_file_logging(false);
    logger.info("This goes to console only");

    // Re-enable file logging
    logger.set_file_logging(true);
    logger.info("Back to logging to both");
}

void log_levels_example() {
    std::cout << "\n=== Log Levels Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();

    // Set minimum level to INFO
    logger.set_level(slog::LogLevel::INFO);
    logger.info("Setting minimum log level to INFO");

    logger.debug("This DEBUG message will NOT appear");
    logger.info("This INFO message WILL appear");
    logger.warn("This WARN message WILL appear");
    logger.error("This ERROR message WILL appear");

    // Set minimum level to ERROR
    logger.set_level(slog::LogLevel::ERROR);
    logger.info("Setting minimum log level to ERROR");

    logger.info("This INFO message will NOT appear");
    logger.warn("This WARN message will NOT appear");
    logger.error("This ERROR message WILL appear");
    logger.fatal("This FATAL message WILL appear");

    // Reset to DEBUG
    logger.set_level(slog::LogLevel::DEBUG);
}

void thread_safety_example() {
    std::cout << "\n=== Thread Safety Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();
    logger.info("Starting multi-threaded logging test");

    // Create multiple threads that log simultaneously
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            auto& logger = slog::Logger::get_instance();
            for (int j = 0; j < 3; ++j) {
                logger.info("Thread {} - Message {}", i, j);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    logger.info("Multi-threaded logging test completed");
}

void custom_log_file_example() {
    std::cout << "\n=== Custom Log File Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();

    // Change log file location
    logger.set_log_file("./logs/application.log");
    logger.info("Logging to custom file location");

    // Change back to default
    logger.set_log_file("log.txt");
    logger.info("Back to default log file");
}

void real_world_example() {
    std::cout << "\n=== Real-World Application Example ===\n" << std::endl;

    auto& logger = slog::Logger::get_instance();

    // Application startup
    logger.info("Application starting...");
    logger.debug("Loading configuration from config.json");
    logger.info("Configuration loaded successfully");

    // Database connection
    logger.debug("Connecting to database...");
    logger.info("Database connection established");

    // Processing some data
    int records_processed = 0;
    int total_records = 100;

    for (int i = 0; i < 5; ++i) {
        records_processed += 20;
        logger.debug("Processed {}/{} records", records_processed, total_records);
    }

    logger.info("Data processing completed: {} records", records_processed);

    // Simulating an error
    bool api_call_success = false;
    if (!api_call_success) {
        logger.error("Failed to connect to external API");
        logger.warn("Retrying API connection...");
    }

    // Application shutdown
    logger.info("Closing database connection");
    logger.info("Application shutting down gracefully");
}

int main() {
    std::cout << "SLOG - Simple Header-Only C++ Logger - Examples\n";
    std::cout << "================================================\n";

    // Run all examples
    basic_usage_example();
    formatted_logging_example();
    configuration_example();
    file_and_console_toggle_example();
    log_levels_example();
    thread_safety_example();
    custom_log_file_example();
    real_world_example();

    std::cout << "\n================================================\n";
    std::cout << "All examples completed! Check 'log.txt' for output.\n";

    return 0;
}
