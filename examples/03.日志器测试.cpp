#include "log.h"

#include <unistd.h>//线程id
#include <sys/types.h>//线程id
#include <sys/syscall.h>//线程id
#include <thread>
int main(int argc,char** argv){
    
    xumj::LoggerEvent::ptr event = std::make_shared<xumj::LoggerEvent>
    (
        //"log.txt",
        __FILE__,
        //1,
        __LINE__,
        1234567,
        //2,
        syscall(SYS_gettid),
        3,
        //std::this_thread::get_id(),
        time(0),
        xumj::LoggerLevel::INFO
    );
    xumj::Logger::ptr log = std::make_shared<xumj::Logger>("xmj");
    
    xumj::StdoutLogAppender::ptr stdApd = std::make_shared<xumj::StdoutLogAppender>();

    xumj::FileLogAppender::ptr fileApd = std::make_shared<xumj::FileLogAppender>("log.txt");

    log->addAppender(stdApd);
    log->addAppender(fileApd);

    log->log(event);

    return 0;
}