#include <mylog/filesink.h>
#include <stdexcept>

FileSink::FileSink(const std::string& fname){
    this->file_ = std::ofstream(fname, std::ios::app);      // Иициализация файлового потока
    if(!file_.is_open()){
        throw std::runtime_error("Failed to open log file: " + fname);
    }
}

FileSink::~FileSink(){
    if(file_.is_open()){
        file_.close();                                      // Закрытие потока
    }
}

void FileSink::write(const std::string& message){
    std::lock_guard<std::mutex> lock(mutex_);               // Мьютекс для потокобезопасности
    if(file_.is_open()){
        file_ << message << std::endl;                      // Запись в файл
    }
}