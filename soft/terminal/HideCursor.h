#pragma once

#include <iostream>
#include"../../type.h"

namespace qing{

    class HideCursor : Declare{
    public:
        HideCursor();
        HideCursor(HideCursor&) = delete;
        ~HideCursor();
    private:
        static void set();
        static void back();
    };

}
