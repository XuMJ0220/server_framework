#include "log.h"

int main(){

    //主要是测试这个等级，其他的参数随便都可以
    xumj::LoggerEvent::ptr event = std::make_shared<xumj::LoggerEvent>
    (nullptr,0,1,2,3,time(0),xumj::LoggerLevel::WARN);

    xumj::Logger::ptr log = std::make_shared<xumj::Logger>("xmj");

    log->log(event);

    //改为Error
    log->setLevel(xumj::LoggerLevel::ERROR);

    log->log(event);
    return 0;
}