#include "log.h"

#include <iostream>
#include <tuple>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

namespace xumj{

/*********************************************LoggerLevel************************************************************************/
    const char* LoggerLevel::ToString(LoggerLevel::level level){

        switch (level)
        {
            #define XX(name)\
                case LoggerLevel::name:\
                return #name;\
                break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);

            #undef XX //从这里之后XX这个define就无效了
            default:
                return "UNKNOW";
                break;
        }

    }
/*********************************************LoggerLevel************************************************************************/

/*********************************************LoggerEvent************************************************************************/
    LoggerEvent::LoggerEvent(const char* file,
                        int32_t line,
                        int32_t elapse,
                        uint32_t threadId,
                        uint32_t fiberId,
                        uint64_t time,
                        LoggerLevel::level level,
                        const std::string& logName
                        )
    :m_file(file),
    m_line(line),
    m_elapse(elapse),
    m_threadId(threadId),
    m_fiberId(fiberId),
    m_time(time),
    m_level(level),
    m_logName(logName)
    {}
/*********************************************LoggerEvent************************************************************************/
/**********************************************Appender***************************************************************************/

    //输出到控制台
    void StdoutLogAppender::log(LoggerEvent::ptr event){
        // //格式化时间
        // std::string format = "%Y-%m-%d %H:%M:%S";
        // tm tm;
        // time_t t = event->getTime();
        // localtime_r(&t,&tm);
        // char tm_buf[64];
        // strftime(tm_buf,sizeof(tm_buf),format.c_str(),&tm);

        // std::cout
        //         //<<event->getTime()<<" "
        //         <<tm_buf<<" "//时间
        //         <<event->getThreadId()<<" "
        //         <<event->getFiberId()<<" "
        //         <<"["
        //         //<<event->getLevel()
        //         <<LoggerLevel::ToString(event->getLevel())//把从数字的等级转为字符串
        //         <<"]"
        //         <<event->getFile()<<":"<<event->getLine()<<" "
        //         <<"输出到控制台的信息"
        //         <<std::endl;
        std::cout<<m_format->format(event);
    }

    FileLogAppender::FileLogAppender(const std::string& filename)
        : m_filename(filename)
    {}
    
    //输出到文件
    void FileLogAppender::log(LoggerEvent::ptr event){
        std::cout << "输出到文件：" << m_filename << std::endl;
    }

/**********************************************Appender***************************************************************************/
/********************************************LogFormatter***************************************************************************/

    LogFormatter::LogFormatter(const std::string& pattern)
        :m_pattern(pattern)
    {
        //在初始化时就将pattern解析好
        init();
    }

    //我们需要将模板字符串解析成 符号:子串:解析方式 这种结构
    //例如这个模板 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
    //解析成
    //符号  子串                    解析方式    注释
    //"d"  "%Y-%m-%d %H:%M:%S"    1          #当前时间
    //"T"  ""                     1          #制表(4空格)
    //"t"  ""                     1          #线程ID
    //"T"  ""                     1          #制表(4空格)
    //"F"  ""                     1          #协程ID
    //"T"  ""                     1          #制表(4空格)
    //"["  ""                     0          #普通字符
    //"p"  ""                     1          #日志级别
    //"]"  ""                     0          #普通字符
    //"T"  ""                     1          #制表(4空格)
    //"["  ""                     0          #普通字符
    //"c"  ""                     1          #日志器名称
    //"]"  ""                     0          #普通字符
    //"T"  ""                     1          #制表(4空格)
    //"f"  ""                     1          #文件名称
    //":"  ""                     0          #普通字符
    //"l"  ""                     1          #行号
    //"T"  ""                     1          #制表(4空格)
    //"m"  ""                     1          #消息
    //"n"  ""                     1          #换行
    void LogFormatter::init(){
        //我们粗略的把上面的解析对象分成两类 一类是普通字符串 另一类是可被解析的
        //可以用 tuple来定义 需要的格式 std::tuple<std::string,std::string,int> 
        //<符号,子串,类型>  类型0-普通字符串 类型1-可被解析的字符串 
        //可以用一个 vector来存储 std::vector<std::tuple<std::string,std::string,int> > vec;
        std::vector<std::tuple<std::string,std::string,int> > vec;
        //解析后的字符串
        std::string nstr;
        //循环中解析
        for(size_t i = 0; i < m_pattern.size(); ++i) {
            // 如果不是%号
            // nstr字符串后添加1个字符m_pattern[i]
            if(m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            // m_pattern[i]是% && m_pattern[i + 1] == '%' ==> 两个%,第二个%当作普通字符
            if((i + 1) < m_pattern.size()) {
                if(m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }
            
            // m_pattern[i]是% && m_pattern[i + 1] != '%', 需要进行解析
            size_t n = i + 1;		// 跳过'%',从'%'的下一个字符开始解析
            int fmt_status = 0;		// 是否解析大括号内的内容: 已经遇到'{',但是还没有遇到'}' 值为1
            size_t fmt_begin = 0;	// 大括号开始的位置

            std::string str;
            std::string fmt;	// 存放'{}'中间截取的字符
            // 从m_pattern[i+1]开始遍历
            while(n < m_pattern.size()) {
                // m_pattern[n]不是字母 & m_pattern[n]不是'{' & m_pattern[n]不是'}'
                if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                        && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if(fmt_status == 0) {
                    if(m_pattern[n] == '{') {
                        // 遇到'{',将前面的字符截取
                        str = m_pattern.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; // 标志进入'{'
                        fmt_begin = n;	// 标志进入'{'的位置
                        ++n;
                        continue;
                    }
                } else if(fmt_status == 1) {
                    if(m_pattern[n] == '}') {
                        // 遇到'}',将和'{'之间的字符截存入fmt
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        // 找完一组大括号就退出循环
                        break;
                    }
                }
                ++n;
                // 判断是否遍历结束
                if(n == m_pattern.size()) {
                    if(str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if(fmt_status == 0) {
                if(!nstr.empty()) {
                    // 保存其他字符 '['  ']'  ':'
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                // fmt:寻找到的格式
                vec.push_back(std::make_tuple(str, fmt, 1));
                // 调整i的位置继续向后遍历
                i = n - 1;
            } else if(fmt_status == 1) {
                // 没有找到与'{'相对应的'}' 所以解析报错，格式错误
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
    
        //输出看下
        // for(auto& it : vec){
        //     std::cout<<std::get<0>(it)
        //     <<" : "<<std::get<1>(it)
        //     <<" : "<<std::get<2>(it)
        //     <<std::endl;
        // }

        //下面的操作堪称经典
        std::map<std::string,std::function<FormatItem::ptr(const std::string& str)>>
        s_format_items = 
        {
            #define XX(STR,FORMAT)\
                {#STR,[](const std::string& str)->FormatItem::ptr{ return FormatItem::ptr(new FORMAT(str));}}
            
            XX(d,DateTimeFormatItem),
            XX(T,TabFormatItem),
            XX(t,ThreadIdFormatItem),
            XX(F,FiberIdFormatItem),
            XX(p,LevelFormatItem),
            XX(c,NameFormatItem),
            XX(f,FilenameFormatItem),
            XX(m,MessageFormatItem),
            XX(n,NewLineFormatItem),
            XX(r,ElapseFormatItem),
            XX(l,LineFormatItem)

            #undef XX
        };

        for(auto& i : vec){

            if(std::get<2>(i) == 0){
                //如果是普通字符串
                m_items.push_back(std::make_shared<StringFormatItem>(std::get<0>(i)));
            }else{
                auto it = s_format_items.find(std::get<0>(i));//通过符号查找
                if(it!=s_format_items.end()){
                    m_items.push_back(it->second(std::get<1>(i)));
                }else{
                    m_items.push_back(std::make_shared<StringFormatItem>("<<error_format"+std::get<0>(i)+">>"));
                }
            }
        }
    }

    std::string LogFormatter::format(LoggerEvent::ptr event){
        //暂时输出个空
        //return "";
        //可以开始编写了
        std::stringstream ss;
        for(auto& i : m_items){
            i->format(ss,event);
        }
        return ss.str();
    }

    void MessageFormatItem::format(std::ostream& os, LoggerEvent::ptr& event){
        //os<<"Message";
        os<<event->getContent();
    }

    void LevelFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<LoggerLevel::ToString(event->getLevel());
    }

    void ElapseFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<event->getElapse();
    }

    void NameFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        //os<<"Name";
        os<<event->getLogName();
    }

    void ThreadIdFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<event->getThreadId();
    }

    void FiberIdFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<event->getFiberId();
    }

    void DateTimeFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        tm tm;
        time_t time = event->getTime();
        localtime_r(&time,&tm);
        char buf[64];
        strftime(buf,sizeof(buf),m_format.c_str(),&tm);
        os<<buf;
    }

    void FilenameFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<event->getFile();
    }

    void LineFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<event->getLine();
    }

    void NewLineFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<std::endl;
    }

    void StringFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<m_string;
    }   

    void TabFormatItem::format( std::ostream& os, LoggerEvent::ptr& event){
        os<<"\t";
    }
    
/********************************************LogFormatter***************************************************************************/
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
/********************************************LoggerManager***************************************************************************/
    void LoggerManager::init(){}

    Logger::ptr LoggerManager::getLogger(const std::string& name){
        auto it = m_loggers.find(name);
        if(it != m_loggers.end()){
            m_root = it->second;
            return it->second;
        }
        
        Logger::ptr logger(new Logger(name));
        m_loggers[name] = logger;
        return logger;
    }
/********************************************LoggerManager***************************************************************************/

}