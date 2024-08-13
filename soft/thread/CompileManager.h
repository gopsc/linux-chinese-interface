#pragma once

#include <iostream>
#include <vector>
#include "../Printer/StandardPrinter.h"
#include "../script/DScript.h"
#include "CommonThread.h"

//extern qing::StandardPrinter *printer;
//extern qing::DScript *script;
 
namespace qing {
	
	class CompileManager: public CommonThread{
	/*
	 * 此类通过多线程方式编译代码
	 * 有一个线程编译失败时，应该关闭整个控制器
	 */		
	public:
	    /*构造函数*/
	    CompileManager(std::string name): CommonThread(name) {}
            /*析构函数，关闭线程*/
            ~CompileManager(){
                if (this->chk() != SSHUT){
                    this->close();//使线程自主关闭
                }
                this->WaitClose();//等待线程退出
            }
	    /*暂时删除复制构造函数*/
	    CompileManager(CompileManager&) = delete;

	    
        static std::vector<std::string> Split(std::string s, std::string word) {
			//将字符串分割，并装入向量中返回
			std::vector<std::string> vt;
			size_t l = word.size();
            size_t pos1 = -l, pos2 = 0;
            do {
                //开始分割字符串
			    pos2 = pos1 + l;
			    pos1 = s.find(word, pos2); 
			    std::string sub = pos1 != std::string::npos ? s.substr(pos2, pos1 - pos2) : s.substr(pos2, s.size() - pos2);
			    vt.insert(vt.begin(),sub);

			} while (pos1 != std::string::npos);
			
			return vt;
		}
	    
        void StopEvent() override {
            /*程序睡眠阶段的操作函数，线程挂起*/
            this->suspend();
        }/*静止事件*/
        
        
        void WakeEvent() override{
            /*程序在设置阶段的操作函数。*/
            std::string conf = ::script->Open("配置—编译管理器");
            std::vector<std::string> prjSet = Split(conf, "\n");
            for(long i=prjSet.size()-1; i>-1; --i){
				::printer->Print(this->GetLabel(),prjSet[i]);
			}
            /*单向线程*/
            this->close();
        }/*配置事件*/
        
        
        void LoopEvent() override {
            /*程序运行阶段的操作函数。*/
		}/*循环体*/
		

        void ClearEvent() override {
            /*程序在清理阶段的操作函数。*/
            //CommonThread::Print("清除。");
        }/*清除事件*/
		
		
	}; /*编译管理器*/
	
}/*qing*/
