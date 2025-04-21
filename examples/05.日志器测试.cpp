#include "log.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <iostream>
int main(int argc,char** argv){
    //创建一个日志事件(这里的内容随便定义，因为我们没有真正用到它)
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

    xumj::LogFormatter::ptr formatter(new xumj::LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    std::cout << formatter->format(event);
    return 0;
}
