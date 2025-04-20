#include "log.h"

#include <iostream>
#include <tuple>
#include <vector>

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
/**********************************************Appender***************************************************************************/

    //输出到控制台
    void StdoutLogAppender::log(LoggerEvent::ptr event){
        //格式化时间
        std::string format = "%Y-%m-%d %H:%M:%S";
        tm tm;
        time_t t = event->getTime();
        localtime_r(&t,&tm);
        char tm_buf[64];
        strftime(tm_buf,sizeof(tm_buf),format.c_str(),&tm);

        std::cout
                //<<event->getTime()<<" "
                <<tm_buf<<" "//时间
                <<event->getThreadId()<<" "
                <<event->getFiberId()<<" "
                <<"["
                //<<event->getLevel()
                <<LoggerLevel::ToString(event->getLevel())//把从数字的等级转为字符串
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
        //用一个turple来实现存储 符号:子串:解析方式, 用一个vector来存储这些turple
        //解析方式的0表示普通字符串，1表示可以被解析的字符串
        std::vector<std::tuple<std::string,std::string,int>> vec;

        //解析后的字符串
        std::string nstr;

        //开始解析
        //"%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
        for(size_t i =0;i<m_pattern.size();i++){
            if(m_pattern[i]!='%'){
                //如果是‘[’ ']' ':'等等这些普通字符
                //把这个普通字符加入到nstr
                nstr.append(1,'%');
                continue;
            }
            //如果i是‘%’并且i+1也是‘%’
            if((i+1)<m_pattern.size()){
                if(m_pattern[i+1]=='%'){
                    nstr.append(1,m_pattern[i+1]);
                    continue;
                }
            }
            //如果i是‘%’并且i+1不是‘%’，需要进行解析
            std::string str;//存放符号
            std::string fmt;//存放'{}'中间的子串

            size_t n = i+1;//跳过i的‘%’,从下一个开始解析
            int fmt_status = 0;//如果是0，则表示还没有遇到'{'，如果是1,则表示遇到了'{'但是还没遇到'}'
            size_t fmt_begin = 0;//'{'开始的地方

            // 从m_pattern[i+1]开始遍历
            while(n<m_pattern.size()){
                // m_pattern[n]不是字母 & m_pattern[n]不是'{' & m_pattern[n]不是'}'，比如遇到了[ ] :
                if(!fmt_status&&(!isalpha(m_pattern[n]))&&m_pattern[n]!='{'&&m_pattern[n]!='}'){
                    str = m_pattern.substr(i+1,n-i-1);//i+1是'%'的下一个位置，但是这个n是会变的
                    break;
                }
                if(fmt_status==0){
                    if(m_pattern[n]=='{'){
                        //遇到了{，把前面的符号截取
                        //比如%d{%Y-%m-%d %H:%M:%S}
                        //这里的i->0是%,(i+1)->1是d,n->2是{,n-i-1是d 
                        str = m_pattern.substr(i+1,n-i-1);
                        fmt_status = 1;//遇到了'{'
                        fmt_begin = n;//标志进入'{'的位置
                        n++;
                        continue;
                    }
                }else if(fmt_status==1){
                    if(m_pattern[n]=='}'){
                        //把'{}'之间的子串截取
                        fmt =  m_pattern.substr(fmt_begin+1,n-i-1);
                        fmt_status = 0;
                        n++;
                        break;//找完一组大括号就退出循环 
                    }
                }
                n++;//这里是没有遇到'{' 也没有遇到'}'的其他情况，也就是遇到了符号或者{}里面的子串的情况
                //判断是否遍历结束
                if(n==m_pattern.size()){
                    //来到了这里，表示没有进入过前面if(m_pattern[n]=='{')更加没有进入过else if(fmt_status==1)
                    //比如说%T%t%T%F%
                    if(str.empty()){
                        str = m_pattern.substr(i+1);//把比如T这些符号存进去
                    }
                }
            }

            if(fmt_status==0){
                if(!nstr.empty()){
                    // 保存其他字符 '['  ']'  ':'
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                //fmt解析到的
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n-1;//调整i继续向后遍历
            }else if(fmt_status==1){
                // 没有找到与'{'相对应的'}' 所以解析报错，格式错误
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }
        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        //输出看下
        for(auto& it : vec){
            std::cout<<std::get<0>(it)
            <<" : "<<std::get<1>(it)
            <<" : "<<std::get<2>(it)
            <<std::endl;
        }
    }

    std::string LogFormatter::format(LoggerEvent::ptr event){
        //暂时输出个空
        return "";
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

}