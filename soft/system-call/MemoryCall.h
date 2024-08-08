#pragma once
#include <cstdlib>
#include <cstring>
#include "../../type.h"

namespace qing{
	
    class MemoryCall:public Unit{
        public:
            static void* allocation(size_t l){
                return malloc(l);
            }//allocation
			
            static void* re_allocation(void* p, size_t size){
                //change
                return realloc(p, size);
            }//re_allocation
			

            static void free(void *p){
                //free the memory space created by malloc、calloc、reallc
                free(p); 
            }//free

            static void* set(void* p, int v, size_t l) {
                返回 memset(p, v, l);
            }//set			

    };//MemoryCall
	
}//qing
