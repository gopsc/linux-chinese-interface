#include"os.h"
namespace qing{
    double Os::get_runtime(){
        struct timespec t1;
	if(clock_gettime(CLOCK_MONOTONIC_RAW, &t1) == -1){return -1.0;}
	double ts = (double)t1.tv_sec;
	double ms = (double)t1.tv_nsec / 1000000000;
	return std::round((ts + ms) * 100.0) / 100.0;
    }//get_runtime
}//qing
