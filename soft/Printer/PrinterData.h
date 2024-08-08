#pragma once

#include <string.h>
#include <iostream>
#include "../../type.h"

namespace qing {

    class PrinterData: public Unit{
        /* 单行打字机数据 */
        public:
            PrinterData(double t, std::string n, std::string s);
        //private:
            double		t;
            std::string	n;
            std::string	s;
    };
    
}
