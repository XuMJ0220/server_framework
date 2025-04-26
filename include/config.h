#ifndef XUMJ_CONFIG_H_
#define XUMJ_CONFIG_H_

#include <string>
#include <memory>
#include <algorithm>
#include <sstream>
#include <typeinfo>

namespace xumj{

    class ConfigBase{
        public:

            using ptr = std::shared_ptr<ConfigBase>;

            ConfigBase() = default;
            explicit ConfigBase(const std::string& name,const std::string& description = "")
                :m_name(name),m_description(description)
            {
                //将名字变为参数不敏感
                std::transform(name.begin(),name.end(),name.begin(),::tolower);
            }
            virtual ~ConfigBase(){}

            /*
            *@brief 获取字段名
            */
            const std::string& getName(){ return m_name;}

            /*
            *@brief 获取字段描述
            */
            const std::string& getDescription(){ return m_description;}

            /*
            *@brief 设置字段描述
            */
            void setDescription(const std::string& description){ m_description = description;}

            /*
            *@brief 将String类型转为需要的类型
            */
            virtual bool fromString(const std::string& val) = 0;

            /*
            *@brief 将其他的类型转为String类型
            */
            virtual std::string toString() = 0; 

            /*
            *@brief 获取类型名
            */
            virtual std::string getTypeName() const = 0;
        private:
            /*
            *@brief 字段名
            */
            std::string m_name;
            /*
            *@brief 字段描述
            */
            std::string m_description;
    };

    template<typename T>
    class ConfigVar : public ConfigBase{

        public:

            using ptr = std::shared_ptr<ConfigVar>;

            ConfigVar(const std::string& name,const T& val,const std::string& description = "")
                :ConfigBase(name,description),m_val(val){}

            /*
            *@brief 获取字段值
            */
            T getValue() const { return m_val;}

            /*
            *@brief 设置字段值
            */
            void setValue(const T& v){ m_val = v;}

            bool fromString(const std::string& val) override;

            std::string toString() override;

            std::string getTypeName() const override { return typeid(m_val).name();}
            
        private:    

            T m_val;//字段值

    };

    template<typename T>
    bool ConfigVar<T>::fromString(const std::string& val){
        try
        {
            /* code */
            std::istringstream iss(val);
            iss >> m_val;
            return !iss.fail();
        }
        catch(const std::exception& e)
        {
            std::cerr << "ConfigVar::fromString exception: "
                <<e.what()<<" convert: string to " << typeid(m_val).name()<<std::endl;
        }
        return false;
    }

    template<typename T>
    std::string ConfigVar<T>::toString(){
        try
        {
            /* code */
            std::ostringstream oss;
            oss<<m_val;
            return oss.str();
        }
        catch(const std::exception& e)
        {
            std::cerr << "ConfigVar::toString exception: "
                <<e.what()<<" convert: " << typeid(T).name()<<" to string"<<std::endl;
            return "";
        }
        
    }
};

#endif   