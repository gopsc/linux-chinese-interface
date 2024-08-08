#ifndef __H_OS
#define __H_OS
#include<math.h>
#include<time.h>
#include"../type.h"
namespace qing{
    class Os :public Unit{
        public:
	    static double get_runtime();
    };//Os
}//qing
#endif//!__H_OS
