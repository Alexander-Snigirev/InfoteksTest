#include <mylog/log_manager.h>
#include <cstdlib>
#include <chrono>
#include <iomanip>


LogManager::LogManager(const std::string& fname, Importance base_imp){
    this->fname = fname;
    this->base_importance = base_imp;
    this->journal = std::make_unique<FileSink>(fname);   // Умный указатель на объект журнала
}

void LogManager::set_base_importance(Importance new_base_imp){
    std::lock_guard<std::mutex> lock(level_mutex_);     // Мьютекс для потокобезопасности
    this->base_importance = new_base_imp;
}


void LogManager::log(std::string& message, Importance imp){
    if(imp < base_importance){
        return;
    }
    auto now_utc = std::chrono::system_clock::now();
    auto now_utc_sec = std::chrono::floor<std::chrono::seconds>(now_utc);
    std::time_t now_c = std::chrono::system_clock::to_time_t(now_utc_sec);
    std::ostringstream oss;
    // Формирование строки лога
    oss << std::put_time(std::gmtime(&now_c), "[%F, %T]");
    std::string realtime = oss.str();
    std::string log_msg = "[Priority: " + std::to_string(static_cast<int>(imp))+ "]" + realtime + message;
    this->journal.get()->write(log_msg);
}