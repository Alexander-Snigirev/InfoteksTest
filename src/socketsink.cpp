#include <mylog/socketsink.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>


SocketSink::SocketSink(const std::string& host, unsigned short port){
    socket_fd_ = -1;
    is_connected = false;
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);           // Создание TCP ipv4 сокета
    if(socket_fd_==-1){
        std::cerr <<"socketSink: Failed to create socket" <<std::endl;
        return;
    }

    sockaddr_in server_addr{};                              // Инициализация структуры сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr)<=0){
        std::cerr << "SocketSink: Invalid host address: "<<host<<std::endl;
        close(socket_fd_);
        socket_fd_ = -1;
        return;
    }

    // Попытка подключения к серверу
    if(connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1){
        std::cerr<<"SocketSink: failed to connect to " <<host<<":"<<port<<std::endl;
        close(socket_fd_);
        socket_fd_ = -1;
        return;
    }
    is_connected = true;
}

SocketSink::~SocketSink(){
    if(socket_fd_==-1){
        close(socket_fd_);              // Закрытие сокета
    }
}

void SocketSink::write(const std::string& message){
    std::lock_guard<std::mutex> lock(mutex_);           // Мьютекс для потокобезопасноти
    if(!is_connected || socket_fd_ == -1){
        std::cerr <<"SocketSink: Not connected, cannot write message" <<std::endl;
        return;
    }
    std::string msg_ln = message+'\n';
    ssize_t sent = send(socket_fd_, msg_ln.c_str(), msg_ln.size(), 0); // Передача сообщения
    if(sent==-1){
        std::cerr << "SocketSink: Failed to send msg"<<std::endl;
        is_connected = false;
    }
}
