#pragma once

#include <mylog/sink.h>
#include <string>
#include <mutex>

class SocketSink: public Sink{
    public:
        SocketSink(const std::string& host, unsigned short port);
        ~SocketSink();
        void write(const std::string& message) override;
    private:
        int socket_fd_;
        std::mutex mutex_;
        bool is_connected;    
};