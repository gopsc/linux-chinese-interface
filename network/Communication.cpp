
/* LCI简单数据交换协议：
 * 1.发送方发送此次数据的长度，格式为JSON
 * 2.接收方收到长度后发送反馈信息，格式为JSON。
 * 3.发送方接收反馈后发送数据体
 * 4.接收方根据数据长度创建缓冲区并接收数据。
 *
 * extra：
 * 1.在发送数据长度时，可以携带数据的加密方式信息。*/

#include "Communication.h"
#include "../soft/script/JsonInterface.h"

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../cJSON/cJSON.h"
#include "../type.h"

namespace qing {

    //=================================================================
    std::string Communication::recv_msg(int sock) {
        /* 使用简单LCI数据交换协议接收信息 */
	std::string s = "";
        double lengthValue = 0;
        //---------------------------------------------------------
        //---------------------------------------------------------
	{
            /* 接收并解析长度信息，格式为JSON字符串 */
            /* 创建第一缓冲区 */
            char buffer[BUFFER_SIZE+1];
            buffer[0] = '\0';
            /*从第一缓冲区接收长度信息*/
            ssize_t size = recv(sock, buffer, BUFFER_SIZE, 0);
            if (size == -1)
                throw RcvError(std::string("接收LCI长度信息。"));
            buffer[size] = '\0';
//std::cout << std::string(std::to_string(size)) + "<-第一次接收\n";
            /*解析位于第一缓冲区的JSON格式的长度信息*/
            cJSON *root = cJSON_Parse(buffer);
            cJSON *lengthObj = cJSON_GetObjectItem(root, "length");
            if (lengthObj && cJSON_IsNumber(lengthObj)){ /*找到了*/
                 lengthValue = cJSON_GetNumberValue(lengthObj);
            if (lengthValue < 0) /* 非法的长度 */
                throw RcValueError(std::string("非法的LCI长度") + std::to_string(lengthValue) + "。");
            } else /*没找到*/
                throw JsonInterface::JsonParseError(std::string("LCI长度信息。") + strerror(errno));
            /*释放掉解析的JSON对象*/
            cJSON_Delete(root);
	}
        //-------------------------------------------------------------
        //-------------------------------------------------------------

        //Check if there is data to read
        //检查缓冲区中是否还有数据（测试用）
        int len2 = checkSocket(sock);
        if (len2 > 0)
		throw RcvError(std::string("缓冲区剩余数据") + std::to_string(len2) + "。");

        //-------------------------------------------------------------
        //-------------------------------------------------------------
	{
            /* 准备回应的JSON字符串 */
            cJSON *root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "response", "1ok");
            char *jsonString = cJSON_Print(root);
            /* 发送回应的JSON字符串 */
            size_t n1 = strlen(jsonString);
            ssize_t n2 = send(sock, jsonString, n1, 0);
            if (n1 != n2)
		throw SndError(std::string("发送第一次LCI回应。"));           
            /* 释放打印的JSON字符串，以及创建的JSON对象 */
            free(jsonString);
            cJSON_Delete(root);
	}
        //-------------------------------------------------------------
	{
            /* 准备用于接收数据体的第二缓冲区 */
            char *recvBuf = (char*)malloc((lengthValue + 1) * sizeof(char));
            *recvBuf = '\0';
            /* 接收数据体 */
            ssize_t size = recv(sock, recvBuf, lengthValue, 0);
            if (size == -1 || size != lengthValue)
                throw RcvError(std::string("接收LCI数据体。"));
            recvBuf[size] = '\0';
            /*获取数据体，释放创建的第二缓冲区*/
            s = recvBuf;
            free(recvBuf);
	}
        //-------------------------------------------------------------
        //-------------------------------------------------------------

        //Check if there is data to read
        //检查缓冲区中是否还有数据（测试用）
        len2 = checkSocket(sock);
        if (len2 > 0)
		throw RcvError(std::string("缓冲区剩余数据") + std::to_string(len2) + "。");

        //-------------------------------------------------------------
        //-------------------------------------------------------------
	{

            // 准备回应的JSON字符串
            cJSON *root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "response", "2ok");
            char *jsonString = cJSON_Print(root);
            // 发送回应的JSON字符串
            size_t n1 = strlen(jsonString);
            ssize_t n2 = send(sock, jsonString, n1, 0);
//std::cout << std::string(std::to_string(n1)) + ":" + std::string(std::to_string(n2)+"<-第四次发送\n");
            if (n1 != n2)
		throw SndError(std::string("发送第二次LCI回应。"));           
            // 释放打印的JSON字符串，以及创建的JSON对象 
            free(jsonString);
            cJSON_Delete(root);

	}

        //返回接收到的LCI数据体
        return s;

    }/*接收信息*/


    //=================================================================
    long Communication::send_msg(int sock, std::string s) {
        /* 通过LCI协议发送数据 */

        /* 获取该消息的长度 */
        size_t l1 = s.size(), l2 = 0;

        //-------------------------------------------------------------
        /* 创建JSON格式的消息长度信息 */
	{
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "length", l1);
            char *jsonString = cJSON_Print(root);
            /* 发送长度信息 */
            size_t n1 = strlen(jsonString);
            ssize_t n2 = send(sock, jsonString, n1, 0);
            if (n1 != n2)
                throw SndError(std::string("发送长度信息。"));           
//std::cout << std::string(std::to_string(n1)) + ":" + std::string(std::to_string(n2)+"<-第一次发送\n");
            /* 释放打印出的JSON字符串，以及JSON对象 */
            free(jsonString);
            cJSON_Delete(root);
	}
        //-------------------------------------------------------------
        //-------------------------------------------------------------
	{
            /* 创建缓冲区，以接收回应 */
            char buffer[BUFFER_SIZE+1];
            buffer[0] = '\0';
            /* 接收回应信息 */
            ssize_t size = recv(sock, buffer, BUFFER_SIZE, 0);
            if (size == -1)
		throw RcvError(std::string("接收第一次回应。"));
            buffer[size] = '\0';
            /* 解析回应信息 */
            cJSON *root = cJSON_Parse(buffer);
            cJSON *responseObj = cJSON_GetObjectItem(root, "response");
            if (responseObj && cJSON_IsString(responseObj)){
               const char *responseValue = cJSON_GetStringValue(responseObj);
                 if (strcmp(responseValue, "1ok") != 0)
                    throw RcValueError(std::string("第一次回应消息") + responseValue + "非法");
            } else
                throw JsonInterface::JsonParseError(std::string("第一次回应信息") + strerror(errno));
            /* 删除解析出的JSON对象 */
            cJSON_Delete(root);
	}
        //-------------------------------------------------------------
        //-------------------------------------------------------------

        //Check if there is data to read
        //检查缓冲区中是否还有数据（测试用）
        int len2 = checkSocket(sock);
        if (len2 > 0)
		throw RcvError(std::string("缓冲区剩余数据") + std::to_string(len2) + "。");

        //-------------------------------------------------------------
	{
            /* 发送数据体 */
            l2 = send(sock, s.c_str(), l1, 0);
            if (l1 != l2)
                throw SndError(std::string("发送消息。"));
	}
        //-------------------------------------------------------------
	{

            //创建缓冲区，以接收第二次回应
            char buffer[BUFFER_SIZE+1];
            buffer[0] = '\0';
            // 接收回应信息
            ssize_t size = recv(sock, buffer, BUFFER_SIZE, 0);
            if (size == -1)
		throw RcvError(std::string("接收第二次回应。 "));
            buffer[size] = '\0';
//std::cout << std::string(std::to_string(size)+"<-第四次接收\n");
            //解析回应信息
            cJSON *root = cJSON_Parse(buffer);
            cJSON *responseObj = cJSON_GetObjectItem(root, "response");
            if (responseObj && cJSON_IsString(responseObj)){
                const char *responseValue = cJSON_GetStringValue(responseObj);
                if (strcmp(responseValue, "2ok") != 0)
                    throw RcValueError(std::string("第二次回应") + responseValue + "非法");
            } else
                throw JsonInterface::JsonParseError(std::string("第二次回应信息") + strerror(errno));
            // 删除解析出的JSON对象 
            cJSON_Delete(root);

	}
        //-------------------------------------------------------------

        //Check if there is data to read
        //检查缓冲区中是否还有数据（测试用）
        len2 = checkSocket(sock);
        if (len2 > 0)
	    throw RcvError(std::string("缓冲区剩余数据") + std::to_string(len2) + "。");

        //-------------------------------------------------------------
	//-------------------------------------------------------------
        return l1;
    }//发送数据
        
    //=================================================================
    int Communication::checkSocket(int sock_fd){
        //Check if there is data available to read on the socket
        //检查套接字缓冲区中是否还有可以读的数据
        //
        //The parameter is a created socket descripor
        //参数是一个已经创建好的套接字描述符
        //
        //@qing,20240525: 第二次接收时可能会发生问题

        fd_set readfds;            //定义一个文件描述符集合变量readfds
        FD_ZERO(&readfds);         //初始化readfds集合，将所有位设置为0
        FD_SET(sock_fd, &readfds); //将套接字文件描述符sock_fd添加到readfds集合中

        //设置堵塞时间
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        //使用select函数来检查套接字sock_fd上是否有数据可读
        //因为文件描述符的范围是从0开始的，所以要加1以包括套接字本身
        int retval = select(sock_fd + 1, &readfds, NULL, NULL, &tv);

        if (retval == -1) {
            // select调用失败
            throw ChkRcvBufError();
        } else if (retval > 1) {
            //数据可以被读取
            return retval;
        } else {//结果等于1，可能是有数据，也可能是
            //select调用成功了, 但是没有可读数据
            //堵塞时间到了或是别的文件描述符就绪了
            return retval - 1;
        }
    }


}//qing
