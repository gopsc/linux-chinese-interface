#ifndef __H_DISABLEECHO
#define __H_DISABLEECHO

#include"../../type.h"
//-----------------------------------------------------------
//终端特殊处理

#ifdef __cplusplus
#include <fcntl.h>  
#include <iostream>

#ifdef __linux__
#include <termios.h>  
#include <unistd.h>  
#endif //__linux__

namespace qing{
    class DisableEcho : Declare{
    public:
        DisableEcho();
        DisableEcho(DisableEcho&) = delete;
        ~DisableEcho();
    private:
        static void set();
        static void back();

    };
}

#endif //__cplusplus
#endif //__禁止回显_头
