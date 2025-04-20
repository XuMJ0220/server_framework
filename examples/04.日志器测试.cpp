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
    
    xumj::LogFormatter::ptr formatter = std::make_shared<xumj::LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    formatter->format(event);
    return 0;
}