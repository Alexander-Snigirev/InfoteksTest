#include <mylog/log_manager.h>
#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>

Importance str_to_imp(const std::string& level_str, Importance default_level){
    if (level_str.empty()) return default_level;
    if (level_str == "LOW") return Importance::LOW;
    if (level_str == "MEDIUM") return Importance::MEDIUM;
    if (level_str == "HIGH") return Importance::HIGH;
    return default_level;
}

struct LogMessage{
    std::string message;
    Importance level;
};

class App{
    private:
        Importance default_lev_;
        LogManager logger;
        std::queue<LogMessage> log_queue_;
        std::mutex queue_mutex_;
        std::condition_variable queue_cond_;
        bool running_;
        std::thread worker_;
        void worker_function(){
            while(1){
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cond_.wait(lock, [this] {
                    return !log_queue_.empty() || !running_;
                });
                if(!running_ && log_queue_.empty()) break;
                auto msg = log_queue_.front();
                log_queue_.pop();
                lock.unlock();
                logger.log(msg.message, msg.level);
            }
        }
        
    public:
        App(const std::string& log_file, const std::string& default_level_str): 
        default_lev_(str_to_imp(default_level_str, Importance::LOW)),
        logger(log_file, default_lev_), running_(true){
            worker_ = std::thread([this]() {
                worker_function();
            });
        }

        ~App() {
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                running_ = false;
            }
            queue_cond_.notify_one();
            if(worker_.joinable()){
                worker_.join();
            }
        }

        int run(){
            std::string message, level_str;
            while(1){
                std::cout<<"Enter message (or 'quit' to exit, Enter 'change' to set base importance): ";
                std::getline(std::cin, message);
                if(message == "quit") break;
                if(message == "change"){
                    std::string new_base_imp;
                    std::cout<<"Enter new base imortance: ";
                    std::getline(std::cin,new_base_imp);
                    default_lev_ = str_to_imp(new_base_imp, default_lev_);
                    logger.set_base_importance(default_lev_);
                }
                else{
                    std::cout<<"Enter importance (LOW/MEDIUM/HIGH, Enter for default): ";
                    std::getline(std::cin, level_str);
                    Importance level = str_to_imp(level_str, default_lev_);
                    {
                        std::lock_guard<std::mutex> lock(queue_mutex_);
                        log_queue_.push({message, level});
                    }
                    queue_cond_.notify_one();
                }
            }
            return 0;
        }


};




int main(int argc, char* argv[]) {
    if(argc!=3){
        std::cerr << "Usage: " << argv[0] << " <log_file> <default_importance>"<<std::endl;
        return 1;
    }

    try{
        App app(argv[1], argv[2]);
        return app.run();
    }catch(const std::exception& e){
        std::cerr<<"Failed to initialize application: "<<e.what()<<std::endl;
        return 1;
    }

}
