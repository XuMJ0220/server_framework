#ifndef XUMJ_LOG_H_
#define XUMJ_LOG_H_

#include <string>
#include <memory>
#include <list>

namespace xumj{
    
    /*
    *@brief 日志级别
    */
    class LoggerLevel{
        public:
            enum level{
                UNKNOW = 0,
                DEBUG = 1,
                INFO = 2,
                WARN = 3,
                ERROR = 4,
                FATAL = 5
            };
    };


    /*
    *@brief 日志事件 我们要输出的格式大致为
    * 时间					线程id	 线程名称		   协程id	[日志级别]	 [日志名称]	  文件名:行号:           			  消息 	    换行符号
    *2024-01-07 10:06:00   2048    thread_name      1024    [INFO]      [logger]    /apps/sylar/tests/test_log.cc:40  消息内容 
    */
    class LoggerEvent{
        public:
            using ptr = std::shared_ptr<LoggerEvent>;

            LoggerEvent(const char* file,
                        int32_t line,
                        int32_t elapse,
                        uint32_t threadId,
                        uint32_t fiberId,
                        uint64_t time,
                        LoggerLevel::level level
                        );

            /*
            *@brief 获取文件名
            */
            const char* getFile() const { return m_file;}

            /*
            *@brief 获取行号
            */
            int32_t getLine() const {return m_line;}

            /*
            *@brief 获取程序启动开始到现在的毫秒数
            */
            int32_t getElapse() const {return m_elapse;}

            /*
            *@brief 获取线程id
            */
            uint32_t getThreadId() const {return m_threadId;}

            /*
            *@brief 获取协程id
            */
            uint32_t getFiberId() const {return m_fiberId;}

            /*
            *@brief 获取时间戳
            */
            uint64_t getTime() const {return m_time;}

            /*
            *@brief 获取日志等级
            */
            LoggerLevel::level getLevel() const { return m_level;}

        private:

            const char* m_file = nullptr;//文件名
    
            int32_t m_line = 0;//行号

            int32_t m_elapse = 0;//程序启动开始到现在的毫秒数

            uint32_t m_threadId = 0;//线程id

            uint32_t m_fiberId = 0;//协程id

            uint64_t m_time = 0;//时间戳

            LoggerLevel::level m_level;//日志级别
    };

    /*
    *@brief Appender基类 不需要构造函数
    */
    class LogAppender{
        
        public:

            using ptr = std::shared_ptr<LogAppender>;

            /*
            *@brief 虚析构 基类的不需要实现什么
            */
            virtual ~LogAppender(){}

            /*
            *@brief 一个输出函数，需要是存虚函数
            *@param[in] event 一个日志事件的智能指针
            */
            virtual void log(LoggerEvent::ptr event) = 0;
    
    };

    /*
    *@brief 输出到控制台适配器类
    */
    class StdoutLogAppender : public LogAppender{
        
        public:

            using ptr = std::shared_ptr<StdoutLogAppender>;

            /*
            *@brief override是一定要重写
            */
            void log(LoggerEvent::ptr event) override;
        private:
    };

    /*
    *@brief 输出到文件适配器类
    */
    class FileLogAppender : public LogAppender{
        
        public:

            using ptr = std::shared_ptr<FileLogAppender>;

            FileLogAppender(const std::string& filename);

            /*
            *@brief override是一定要重写
            */
            void log(LoggerEvent::ptr event) override;

        private:
            /*
            *@brief 文件名
            */
            std::string m_filename;
    };

    /*
    *@brief 日志器
    */
    class Logger{
        
        public:
            /*
            *@brief Logger类型的智能指针
            */
            using ptr = std::shared_ptr<Logger>;

            /*
            *@brief 构造函数
            *@param[in] name 引用是为了不必要的拷贝,const是name不得修改
            */
            Logger(const std::string& name = "root");

            /*
            *@brief 获取日志器的名字，返回是引用是直接把私有变量返回除去，避免拷贝，返回值为const是为了不让这个私有变量被修改
            */
            const std::string& getName(){ return m_name;}

            /*
            *@brief 获取日志器的最大等级
            */
            LoggerLevel::level getLevel(){ return m_level;}

            /*
            *@brief 设置日志器能输出的最大等级
            */
            void setLevel(LoggerLevel::level val){ m_level = val;}

            /*
            *@brief 一个日志的输出,传入要查看的事件，主要是为了获得这个事件的等级
            */
            void log(LoggerEvent::ptr event);

            /*
            *@brief 添加一个适配器
            */
            void addAppender(LogAppender::ptr appender);

            /*
            *@brief 删除一个适配器
            */
            void delAppender(LogAppender::ptr appender);
        private:
            /*
            *@brief 日志器名字，m表示的是私有变量
            */
            std::string m_name;

            /*
            *@brief 日志器能输出的最大等级
            */
            LoggerLevel::level m_level;

            /*
            *@list 存储适配器的智能指针
            */
            std::list<LogAppender::ptr> m_appenders;
    };
}

#endif // !XUMJ_LOG_H_

