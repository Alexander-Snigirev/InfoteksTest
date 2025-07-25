#pragma once
#include <string>
#include <memory>
#include <mylog/filesink.h>
#include <mylog/socketsink.h>

enum class Importance{
    LOW,
    MEDIUM,
    HIGH
};

class LogManager{
    private:
        std::string fname;
        Importance base_importance;
        std::unique_ptr<Sink> journal;
        std::mutex level_mutex_;
    public:
        LogManager(const std::string& fname, Importance base_imp);
        LogManager(const std::string& host, unsigned short port, Importance base_imp);
        void log(std::string message, Importance imp);
        void set_base_importance(Importance new_base_imp);
};