#pragma once

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include "../../cJSON/cJSON.h"
#include "../../network/Communication.h"
#include "../script/DScript.h"
#include "../script/JsonInterface.h"
#include "../Printer/StandardPrinter.h"
#include "CommonThread.h"

/*main.cpp*/
extern bool isDebug;
/*定义在main.c中的输出管理类型*/
//extern qing::StandardPrinter *printer;
/*定义在main.c中的全局脚本类型*/
//extern qing::DScript *script;

namespace qing {
	
	class Connector: public CommonThread{
	    /*服务器获取连接后，创建该线程与客户端通信
        连接关闭后，也许通过信号机制删除连接池中的连接*/
	public:
	    Connector(std::string name, int sock): CommonThread(name) {
			this->sock = sock;
		}/*构造函数*/
	    /*析构函数，关闭线程*/
	    ~Connector(){
                if (this->chk() != SSHUT){
                    this->close();//使线程自主关闭
                }
                this->WaitClose();//等待线程退出
	    }
		
	    /*暂时删除复制构造函数*/
	    Connector(Connector&) = delete;
        
        void StopEvent() override {
            /*程序睡眠阶段的操作函数，什么也不做*/
            this->suspend();
        }/*静止事件*/
        
            
        void WakeEvent() override {
            /*程序设置阶段的操作函数。*/

            try {

                //sock->setBlockTime(2);
                {
                    struct timeval t;
                    t.tv_sec = 1;
                    t.tv_usec = 0;
                    
                    if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("设置套接字堵塞时间失败。") + strerror(errno));
                        
                    if (setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("设置套接字堵塞时间失败。") + strerror(errno));
                }

		{
		    int flag = 1;

		    //设置套接字选项以关闭Nagle算法
		    if(setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
			throw std::runtime_error(std::string("关闭Nagle算法失败。"));
		}

                //::printer->Print("","初始化成功。");
                this->run();
            } catch (std::exception& e) {/*设置阶段异常*/
                ::printer->Print("",std::string("连接器程序在设置阶段出现异常：") + e.what());
                this->stop(); return;
            }/*设置阶段异常*/
            
		}/*配置事件*/
		
             
        void LoopEvent() override {
            /*程序运行阶段的操作函数。开始交流*/

            try {
                /*一会儿我们将其改为以事件队列驱动
		 *
		 * 接收和发送LCI消息*/

                /*接收数据*/
                if (isDebug) printer->Print(this->GetLabel(),"recive");
                std::string RcvData = "";
		RcvData = Communication::recv_msg(this->sock);
                    
                /*读取接收到的指示*/
		std::string InstructionGot;
                if(isDebug) printer->Print(this->GetLabel(),"analyze");
                //--------------------------------------------------------------------------------
                /*检查简报中的指示*/
                {
                    /*解析收到的数据*/
                    cJSON *dataReport = cJSON_Parse(RcvData.c_str());

            	    cJSON *communicationItem = cJSON_GetObjectItem(dataReport, "通信");
            	    if(!communicationItem || !cJSON_IsObject(communicationItem))
			throw JsonInterface::JsonParseError("非法的LCI消息。");

            	    cJSON *InstructionItem = cJSON_GetObjectItem(communicationItem, "指示");
            	    if(!InstructionItem || !cJSON_IsString(InstructionItem))
			throw JsonInterface::JsonParseError("非法的LCI消息。");

		    /*获取消息中的指令*/
            	    InstructionGot = std::string(InstructionItem->valuestring);

		    /*清理*/
                    cJSON_Delete(dataReport);

                }

                //--------------------------------------------------------------------------------
                /*尝试将数据保存到全局配置*/

                std::string InstructionSend = "关闭连接";
                if (InstructionGot == "关闭连接"){
                    this->close();/*出口*/
                }
                else if	(InstructionGot == "保持联络") InstructionSend = "保持联络";
	       else {
	           this->close();
		   ::printer->Print("",InstructionGot);
	      }
                
                if(isDebug) ::printer->Print(this->GetLabel(),"prepare");
                std::string msg;
                //--------------------------------------------------------------------------------
                /*准备要发送的脚本，服务器不主动关闭连接*/
                {
            	    cJSON *reportItem = cJSON_CreateObject();
                    std::string name = ::script->Open("简报—名字");
                	cJSON_AddItemToObject(reportItem, "名字", cJSON_CreateString(name.c_str()));

	                cJSON *communicationItem = cJSON_CreateObject();
                	cJSON_AddItemToObject(communicationItem, "指示", cJSON_CreateString(InstructionSend.c_str()));

			cJSON *dataReport = cJSON_CreateObject();
        	        cJSON_AddItemToObject(dataReport, "简报", reportItem);
        	        cJSON_AddItemToObject(dataReport, "通信", communicationItem);
			/*打印*/
			char *jsonString = cJSON_Print(dataReport);
                        msg = std::string(jsonString);
			/*清理*/
                        free(jsonString);
                        cJSON_Delete(dataReport);
                }
                //--------------------------------------------------------------------------------
                if (isDebug) ::printer->Print(this->GetLabel(),"send");

                /*发送数据*/
                //sock->send_msg(sock->getSocket(), DataReport_out.toStr());
                Communication::send_msg(this->sock, msg);    
                    
            }
	    catch (Communication::CommuniError& e) {
                ::printer->Print("",std::string("连接断开，因为") + e.what());
                this->close();
            }/*通信异常*/
	    
	    catch (JsonInterface::JsonParseError& e){
	        ::printer->Print("",std::string("解析失败，因为") + e.what());
		this->close();
	    }
            // catch (std::exception& e) {
            // }

	}/*循环体*/
            

        void ClearEvent() override {
            /*程序清理阶段的操作函数。*/
            
            try {
                if (this->sock != -1) {
                    /*如果Linux服务器类型已经被设置过了
                    就将其析构，如果没有什么也不做*/
                    ::close(this->sock);
                    this->sock = -1;
                    /*执行成功*/
                    //::printer->Print("","清除。");
                } /*删除伺服器*/

            } catch(std::exception e) {
                ::printer->Print("",std::string("清理时出错：") + e.what());
                this->stop();
            }/*清理阶段异常*/
            
        }/*清除事件*/


        private:
    
            //ClientSocket *sock;
            int sock;

    
	};/*连接器*/
	
}/*qing*/
