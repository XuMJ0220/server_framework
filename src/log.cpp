#include "log.h"

#include <iostream>

namespace xumj{


/*********************************************LoggerEvent************************************************************************/
    LoggerEvent::LoggerEvent(const char* file,
                        int32_t line,
                        int32_t elapse,
                        uint32_t threadId,
                        uint32_t fiberId,
                        uint64_t time,
                        LoggerLevel::level level
                        )
    :m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(threadId),
    m_fiberId(fiberId),
    m_time(time),
    m_level(level)
    {}
/*********************************************LoggerEvent************************************************************************/
/***********************************************Logger***************************************************************************/
    Logger::Logger(const std::string& name)
        :m_name(name),
        m_level(LoggerLevel::DEBUG)
    {
    }

    void Logger::log(LoggerEvent::ptr event){
        //要查看的事件的等级需要比日志器的等级高才能进行
        if(event->getLevel() >= m_level){
            //std::cout<<"日志模拟输出"<<std::endl;
            //把m_appenders里的都输出
            for(auto& i : m_appenders){
                i->log(event);
            }
        }else{
            std::cout<<"日志模拟失败"<<std::endl;
        }
    }

    void Logger::addAppender(LogAppender::ptr appender){
        m_appenders.emplace_back(std::move(appender));
    }

    void Logger::delAppender(LogAppender::ptr appender){
        for(auto it = m_appenders.begin();it!=m_appenders.end();it++){
            if(*it==appender){
                m_appenders.erase(it);
                break;
            }
        }
    }

/***********************************************Logger***************************************************************************/
/**********************************************Appender***************************************************************************/

    //输出到控制台
    void StdoutLogAppender::log(LoggerEvent::ptr event){
        std::cout<<event->getTime()<<" "
                <<event->getThreadId()<<" "
                <<event->getFiberId()<<" "
                <<"["
                <<event->getLevel()
                <<"]"
                <<event->getFile()<<":"<<event->getLine()<<" "
                <<"输出到控制台的信息"
                <<std::endl;
    }

    FileLogAppender::FileLogAppender(const std::string& filename)
        : m_filename(filename)
    {}
    
    //输出到文件
    void FileLogAppender::log(LoggerEvent::ptr event){
        std::cout << "输出到文件：" << m_filename << std::endl;
    }

/**********************************************Appender***************************************************************************/
}