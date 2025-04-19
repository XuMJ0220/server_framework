#include "log.h"

int main(int argc,char** argv){
    
    xumj::LoggerEvent::ptr event = std::make_shared<xumj::LoggerEvent>
    (
        "log.txt",
        1,
        1234567,
        2,
        3,
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