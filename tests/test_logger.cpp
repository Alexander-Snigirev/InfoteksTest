#include <mylog/log_manager.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>
#include <filesystem>
#include <algorithm>

// Вспомогательная функция для преобразования строки в Importance (из demo.cpp)
namespace mylog {
Importance string_to_importance(const std::string& level_str, Importance default_level) {
    if (level_str.empty()) return default_level;
    if (level_str == "LOW") return Importance::LOW;
    if (level_str == "MEDIUM") return Importance::MEDIUM;
    if (level_str == "HIGH") return Importance::HIGH;
    return default_level;
}
}

// Простой тестовый фреймворк
class TestRunner {
public:
    void run_test(const std::string& name, bool (*test_func)()) {
        std::cout << "Running test: " << name << "... ";
        if (test_func()) {
            std::cout << "PASS" << std::endl;
            passed_++;
        } else {
            std::cout << "FAIL" << std::endl;
            failed_++;
        }
        total_++;
    }

    void print_summary() const {
        std::cout << "\nTest Summary: " << passed_ << "/" << total_ << " passed, " << failed_ << " failed" << std::endl;
    }

private:
    size_t passed_ = 0;
    size_t failed_ = 0;
    size_t total_ = 0;
};

// Вспомогательная функция для чтения файла
std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}






// Тесты для FileSink
bool test_file_sink_write() {
    std::string filename = "test_log.txt";
    {
        FileSink sink(filename);
        sink.write("Test message");
    }
    std::string content = read_file(filename);
    std::filesystem::remove(filename);
    return content == "Test message\n";
}

bool test_file_sink_thread_safety() {
    std::string filename = "test_log_thread.txt";
    {
        FileSink sink(filename);
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&sink, i]() {
                sink.write("Message " + std::to_string(i));
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }
    std::string content = read_file(filename);
    std::filesystem::remove(filename);
    size_t line_count = std::count(content.begin(), content.end(), '\n');
    return line_count == 10; // Проверяем, что все 10 сообщений записаны
}

// Тесты для Logger
bool test_logger_initialization() {
    try {
        LogManager logger("test_init.txt", Importance::MEDIUM);
        return true;
    } catch (...) {
        return false;
    }
}


bool test_logger_filtering() {
    std::string filename = "test_filter.txt";
    {
        LogManager logger(filename, Importance::MEDIUM);
        logger.log("Low message", Importance::LOW);
        logger.log("Medium message", Importance::MEDIUM);
        logger.log("High message", Importance::HIGH);
    }
    std::string content = read_file(filename);
    std::filesystem::remove(filename);
    bool has_medium = content.find("Medium message") != std::string::npos;
    bool has_high = content.find("High message") != std::string::npos;
    bool no_low = content.find("Low message") == std::string::npos;
    return has_medium && has_high && no_low;
}

bool test_logger_set_base_importance() {
    std::string filename = "test_base.txt";
    {
        LogManager logger(filename, Importance::MEDIUM);
        logger.log("Medium message", Importance::MEDIUM);
        logger.set_base_importance(Importance::HIGH);
        logger.log("Medium message 2", Importance::MEDIUM);
        logger.log("High message", Importance::HIGH);
    }
    std::string content = read_file(filename);
    std::filesystem::remove(filename);
    bool has_first = content.find("Medium message") != std::string::npos;
    bool no_second = content.find("Medium message 2") == std::string::npos;
    bool has_high = content.find("High message") != std::string::npos;
    return has_first && no_second && has_high;
}

bool test_logger_format() {
    std::string filename = "test_format.txt";
    {
        LogManager logger(filename, Importance::MEDIUM);
        logger.log("Test message", Importance::HIGH);
    }
    std::string content = read_file(filename);
    std::filesystem::remove(filename);
    return content.find("[Priority: 2][") != std::string::npos &&
           content.find("]Test message") != std::string::npos;
}

int main() {
    TestRunner runner;

    runner.run_test("FileSink write", test_file_sink_write);
    runner.run_test("FileSink thread safety", test_file_sink_thread_safety);
    runner.run_test("Logger initialization", test_logger_initialization);
    runner.run_test("Logger filtering", test_logger_filtering);
    runner.run_test("Logger set_base_importance", test_logger_set_base_importance);
    runner.run_test("Logger format", test_logger_format);

    runner.print_summary();
    return 0;
}