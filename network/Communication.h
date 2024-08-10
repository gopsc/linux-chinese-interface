
/* LCI简单数据交换协议：
 * 1.发送方发送此次数据的长度，格式为JSON
 * 2.接收方收到长度后发送反馈信息，格式为JSON。
 * 3.发送方接收反馈后发送数据体
 * 4.接收方根据数据长度创建缓冲区并接收数据。
 *
 * extra：
 * 1.在发送数据长度时，可以携带数据的加密方式信息。*/


#pragma once
#include <iostream>
#include "../type.h"


/* 这个缓冲区大小是用来创建缓冲区，以接收此次数据的长度信息的 */
#define BUFFER_SIZE 1024

namespace qing {

    class Communication: public Interface {
        //Basic and Universal Communication Interface
	/*基本和通用的数据交换接口，lci简单数据交换协议*/
        public:

    //=================================================================
        /* 使用简单LCI数据交换协议接收信息 */
        static std::string recv_msg(int sock);

    //=================================================================
        //Check if there is data available to read on the socket
        //检查套接字缓冲区中是否还有可以读的数据
        //
        //The parameter is a created socket descripor
        //参数是一个已经创建好的套接字描述符
        //
        //@qing,20240525: 第二次接收时可能会发生问题
        static int checkSocket(int sock_fd);

    //=================================================================
        /* 通过LCI协议发送数据 */
        static long send_msg(int sock, std::string s);
        
    };//交流

}//qing
