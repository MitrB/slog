#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>

namespace slog {

class Logger {
    public:
        static Logger& get_instance() {
            static Logger instance;
            return instance;
        }

        void log(std::string message) {
#ifdef LOG_MODE
            if (get_logfile_size() > MAX_FILESIZE) {
                truncate_logfile();
            }
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::tm* local_time = std::localtime(&now_time);

            char buffer[80];
            std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", local_time);
            std::string log_message = "[" + std::string(buffer) + "]: " + message;
            std::cout << "[LOG]" << log_message << std::endl;
            _logfile << log_message << std::endl;
#endif
        }

    private:
        std::ofstream _logfile;
        std::string _logfile_name;
        std::uintmax_t MAX_FILESIZE = 1024 * 1024 * 1024; // 1 gigabyte

        Logger() {
            _logfile_name = "log.txt";
            _logfile.open(_logfile_name, std::ios::out | std::ios::app);
        }

        ~Logger() {
            if (_logfile.is_open()) {
                _logfile.close();
            }
        }

        std::uintmax_t get_logfile_size() const {
            return std::filesystem::file_size(_logfile_name);
        }

        void truncate_logfile() {
            if (_logfile.is_open()) {
                _logfile.close();
            }
            _logfile.open(_logfile_name, std::ios::out | std::ios::trunc);
            if (!_logfile.is_open()) {
                std::cerr << "Error: Failed to truncate log file." << std::endl;
            }
        }

};

} // namespace slog

#define slog(message) slog::Logger::get_instance().log(message)
