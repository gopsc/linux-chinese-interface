#pragma once

#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include "../../type.h"
#include "../system-call/FilesystemOp.h"
//循环依赖

#define CSC_COMMENTS  2
#define CSC_PATHNAME  3
#define CSC_PATHEND   4
#define CSC_LONGVALUE 5
#define CSC_VALUE     6
#define CSC_COMMAND   7

namespace qing{

    class DScript: public Unit{
    public:
    
        //构造函数，创建空的csc
        DScript() {}
        //构造函数，从文本或者文件中读取
        DScript(std::string text){
            this->toDict(text);
        }//Constructor
        
        //暂时关闭该函数的复制构造函数
        DScript(DScript&) = delete;

        
        //-----------------------------------------------------------
        //将字典写成配置文件
        std::string toStr(){
            lk.lock();
            
            //对表进行收集
            //要保存的结果字符串
            std::string res = "";
            // 使用基于范围的for循环遍历
            for(const auto &kv : kvTable){
                //根据有无换行符这一特性，对键值对进行分类
                //有换行符的称为长值对，没有换行符的称为键值对
                
                if (kv.second.find("\n") == -1) {
                    res += kv.first + "：" + kv.second + "\n";
                } else {
                    res += kv.first + "：（\n" + kv.second + "\n）\n"; 
                }
                //这里将以前所有包含路径名的部分全部统一成了键值对
                //准备返回转换的结果
            }
            
            lk.unlock();
            return res;
        }


        //打开配置文件的地址
        std::string Open(std::string k){
            lk.lock();
            std::string v = kvTable[k];
            lk.unlock();
            return v;
        }
        
        //获取这个变量的地址
        /*
        Unit* Var(std::string k){
            锁.lock();
            Unit* res = 变量池[k];
            锁.unlock();
            return res;
        }
        */

 /*
        //寻找地址
        std::string Find(std::string k){
        }
*/

        //向配置文件中写入字段
        void Add(std::string k, std::string v){
            lk.lock();
            kvTable[k] = v;
            lk.unlock();
        }


        //从配置文件中删除字段
        void Del(std::string k)
        {
            lk.lock();
            kvTable.erase(k);
            lk.unlock();
        }

    private:

        //锁类型
        std::mutex lk;
        //存放读出的字典。
        std::unordered_map<std::string, std::string>    kvTable;
        std::unordered_map<std::string, Unit*>          variPool;
        //全局路径
        std::string curPath = "";


        //私有成员方法域
        //从配置文件中读取字典
        void toDict(std::string msg) {
            
            //初始化全局路径
            curPath = "";

            //长值对读取状态
            bool IsLKV = false;
            //长值对中存储键值对
            std::string k = "", v = "";

            //换行符号的位置信息
            int pos1 = -1, pos2;
            //记录行数
            int line;

            do {
                //查找下一个换行符
                pos2 = msg.find('\n', pos1 + 1);
                //截取此行
	            std::string sentence = (pos2 == -1 
                    ? msg.substr(pos1 + 1, msg.size() - pos1 - 1)
                    : msg.substr(pos1 + 1, pos2 - pos1 - 1));

                //std::cout << s1 << std::endl;
                //位置变量前进一行
                pos1 = pos2;
                //计数变量自增
                line++;


                //长值对模式
                if (IsLKV == true){
                    //检查长值对结束符
                    if (IsLongkvEnd(sentence)){
                        //是长值对结束
                        
                        //去除末尾的空格
                        v = v.substr(0, v.size()-1);
                        //将收集到的长值对添加到字典中
                        JoinLongkv(this->curPath, this->kvTable, k, v);
                        
                        //清理
                        k = ""; v = "";
                        IsLKV = false;
                        continue;

                    } else {
                        //是长值对内容
                        v += sentence + "\n";
                        continue;
                    }//长值对内容
                }

                //单行模式
                //跳过空行
                if (sentence == "") continue;
                //处理此行
                switch (AnalyseSentence(sentence, line))
                {
                        //如果是注释，就跳过
                    case CSC_COMMENTS:  break;
                     //如果是路径名定义，就提取路径名并添加到全局路径
                    case CSC_PATHNAME:  PathIn(sentence, this->curPath); break;
                    //如果是路径名结束符，就将全局路径退出一层
                    case CSC_PATHEND:   PathOut(this->curPath, line); break;
                    //如果是长值对声明，就进行读取
                    case CSC_LONGVALUE:
                    {
                        IsLKV = true;
                        k = sentence.substr(0, sentence.size() - 6);
                        v= "";
                        break;
                    }
                    //如果是键值对，就添加到字典中
                    case CSC_VALUE: JoinKv(this->curPath, this->kvTable, sentence); break;
                    //如果是运行指示，就准备执行任务
                    case CSC_COMMAND:{
                        if (Execute(sentence) != 0) throw std::runtime_error("在读取CSC时出现了无法执行的指令，请检查代码。行" + std::to_string(line));
                        break;
                    }
                    //如果不属于以上任意一种，那就是错误的行。
                    default: throw std::runtime_error("在读取CSC时出现了未知的错误，请检查代码。行" + std::to_string(line));

                }

            } while (pos1 != -1);

            //文件结束仍处于长文本状态，意味着长文本没有结束指示
            if (IsLKV == true) throw std::runtime_error("在读取CSC时出现了长值对定义但没有结束，请检查代码。");
        }
    
        //-----------------------------------------------------------
        //-----------------------------------------------------------
        int AnalyseSentence(std::string &msg, int l) {
            /* 分析这一行代码的意义 */
            if (true == IsComment(msg, l))      return CSC_COMMENTS;
            if (true == IsPath(msg, l))         return CSC_PATHNAME;
            if (true == IsPathOut(msg, l))      return CSC_PATHEND;
            if (true == IsLongkv(msg, l))       return CSC_LONGVALUE;
            if (true == IsKv(msg, l))           return CSC_VALUE;
            if (true == IsConstruction(msg, l)) return CSC_COMMAND;
            //
            throw std::runtime_error("在读取CSC时出现了无效的行。行" + std::to_string(l));
        }
        //-----------------------------------------------------------
        //-----------------------------------------------------------
        static bool IsComment(std::string &msg, int l){
            /*检查是否存在格式正确的注释部分
            如果存在格式正确的注释，则返回真
            如果不存在注释有关的内容，则返回假
            如果发现了注释有关的内容，但不合规范，就返回错误*/

            size_t pos1 = msg.find("“", 0);
            size_t pos2 = msg.find("”", 0);
            //
            if      (pos1 == 0  && pos2 == msg.size() - 3)   return true;
            else if (pos1 == -1 && pos2 == -1)              return false;
            else throw std::runtime_error("在读取CSC时出现了错误的注释格式，请检查代码。行" + std::to_string(l));
        }

        
        static bool IsPath(std::string &msg, int l){
            /*检查是否存在格式正确的路径名定义
            如果存在格式正确的路径名定义，则返回真
            如果不存在路径定义有关的内容，则返回假
            如果发现了路径定义有关的内容，但不合规范，就返回错误*/

            size_t pos1 = msg.find("【", 0);
            size_t pos2 = msg.find("】", 0);
            //
            if      (pos1 == 0  && pos2 == msg.size() - 3)  return true;
            else if (pos1 == -1 && pos2 == -1)              return false;
            else throw std::runtime_error("在读取CSC时出现了错误的路径定义，请检查代码。行" + std::to_string(l));
        }

        static bool IsPathOut(std::string &msg, size_t l){
            /*检查该行是否为一个路径名的结束指示
            如果该行整个是一句路径结束符，就返回真
            如果该行中没有出现路径结束符号，就返回假*/
            if (msg == "，") return true;
            else return false;
        }

        static bool IsLongkv(std::string &msg, size_t l){
            /*检查该行是否是一个长值对的声明
            如果在末尾存在长值对的声明指示，则返回真
            如果不存在长值对的声明指示，则返回假
            如果存在长值对的声明指示，但是不在末尾，则抛出异常*/

            size_t pos1 = msg.find("：（", 0);
            //
            if (pos1 == msg.size() - 6) return true;
            else if (pos1 == -1) return false;
            else throw std::runtime_error("在读取CSC时出现了错误的长值对声明，请检查代码。行" + std::to_string(l));
        }

        static bool IsKv(std::string &msg, size_t l){
            /*检查该行是否是一个键值对的声明
            如果存在格式正确的键值对声明，则返回真
            如果存在位置错误的键值对声明符号，就抛出异常
            如果不存在键值对声明有关的内容，则返回假*/
            size_t pos = msg.find("：", 0);
            //
            if (pos > 0/* 且 位置 < 内容.size() - 3*/) return true;
            else if (pos == 0 /*或 位置 == 内容.size() - 3*/) throw std::runtime_error("在读取CSC时出现了错误的键值对声明，请检查代码。行" + std::to_string(l));
            else return false;
        }

        static bool IsLongkvEnd(std::string &msg){
            /*检查是否为长值对的结束
            如果该行整个是一句长值对结束符，就返回真。*/
            if (msg == "）") return true;
            else return false;
        }

        bool IsConstruction(std::string &msg, size_t l){
            /*检查是否是运行指示,这个方法不再是静态了
            如果该行是一句运行指示，就返回真*/
            
            size_t pos = msg.find("。");
            //
            if (pos == msg.size() - 3);
            else if (pos == -1) return false;
            else throw std::runtime_error("在读取CSC时出现了错误的运行指示，请检查代码。行" + std::to_string(l));
            
            /*执行指示，匹配格式化字符串，如过能够匹配，就开始执行*/
            if (msg.find("建立") == 0) {
                std::string keyword = msg.substr(6, msg.size() - 9);
                //std::cout << keyword << std::endl;
                    
                size_t pos1 = keyword.find("。");
                size_t pos2 = keyword.find("，取名");

                if (pos2 == -1) throw std::runtime_error("缺少变量名定义，请检查代码。行" + std::to_string(l));
                else {
                    
                    std::string type = keyword.substr(0, pos2);
                    std::string name = keyword.substr(pos2 + 9, keyword.size() - pos2 - 9);
                    //std::cout << name << std::endl;
                    
                    //查询该变量是否已经存在
                    auto it = variPool.find(type);
                    if (it != variPool.end()){
                        throw std::runtime_error("该变量已被定义，请检查代码。行" + std::to_string(l));
                    } else {
/*                            
                        // 根据类别创建变量
                        if (type == "Linux显示器") {
                            std::cout << "准备Linux显示器..." << std::endl;
                            this->variPool[name] = new BasicLinuxScreen(kvTable["配置—屏幕—帧缓冲"], kvTable["配置—屏幕—旋转"] == "真");
                        } else  {
                            throw std::runtime_error("变量类型错误，请检查代码。行" + std::to_string(l));
                        }
*/
                    }
                }

            } else {
                std::cout << msg << std::endl;
            }
            
            return true;
        }

        

        //-----------------------------------------------------------
        //-----------------------------------------------------------
        /* 以下是具体的操作 */

        void PathIn(std::string &msg, std::string &cur){
            /*从路径名定义的代码中提取路径名，添加到全局路径中*/

            //获取路径名
            std::string tg = msg.substr(3, msg.size() - 6);
            //std::cout << 目标 << std::endl;

            if (cur == "") cur = tg;
            else {
                cur += "—";
                cur += tg;
            }
        }

        void PathOut(std::string &curp, size_t l){
            /*检测到了路径名退出符号，将全局路径退出一层
            如果全局路径已经是空的，那就说明多写了一个路径结束符。
            如果全局路径不是空的，但是里面没有路径结束符号，那就说明全局路径将被清零
            如果路径分隔符既不在字符串的开头，又不在字符串的结尾，那就符合要求。*/
            size_t pos;

            if (curp == "") throw std::runtime_error("在读取CSC时出现了重复的结束符定义，请检查代码。行" + std::to_string(l));

            else if ( (pos = curp.rfind("—")) == -1 ) curp = "";
            else if (pos > 0 && pos < curp.size() - 3) {

                curp = curp.substr(0, pos);
                //std::cout << curp << std::endl;

            } else throw std::runtime_error("在读取CSC时出现了全局路径故障，请检查代码。行" + std::to_string(l));

        }

        void JoinLongkv(std::string &curp, std::unordered_map<std::string, std::string> &kvTable, std::string k, std::string &v){ 
            /*检测到了长值对，将其添加到字典中*/
            kvTable[std::string(curp + k)] = v;
        }

        void JoinKv(std::string &curp, std::unordered_map<std::string, std::string> &kvTable, std::string &msg){
            /*检测到了键值对声明，将其添加到字典中*/
            size_t pos = msg.find("：", 0);
            //
            std::string primerKey = msg.substr(0, pos);
            std::string k = (curp == "") ? primerKey : curp + "—" + primerKey;
            std::string v = msg.substr(pos + 3, msg.size() - pos - 3);
            //
            kvTable[k] = v;
        }

        bool Execute(std::string &msg)
        {
            //提取指示体
            std::string body = msg.substr(0, msg.size() - 3);
            //根据运行指示执行相应的操作
            // ...
            //std::cout << body << std::endl;
            return 0;
        }



    };

}

