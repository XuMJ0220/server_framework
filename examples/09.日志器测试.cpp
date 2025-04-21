#include "log.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>

int main(int argc,char** argv){

    xumj::Logger::ptr log = std::make_shared<xumj::Logger>("xumj");

    xumj::LoggerEvent::ptr event = std::make_shared<xumj::LoggerEvent>
    (
        __FILE__,
        __LINE__,
        123456,
        syscall(SYS_gettid),
        1,
        time(0),
        xumj::LoggerLevel::INFO,
        log->getName()
    );

    xumj::LogFormatter::ptr format = std::make_shared<xumj::LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    //xumj::LogFormatter::ptr format = std::make_shared<xumj::LogFormatter>("%d{%Y-%m-%d %H:%M:%S}");
    xumj::StdoutLogAppender::ptr stdAppender = std::make_shared<xumj::StdoutLogAppender>();
    stdAppender->setFormatter(format);

    log->addAppender(stdAppender);

    //消息
    event->getSS()<<"hello xumj";

    //log->log(event);
    xumj::LogEventWrap(log,event).getSS()<<" hahaha";//追加的内容
    log->log(event);
    return 0;
}