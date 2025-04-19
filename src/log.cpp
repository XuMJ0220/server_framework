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
            std::cout<<"日志模拟输出"<<std::endl;
        }else{
            std::cout<<"日志模拟失败"<<std::endl;
        }
    }
/***********************************************Logger***************************************************************************/

}