#include <mylog/log_manager.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>

// Конвертирует строковое представление уровня важности в enum Importance
Importance str_to_imp(const std::string& level_str, Importance default_level) {
    if (level_str.empty()) return default_level;
    if (level_str == "LOW") return Importance::LOW;
    if (level_str == "MEDIUM") return Importance::MEDIUM;
    if (level_str == "HIGH") return Importance::HIGH;
    return default_level;
}

// Структура для хранения сообщения и его уровня важности
struct LogMessage {
    std::string message;
    Importance level;
};

// Основной класс приложения
class App {
private:
    Importance default_lev_;         // Уровень важности по умолчанию
    LogManager logger;               // Менеджер логирования
    std::queue<LogMessage> log_queue_;  // Очередь сообщений
    std::mutex queue_mutex_;         // Мьютекс для синхронизации доступа к очереди
    std::condition_variable queue_cond_; // Условная переменная для ожидания сообщений
    bool running_;                   // Флаг работы потока
    std::thread worker_;             // Рабочий поток для записи логов

    // Функция рабочего потока
    void worker_function() {
        while(1) {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            // Ожидаем сообщений или завершения работы
            queue_cond_.wait(lock, [this] {
                return !log_queue_.empty() || !running_;
            });
            
            // Завершаем поток если приложение остановлено и очередь пуста
            if(!running_ && log_queue_.empty()) break;
            
            // Извлекаем сообщение из очереди
            auto msg = log_queue_.front();
            log_queue_.pop();
            lock.unlock();
            
            // Записываем сообщение через менеджер логирования
            logger.log(msg.message, msg.level);
        }
    }
        
public:
    // Конструктор инициализирует логгер и запускает рабочий поток
    App(const std::string& log_file, const std::string& default_level_str): 
        default_lev_(str_to_imp(default_level_str, Importance::LOW)),
        logger(log_file, default_lev_), running_(true) {
        worker_ = std::thread([this]() {
            worker_function();
        });
    }

    // Деструктор останавливает поток и дожидается его завершения
    ~App() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            running_ = false;
        }
        queue_cond_.notify_one();
        if(worker_.joinable()) {
            worker_.join();
        }
    }

    // Основной цикл работы приложения
    int run() {
        std::string message, level_str;
        while(1) {
            std::cout << "Enter message (or 'quit' to exit, Enter 'change' to set base importance): ";
            std::getline(std::cin, message);
            
            // Выход из приложения
            if(message == "quit") break;
            
            // Смена уровня важности по умолчанию
            if(message == "change") {
                std::string new_base_imp;
                std::cout << "Enter new base importance: ";
                std::getline(std::cin, new_base_imp);
                default_lev_ = str_to_imp(new_base_imp, default_lev_);
                logger.set_base_importance(default_lev_);
            }
            else {
                // Запрос уровня важности для сообщения
                std::cout << "Enter importance (LOW/MEDIUM/HIGH, Enter for default): ";
                std::getline(std::cin, level_str);
                Importance level = str_to_imp(level_str, default_lev_);
                
                // Добавление сообщения в очередь
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    log_queue_.push({message, level});
                }
                queue_cond_.notify_one(); // Уведомляем рабочий поток
            }
        }
        return 0;
    }
};

// Точка входа в приложение
int main(int argc, char* argv[]) {
    // Проверка аргументов командной строки
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <default_importance>" << std::endl;
        return 1;
    }

    try {
        // Создание и запуск приложения
        App app(argv[1], argv[2]);
        return app.run();
    } catch(const std::exception& e) {
        std::cerr << "Failed to initialize application: " << e.what() << std::endl;
        return 1;
    }
}