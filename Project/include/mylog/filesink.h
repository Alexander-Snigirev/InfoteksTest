#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <mutex>
#include <mylog/sink.h>

class FileSink : public Sink{
    private:
        std::string fname;
        std::ofstream file_;
        std::mutex mutex_;

    public:
        explicit FileSink(const std::string& fname);
        ~FileSink() override;
        void write(const std::string& message) override;
};