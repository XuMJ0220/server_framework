#include "log.h"

namespace xumj{

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

    Logger::Logger(const std::string& name)
        :m_name(name)
    {
    }
}