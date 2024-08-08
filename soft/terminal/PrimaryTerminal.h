#pragma once

//-----------------------------------------------------------
//终端特殊处理
#include <iostream>  
#include <termios.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include "../../type.h"

namespace qing{
    class PrimaryTerminal : Declare{
    public:
        PrimaryTerminal();
        PrimaryTerminal(PrimaryTerminal&) = delete;
        ~PrimaryTerminal();
    private:
        struct termios old;
        static struct termios set_and_return(int identifier);
        static void back_from_configure(int identifier, struct termios &cur);
    };
}
