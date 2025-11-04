#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace slog {

// Log levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4,
    OFF = 5
};

// Configuration structure
struct Config {
    bool enable_file_logging = true;
    bool enable_console_logging = true;
    std::string log_file_path = "log.txt";
    std::uintmax_t max_file_size = 1024 * 1024 * 1024; // 1 GB default
    LogLevel min_log_level = LogLevel::DEBUG;
    bool include_timestamp = true;
    bool include_level = true;
};

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    // Configure the logger
    void configure(const Config& config) {
        std::lock_guard<std::mutex> lock(_mutex);

        // Close existing file if open
        if (_logfile.is_open()) {
            _logfile.close();
        }

        _config = config;

        // Open new log file if file logging is enabled
        if (_config.enable_file_logging) {
            _logfile.open(_config.log_file_path, std::ios::out | std::ios::app);
            if (!_logfile.is_open()) {
                std::cerr << "[SLOG ERROR] Failed to open log file: " << _config.log_file_path << std::endl;
            }
        }
    }

    // Set minimum log level
    void set_level(LogLevel level) {
        std::lock_guard<std::mutex> lock(_mutex);
        _config.min_log_level = level;
    }

    // Enable/disable file logging
    void set_file_logging(bool enable) {
        std::lock_guard<std::mutex> lock(_mutex);
        _config.enable_file_logging = enable;

        if (enable && !_logfile.is_open()) {
            _logfile.open(_config.log_file_path, std::ios::out | std::ios::app);
        } else if (!enable && _logfile.is_open()) {
            _logfile.close();
        }
    }

    // Enable/disable console logging
    void set_console_logging(bool enable) {
        std::lock_guard<std::mutex> lock(_mutex);
        _config.enable_console_logging = enable;
    }

    // Set log file path
    void set_log_file(const std::string& path) {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_logfile.is_open()) {
            _logfile.close();
        }

        _config.log_file_path = path;

        if (_config.enable_file_logging) {
            _logfile.open(_config.log_file_path, std::ios::out | std::ios::app);
        }
    }

    // Main logging function
    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(_mutex);

        // Check if we should log this level
        if (level < _config.min_log_level) {
            return;
        }

        // Check file size and truncate if needed
        if (_config.enable_file_logging && _logfile.is_open()) {
            if (get_logfile_size() > _config.max_file_size) {
                truncate_logfile();
            }
        }

        // Build log message
        std::string log_message = build_log_message(level, message);

        // Output to console
        if (_config.enable_console_logging) {
            if (level >= LogLevel::ERROR) {
                std::cerr << log_message << std::endl;
            } else {
                std::cout << log_message << std::endl;
            }
        }

        // Output to file
        if (_config.enable_file_logging && _logfile.is_open()) {
            _logfile << log_message << std::endl;
            _logfile.flush(); // Ensure immediate write
        }
    }

    // Convenience methods for different log levels
    void debug(const std::string& message) { log(LogLevel::DEBUG, message); }
    void info(const std::string& message) { log(LogLevel::INFO, message); }
    void warn(const std::string& message) { log(LogLevel::WARN, message); }
    void error(const std::string& message) { log(LogLevel::ERROR, message); }
    void fatal(const std::string& message) { log(LogLevel::FATAL, message); }

    // Template methods for formatted logging
    template<typename... Args>
    void debug(const std::string& format, Args... args) {
        log(LogLevel::DEBUG, format_message(format, args...));
    }

    template<typename... Args>
    void info(const std::string& format, Args... args) {
        log(LogLevel::INFO, format_message(format, args...));
    }

    template<typename... Args>
    void warn(const std::string& format, Args... args) {
        log(LogLevel::WARN, format_message(format, args...));
    }

    template<typename... Args>
    void error(const std::string& format, Args... args) {
        log(LogLevel::ERROR, format_message(format, args...));
    }

    template<typename... Args>
    void fatal(const std::string& format, Args... args) {
        log(LogLevel::FATAL, format_message(format, args...));
    }

    // Get current configuration
    Config get_config() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _config;
    }

private:
    std::ofstream _logfile;
    Config _config;
    mutable std::mutex _mutex;

    Logger() {
        // Initialize with default configuration
        if (_config.enable_file_logging) {
            _logfile.open(_config.log_file_path, std::ios::out | std::ios::app);
        }
    }

    ~Logger() {
        if (_logfile.is_open()) {
            _logfile.close();
        }
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string level_to_string(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    std::string get_timestamp() const {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&now_time);

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        char buffer[80];
        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", local_time);

        std::ostringstream oss;
        oss << buffer << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    std::string build_log_message(LogLevel level, const std::string& message) const {
        std::ostringstream oss;

        if (_config.include_timestamp) {
            oss << "[" << get_timestamp() << "]";
        }

        if (_config.include_level) {
            oss << "[" << level_to_string(level) << "]";
        }

        oss << " " << message;
        return oss.str();
    }

    std::uintmax_t get_logfile_size() const {
        try {
            if (std::filesystem::exists(_config.log_file_path)) {
                return std::filesystem::file_size(_config.log_file_path);
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[SLOG ERROR] Failed to get file size: " << e.what() << std::endl;
        }
        return 0;
    }

    void truncate_logfile() {
        if (_logfile.is_open()) {
            _logfile.close();
        }
        _logfile.open(_config.log_file_path, std::ios::out | std::ios::trunc);
        if (!_logfile.is_open()) {
            std::cerr << "[SLOG ERROR] Failed to truncate log file." << std::endl;
        }
    }

    template<typename... Args>
    std::string format_message(const std::string& format, Args... args) {
        std::ostringstream oss;
        format_impl(oss, format, args...);
        return oss.str();
    }

    template<typename T, typename... Args>
    void format_impl(std::ostringstream& oss, const std::string& format, T value, Args... args) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << value;
            format_impl(oss, format.substr(pos + 2), args...);
        } else {
            oss << format;
        }
    }

    void format_impl(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }
};

} // namespace slog

// Convenience macros for easy logging
#define SLOG_DEBUG(msg) slog::Logger::get_instance().debug(msg)
#define SLOG_INFO(msg)  slog::Logger::get_instance().info(msg)
#define SLOG_WARN(msg)  slog::Logger::get_instance().warn(msg)
#define SLOG_ERROR(msg) slog::Logger::get_instance().error(msg)
#define SLOG_FATAL(msg) slog::Logger::get_instance().fatal(msg)

// Legacy compatibility
#define slog(message) slog::Logger::get_instance().info(message)
